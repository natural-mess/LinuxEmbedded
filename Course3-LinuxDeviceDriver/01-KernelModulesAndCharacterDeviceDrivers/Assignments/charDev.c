#include <linux/module.h>  /* Define module_init(), module_exit() */
#include <linux/fs.h>      /* Define alloc_chrdev_region(), register_chrdev_region() */
#include <linux/device.h>  /* Define device_create(), class_create() */
#include <linux/cdev.h>    /* Define cdev_init(), cdev_add() */
#include <linux/uaccess.h> /* Define copy_to_user(), copy_from_user() */
#include <linux/slab.h>    /* Define kfree() */

#define DRIVER_AUTHOR "Rocky"
#define DRIVER_DESC "Read and write data from and to kernel"
#define DRIVER_VERS "1.0"

#define NPAGES 1

struct m_foo_dev
{
    int32_t size;
    char *kmalloc_ptr;
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
} mdev;

static int m_open(struct inode *inode, struct file *file)
{
    pr_info("System call open() called...!!!\n");
    return 0;
}

static int m_release(struct inode *inode, struct file *file)
{
    pr_info("System call close() called...!!!\n");
    return 0;
}

static ssize_t m_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_read;
    pr_info("System call read() called...!!!\n");
    to_read = (size > mdev.size - *offset) ? (mdev.size - *offset) : size;
    if (copy_to_user(user_buf, mdev.kmalloc_ptr + *offset, to_read))
        return -EFAULT;
    *offset += to_read;
    return to_read;
}

static ssize_t m_write(struct file *filp, const char *user_buf, size_t size, loff_t *offset)
{
    size_t to_write;
    pr_info("System call write() called...!!!\n");
    to_write = (size + *offset > NPAGES * PAGE_SIZE) ? (NPAGES * PAGE_SIZE - *offset) : size;
    memset(mdev.kmalloc_ptr, 0, NPAGES * PAGE_SIZE);
    if (copy_from_user(mdev.kmalloc_ptr + *offset, user_buf, to_write) != 0)
        return -EFAULT;
    pr_info("Data from usr: %s", mdev.kmalloc_ptr);
    *offset += to_write;
    mdev.size = *offset;
    return to_write;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = m_read,
    .write = m_write,
    .open = m_open,
    .release = m_release,
};

static int __init chdev_init(void)
{
    // Allocate kernel buffer
    mdev.kmalloc_ptr = kmalloc(NPAGES * PAGE_SIZE, GFP_KERNEL);
    if (!mdev.kmalloc_ptr)
    {
        pr_err("Failed to allocate kernel buffer\n");
        return -ENOMEM;
    }

    if (alloc_chrdev_region(&mdev.dev_num, 0, 1, "m_cdev") < 0)
    {
        pr_err("Failed to alloc chrdev region\n");
        goto free_buffer;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));

    if ((mdev.m_class = class_create("m_class")) == NULL)
    {
        pr_err("Cannot create the struct class for my device\n");
        goto rm_device_numb;
    }

    if ((device_create(mdev.m_class, NULL, mdev.dev_num, NULL, "m_cdev")) == NULL)
    {
        pr_err("Cannot create my device\n");
        goto rm_class;
    }

    cdev_init(&mdev.m_cdev, &fops);
    if ((cdev_add(&mdev.m_cdev, mdev.dev_num, 1)) < 0)
    {
        pr_err("Cannot add the device to the system\n");
        goto rm_device;
    }

    pr_info("Character device driver loaded\n");
    return 0;

rm_device:
    device_destroy(mdev.m_class, mdev.dev_num);
rm_class:
    class_destroy(mdev.m_class);
rm_device_numb:
    unregister_chrdev_region(mdev.dev_num, 1);
free_buffer:
    kfree(mdev.kmalloc_ptr);
    return -1;
}

static void __exit chdev_exit(void)
{
    kfree(mdev.kmalloc_ptr);
    cdev_del(&mdev.m_cdev);
    device_destroy(mdev.m_class, mdev.dev_num);
    class_destroy(mdev.m_class);
    unregister_chrdev_region(mdev.dev_num, 1);
    pr_info("Goodbye\n");
}

module_init(chdev_init);
module_exit(chdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);