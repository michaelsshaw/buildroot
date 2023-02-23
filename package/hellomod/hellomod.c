#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h> /* Needed for the macros */
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

#define DSIF_COUNT 16
#define DSIF_BUFSIZE 8192

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Shaw");
MODULE_DESCRIPTION("I hope this works");
MODULE_VERSION("1.0");

static dev_t dev;
static struct class *dev_class;

static int __init dsif_init(void)
{
	if (alloc_chrdev_region(&dev, 0, DSIF_COUNT, "dsif") < 0) {
		pr_err("Unable to allocate DSIF chrdev region\n");
		return -1;
	}

	dev_class = class_create(THIS_MODULE, "dsif_class\n");
	if (IS_ERR(dev_class)) {
		pr_err("Cannot create DSIF device class\n");
		goto fail_region;
	}

	if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "dsif0"))) {
		pr_err("Failed to create DSIF device\n");
		goto fail_class;
	}

	printk(KERN_INFO "DSIF driver loaded\n");
	return 0;

fail_class:
	class_destroy(dev_class);
fail_region:
	unregister_chrdev_region(dev, DSIF_COUNT);

	return -1;
}

static void __exit dsif_cleanup(void)
{
	class_destroy(dev_class);
	unregister_chrdev_region(dev, DSIF_COUNT);
	printk(KERN_INFO "Unloading DSIF driver...\n");
}

module_init(dsif_init);
module_exit(dsif_cleanup);
