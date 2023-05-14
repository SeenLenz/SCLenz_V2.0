#include <asm/errno.h>
#include <linux/atomic.h>
#include <linux/cdev.h>

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/kernel.h> /* for sprintf() */
#include <linux/module.h>

MODULE_LICENSE("GPL");

struct input_dev *vkb;

static int __init ex1_init(void) {

  /* extra safe initialization */
  vkb = input_allocate_device();

  /* set up descriptive labels */
  vkb->name = "VirtualKB used by SCLENZ";
  vkb->phys = "Virtual";
  vkb->evbit[0] = BIT_MASK(EV_KEY);
  vkb->keybit[BIT_WORD(BTN_0)] = BIT_MASK(BTN_0);

  /* and finally register with the input core */
  input_register_device(vkb);

  return 0;
}

static void __exit ex1_exit(void) { input_unregister_device(vkb); }

module_init(ex1_init);
module_exit(ex1_exit);
