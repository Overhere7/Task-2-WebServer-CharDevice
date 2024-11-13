#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "char_device"
#define BUFFER_SIZE 1024

static int major;               // Major number assigned to our device driver
static char buffer[BUFFER_SIZE]; // Buffer for storing data
static int buffer_pos = 0;       // Position in buffer
static struct cdev char_cdev;    // Character device structure

// Function to handle open system call
static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

// Function to handle read system call
static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset) {
    int bytes_to_read = min((int)size, buffer_pos - (int)(*offset));

    if (bytes_to_read == 0) {
        printk(KERN_INFO "End of buffer reached\n");
        return 0; // End of file
    }

    if (copy_to_user(user_buffer, buffer + *offset, bytes_to_read) != 0) {
        return -EFAULT;
    }

    *offset += bytes_to_read;
    printk(KERN_INFO "Read %d bytes from device\n", bytes_to_read);
    return bytes_to_read;
}

// Function to handle write system call
static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset) {
    int bytes_to_write = min((int)size, BUFFER_SIZE - buffer_pos);

    if (copy_from_user(buffer + buffer_pos, user_buffer, bytes_to_write) != 0) {
        return -EFAULT;
    }

    buffer_pos += bytes_to_write;
    printk(KERN_INFO "Wrote %d bytes to device\n", bytes_to_write);
    return bytes_to_write;
}

// Function to handle close system call
static int dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

// Initialize the module
static int __init char_device_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", major);
        return major;
    }
    printk(KERN_INFO "Device registered with major number %d\n", major);
    return 0;
}

// Cleanup function
static void __exit char_device_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(char_device_init);
module_exit(char_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver");
