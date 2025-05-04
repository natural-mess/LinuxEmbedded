#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>

#define DRIVER_AUTHOR "Rocky"
#define DRIVER_DESC "GPIO LED control driver for Raspberry Pi 4"
#define DRIVER_VERS "1.0"

// GPIO registers for BCM2711 (Raspberry Pi 4, BCM2711 ARM Peripherals Revision 4, 18 Jan 2022)
#define GPIO_BASE 0xFE200000 // Physical base address (Chapter 1, Section 1.2)
#define GPFSEL2 0x08         // Offset for GPFSEL2 (Chapter 5, Section 5.2)
#define GPSET0 0x1C          // Offset for GPSET0 (Chapter 5, Section 5.2)
#define GPCLR0 0x28          // Offset for GPCLR0 (Chapter 5, Section 5.2)
#define GPIO_27 27           // GPIO27 (PIN13, verified via Section 5.3)
#define FSEL27_SHIFT 21      // Bits 23-21 for FSEL27 in GPFSEL2 (Section 5.2)
#define OUTPUT 0x1           // 001 for output mode

// Character device buffer size
#define BUFFER_SIZE 10

struct m_led_dev
{
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
    void __iomem *gpio_base; // Virtual address of GPIO registers
    char *buffer;            // Buffer for user input ("on"/"off")
};

static struct m_led_dev mdev;

// GPIO helper functions
static void gpio_set_output(void __iomem *gpfsel2)
{
    u32 val = readl(gpfsel2);
    val &= ~(7 << FSEL27_SHIFT);     // Clear FSEL27 bits
    val |= (OUTPUT << FSEL27_SHIFT); // Set FSEL27 to output (001)
    writel(val, gpfsel2);
}

static void gpio_set(void __iomem *gpset0)
{
    writel(1 << GPIO_27, gpset0); // Set GPIO27 high
}

static void gpio_clear(void __iomem *gpclr0)
{
    writel(1 << GPIO_27, gpclr0); // Clear GPIO27 low
}

// File operations
static int m_open(struct inode *inode, struct file *file)
{
    pr_info("LED device opened\n");
    return 0;
}

static int m_release(struct inode *inode, struct file *file)
{
    pr_info("LED device closed\n");
    return 0;
}

static ssize_t m_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_write = min(size, (size_t)BUFFER_SIZE - 1);

    // Clear buffer
    memset(mdev.buffer, 0, BUFFER_SIZE);

    // Copy user data
    if (copy_from_user(mdev.buffer, user_buf, to_write))
    {
        pr_err("Failed to copy data from user\n");
        return -EFAULT;
    }

    // Null-terminate buffer
    mdev.buffer[to_write] = '\0';

    pr_info("Received: %s\n", mdev.buffer);

    // Toggle LED based on input
    if (strncmp(mdev.buffer, "on", 2) == 0)
    {
        gpio_set(mdev.gpio_base + GPSET0);
        pr_info("LED turned ON\n");
    }
    else if (strncmp(mdev.buffer, "off", 3) == 0)
    {
        gpio_clear(mdev.gpio_base + GPCLR0);
        pr_info("LED turned OFF\n");
    }
    else
    {
        pr_info("Invalid command (use 'on' or 'off')\n");
    }

    return to_write;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = m_open,
    .release = m_release,
    .write = m_write,
};

// Module initialization
static int __init led_init(void)
{
    // Allocate character device region
    if (alloc_chrdev_region(&mdev.dev_num, 0, 1, "m_led") < 0)
    {
        pr_err("Failed to allocate chrdev region\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));

    // Create class
    if ((mdev.m_class = class_create("m_led")) == NULL)
    {
        pr_err("Cannot create class\n");
        goto rm_device_numb;
    }

    // Create device
    if (device_create(mdev.m_class, NULL, mdev.dev_num, NULL, "m_led") == NULL)
    {
        pr_err("Cannot create device\n");
        goto rm_class;
    }

    // Initialize cdev
    cdev_init(&mdev.m_cdev, &fops);
    if (cdev_add(&mdev.m_cdev, mdev.dev_num, 1) < 0)
    {
        pr_err("Cannot add device to system\n");
        goto rm_device;
    }

    // Allocate buffer for user input
    mdev.buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!mdev.buffer)
    {
        pr_err("Failed to allocate buffer\n");
        goto rm_cdev;
    }

    // Map GPIO registers
    mdev.gpio_base = ioremap(GPIO_BASE, 0x100);
    if (!mdev.gpio_base)
    {
        pr_err("Failed to map GPIO registers\n");
        goto rm_buffer;
    }

    // Configure GPIO27 as output
    gpio_set_output(mdev.gpio_base + GPFSEL2);

    // Turn LED on
    gpio_set(mdev.gpio_base + GPSET0);
    pr_info("LED driver loaded, LED ON\n");

    return 0;

rm_buffer:
    kfree(mdev.buffer);
rm_cdev:
    cdev_del(&mdev.m_cdev);
rm_device:
    device_destroy(mdev.m_class, mdev.dev_num);
rm_class:
    class_destroy(mdev.m_class);
rm_device_numb:
    unregister_chrdev_region(mdev.dev_num, 1);
    return -1;
}

// Module cleanup
static void __exit led_exit(void)
{
    // Turn LED off
    gpio_clear(mdev.gpio_base + GPCLR0);

    // Unmap GPIO registers
    iounmap(mdev.gpio_base);

    // Free buffer
    kfree(mdev.buffer);

    // Clean up character device
    cdev_del(&mdev.m_cdev);
    device_destroy(mdev.m_class, mdev.dev_num);
    class_destroy(mdev.m_class);
    unregister_chrdev_region(mdev.dev_num, 1);

    pr_info("LED driver unloaded, LED OFF\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);