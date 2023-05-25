#include <X11/Xlib.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <unistd.h>

const char char_to_key[248];

void emit(int fd, int type, int code, int val) {
  struct input_event ie;

  ie.type = type;
  ie.code = code;
  ie.value = val;
  /* timestamp values below are ignored */
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;

  write(fd, &ie, sizeof(ie));
}

int init(int fd, struct uinput_setup *usetup) {

  ioctl(fd, UI_SET_EVBIT, EV_KEY);

  for (int i = 1; i <= 248; i++) {
    ioctl(fd, UI_SET_KEYBIT, i);
  }

  memset(usetup, 0, sizeof(usetup));
  usetup->id.bustype = BUS_USB;
  usetup->id.vendor = 0x1234;  /* sample vendor */
  usetup->id.product = 0x5678; /* sample product */
  strcpy(usetup->name, "vkb_sclenz, virtual input device");

  ioctl(fd, UI_DEV_SETUP, usetup);
  ioctl(fd, UI_DEV_CREATE);
  return 0;
}

void sendkey(int fd, int key) {
  emit(fd, EV_KEY, key, 1);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  emit(fd, EV_KEY, key, 0);
  emit(fd, EV_SYN, SYN_REPORT, 0);
}

int main(void) {

  int ifinst = inotify_init();
  inotify_add_watch(ifinst, "/home/lenz/Documents/asdf.txt", IN_ACCESS);

  for (;;) {
    struct inotify_event ev;
    int i;
    if (i = read(ifinst, &ev, sizeof(ev))) {
      fprintf(stderr, "An error occured when opening the file: %d", i);
    }

    printf("the file had been opened");
  }

  struct uinput_setup usetup;

  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  init(fd, &usetup);

  sleep(1);

  sendkey(fd, KEY_B);

  sleep(1);

  ioctl(fd, UI_DEV_DESTROY);

  close(fd);
  close(ifinst);
  return 0;
}