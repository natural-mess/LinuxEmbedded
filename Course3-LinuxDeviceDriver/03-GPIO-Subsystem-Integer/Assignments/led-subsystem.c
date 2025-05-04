#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "led_gpio"
#define CLASS_NAME "led"

static int major_number;
static struct class *led_class = NULL;
static struct device *led_device = NULL;
static struct cdev led_cdev;
static int led_gpio = 27; // GPIO27 (PIN13)

static ssize_t led_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    char command[10] = {0};
    if (len > sizeof(command) - 1)
        return -EINVAL;
    if (copy_from_user(command, buf, len))
        return -EFAULT;
    command[len] = '\0';

    if (strncmp(command, "on", 2) == 0) {
        gpio_set_value(led_gpio, 1);
        printk(KERN_INFO "LED on\n");
    } else if (strncmp(command, "off", 3) == 0) {
        gpio_set_value(led_gpio, 0);
        printk(KERN_INFO "LED off\n");
    } else {
        return -EINVAL;
    }
    return len;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = led_write,
};

static int __init led_init(void) {
    int ret;

    ret = gpio_request(led_gpio, "led");
    if (ret) {
        printk(KERN_ERR "Failed to request GPIO %d\n", led_gpio);
        return ret;
    }

    ret = gpio_direction_output(led_gpio, 0);
    if (ret) {
        printk(KERN_ERR "Failed to set GPIO %d direction\n", led_gpio);
        gpio_free(led_gpio);
        return ret;
    }

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ERR "Failed to register character device\n");
        gpio_free(led_gpio);
        return major_number;
    }

    led_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(led_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        gpio_free(led_gpio);
        return PTR_ERR(led_class);
    }

    led_device = device_create(led_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        class_destroy(led_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        gpio_free(led_gpio);
        return PTR_ERR(led_device);
    }

    printk(KERN_INFO "LED GPIO module loaded\n");
    return 0;
}

static void __exit led_exit(void) {
    device_destroy(led_class, MKDEV(major_number, 0));
    class_destroy(led_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    gpio_free(led_gpio);
    printk(KERN_INFO "LED GPIO module unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("GPIO Integer-Based LED Control for Raspberry Pi 4");