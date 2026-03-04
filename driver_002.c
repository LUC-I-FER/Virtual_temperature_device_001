#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define DEVICE_NAME "driver_002"
#define CLASS_NAME  "driver_002_class"
#define DEVICE_COUNT 2

struct driver_002_dev {
    int device_id;
    int temperature;

    struct cdev cdev;
    struct device *device;

    struct timer_list timer;
};

static dev_t dev_number;
static struct cdev driver_002_cdev;
static struct class *driver_002_class;
static struct driver_002_dev driver_002_devices[DEVICE_COUNT];


static int driver_002_open(struct inode *inode, struct file *file){
    int minor = iminor(inode);
    if (minor >= DEVICE_COUNT){
        return -ENODEV; // prevents unknown driver information extract
    }

    file->private_data = &driver_002_devices[minor];

    pr_info("driver_002: device %d opened\n", minor);
    return 0;
}

static int driver_002_release(struct inode *inode, struct file *file){
    struct driver_002_dev *dev = file->private_data;
    
    pr_info("driver_002: device %d closed\n", dev->device_id);
    return 0;
}

static ssize_t driver_002_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    struct driver_002_dev *dev = file->private_data;
    char message[32];
    int msg_len;

    msg_len = snprintf(message, sizeof(message), "Temp: %d\n", dev->temperature);
    return simple_read_from_buffer(buf, len, offset, message, msg_len);

}

static struct file_operations driver_002_fops = {
    .owner   = THIS_MODULE,
    .open    = driver_002_open,
    .release = driver_002_release,
    .read = driver_002_read,
};

static void driver_002_timer_callback(struct timer_list *t)
{
    struct driver_002_dev *dev;
    dev = from_timer(dev, t, timer);

    dev->temperature++;
    pr_info("driver_002_%d: Temp updated to %d\n", dev->device_id, dev->temperature);
    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(2000));
}

static int __init driver_002_init(void){
    int ret, i;

    pr_info("driver_002: Initializing driver\n");

    ret = alloc_chrdev_region(&dev_number, 0, DEVICE_COUNT, DEVICE_NAME);
    if (ret < 0){
        pr_err("Failed to allocate device numbers\n");
        return ret;
    }

    cdev_init(&driver_002_cdev, &driver_002_fops);
    driver_002_cdev.owner = THIS_MODULE;

    ret = cdev_add(&driver_002_cdev, dev_number, DEVICE_COUNT);
    if (ret < 0){
        pr_err("Failed to add cdev\n");
        goto unregister_region;
    }

    driver_002_class = class_create(CLASS_NAME);
    if (IS_ERR(driver_002_class)){
        ret = PTR_ERR(driver_002_class);
        goto del_cdev;
    }

    for (i = 0; i < DEVICE_COUNT; i++){
        driver_002_devices[i].device = device_create(
            driver_002_class,
            NULL,
            MKDEV(MAJOR(dev_number), MINOR(dev_number) + i),
            NULL,
            "driver_002_%d",
            i
        );

        driver_002_devices[i].device_id = i;
        driver_002_devices[i].temperature = 25 + i;

        timer_setup(&driver_002_devices[i].timer, driver_002_timer_callback, 0); // initiailze the timer
        mod_timer(&driver_002_devices[i].timer, jiffies + msecs_to_jiffies(2000)); // start the timer

        if (IS_ERR(driver_002_devices[i].device)){
            ret = PTR_ERR(driver_002_devices[i].device);
            goto destroy_devices;
        }
    }

    pr_info("driver_002: Driver loaded successfully\n");
    return 0;

destroy_devices:
    while (i--)
        device_destroy(driver_002_class,
            MKDEV(MAJOR(dev_number), MINOR(dev_number) + i));

    class_destroy(driver_002_class);

del_cdev:
    cdev_del(&driver_002_cdev);

unregister_region:
    unregister_chrdev_region(dev_number, DEVICE_COUNT);
    return ret;
}

static void __exit driver_002_exit(void){
    int i;

    for (i = 0; i < DEVICE_COUNT; i++){
        del_timer_sync(&driver_002_devices[i].timer); // deletes the timers

        device_destroy(driver_002_class, MKDEV(MAJOR(dev_number), MINOR(dev_number) + i));
    }

    class_destroy(driver_002_class);
    cdev_del(&driver_002_cdev);
    unregister_chrdev_region(dev_number, DEVICE_COUNT);

    pr_info("driver_002: Driver unloaded\n");
}

module_init(driver_002_init);
module_exit(driver_002_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucifer");
MODULE_DESCRIPTION("Virtual Temperature Driver - Stage 01");
MODULE_VERSION("1.0");