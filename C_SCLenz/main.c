#include <X11/Xlib.h>
#include <dirent.h>
#include <linux/inotify.h>
#include <linux/input.h>
#include <linux/ioctl.h>
#include <linux/uinput.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
int contains(char str[256], char target[256]) {
  int i = 0, ti = 0;

  while (str[i] != '\0' && i < 255) {

    if (str[i] == target[ti]) {
      ti++;
    } else {
      ti = 0;
    }
    i++;
    if (target[ti] == '\0') {
      return 1;
    }
  }

  return 0;
}

int dirlen(char target[256]) {
  struct dirent *entry;
  int i = 0;

  for (; (entry = readdir(opendir(target))); i++) {
  }

  return i;
}

int main(int argc, char **argv) {
  char iobuff[256];
  unsigned int iomods = 0x0000000;
  DIR *folder = opendir("/dev/input/by-path");
  struct dirent *entry;
  int devices[8];
  int iodevc = 0;
  int iodirlen = 0;

  // Close the event file
  if (!folder) {
    perror("error reading the directory");
    return 1;
  }

  while ((entry = readdir(folder))) {
    char str[256];
    sprintf(str, "/dev/input/by-path/%s", entry->d_name);

    if (contains(entry->d_name, "kbd")) {
      if (iodevc < 8) {
        devices[iodevc] = open(str, O_RDONLY);
        if (devices[iodevc] < 0) {
          perror("error with opening the file:");
          return 1;
        }
        iodevc++;
      }
    }
    iodirlen++;
  }
  for (int i = 0; i < iodevc; i++) {
    printf("File descriptor %d: %d\n", i, devices[i]);
  }
  struct input_event event;
  ssize_t bytesRead;
  struct pollfd fds[iodevc];
  for (int i = 0; i < iodevc; i++) {
    fds[i].fd = devices[i];
    fds[i].events = POLLIN;
  }

  while (1) {
    int result = poll(fds, iodevc, -1);
    if (result < 0) {
      perror("Failed to poll events");
      break;
    }

    for (int i = 0; i < iodevc; i++) {
      if (fds[i].revents & POLLIN) {
        ssize_t bytesRead = read(fds[i].fd, &event, sizeof(event));
        if (bytesRead == -1) {
          perror("Failed to read event");
          break;
        } else if (bytesRead != sizeof(event)) {
          fprintf(stderr, "Incomplete event read\n");
          break;
        }

        // Process the event
        printf("Event: time=%ld.%06ld, type=%d, code=%s, value=%d\n",
               event.time.tv_sec, event.time.tv_usec, event.type,
               XKeysymToString, event.value);

        fflush(stdout); // Flush the output buffer

        // Add your event processing logic here
        // ...

        // Break the loop if you want to exit
        // ...
      }
    }
  }

  return 0;
}

// const char char_to_key[248];

// #define EVENT_SIZE (sizeof(struct inotify_event))
// #define EVENT_BUF_LEN (16 * (EVENT_SIZE + 16))

// void emit(int fd, int type, int code, int val) {
//   struct input_event ie;

//   ie.type = type;
//   ie.code = code;
//   ie.value = val;
//   /* timestamp values below are ignored */
//   ie.time.tv_sec = 0;
//   ie.time.tv_usec = 0;

//   write(fd, &ie, sizeof(ie));
// }

// int uinput_init(int fd, struct uinput_setup *usetup) {

//   ioctl(fd, UI_SET_EVBIT, EV_KEY);

//   for (int i = 1; i <= 248; i++) {
//     ioctl(fd, UI_SET_KEYBIT, i);
//   }

//   memset(usetup, 0, sizeof(usetup));
//   usetup->id.bustype = BUS_USB;
//   usetup->id.vendor = 0x1234;  /* sample vendor */
//   usetup->id.product = 0x5678; /* sample product */
//   strcpy(usetup->name, "vkb_sclenz, virtual input device");

//   ioctl(fd, UI_DEV_SETUP, usetup);
//   ioctl(fd, UI_DEV_CREATE);
//   return 0;
// }

// int ev_init() { return 0; }

// void sendkey(int fd, int key) {
//   emit(fd, EV_KEY, key, 1);
//   emit(fd, EV_SYN, SYN_REPORT, 0);
//   emit(fd, EV_KEY, key, 0);
//   emit(fd, EV_SYN, SYN_REPORT, 0);
// }

// int main(void) {
//   char buffer[EVENT_BUF_LEN];
//   int length, i = 0;
//   int fd;
//   int wd;
//   fd = inotify_init();

//   if (fd < 0) {
//     perror("inotify_init");
//   }

//   wd =
//       inotify_add_watch(fd, "/home/lenz/Documents/test", IN_CREATE |
//       IN_DELETE);

//   while (1) {
//     printf("loop 1");
//     length = read(fd, buffer, EVENT_BUF_LEN);

//     if (length < 0) {
//       perror("read");
//     }

//     while (i < length) {
//       struct inotify_event *event = (struct inotify_event *)&buffer[i];
//       if (event->len) {
//         if (event->mask & IN_CREATE) {
//           if (event->mask & IN_ISDIR) {
//             printf("New directory %s created.\n", event->name);
//           } else {
//             printf("New file %s created.\n", event->name);
//           }
//         } else if (event->mask & IN_DELETE) {
//           if (event->mask & IN_ISDIR) {
//             printf("Directory %s deleted.\n", event->name);
//           } else {
//             printf("File %s deleted.\n", event->name);
//           }
//         }
//       }
//       i += EVENT_SIZE + event->len;
//     }
//     i = 0;
//   }

//   return 0;
// }
