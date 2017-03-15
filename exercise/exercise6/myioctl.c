#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include "myioctl.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1
#define DEVICE_NAME "myioctl"
#define DNAME "khalid"
//#define MAX_ARRAY_SIZE 10

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
int array[MAX_ARRAY_SIZE];
static DEFINE_MUTEX(read_lock);
static DEFINE_MUTEX(write_lock);

static int my_open(struct inode *i, struct file *f){
    return 0;
}
static int my_close(struct inode *i, struct file *f){
    return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg){
    myioctl_arg_t myobj;
    array_arg array_obj;
    int i;

    switch (cmd){
        case MYIOCTL_GET_VALUE:
	    if (copy_from_user(&myobj, (myioctl_arg_t *)arg, sizeof(myioctl_arg_t))){
                return -EACCES;
            }
            
	    if (mutex_lock_interruptible(&read_lock))
                return -ERESTARTSYS;					
            myobj.value = array[myobj.index];
	    mutex_unlock(&read_lock);	
            
	    if (copy_to_user((myioctl_arg_t *)arg, &myobj, sizeof(myioctl_arg_t))){   
                return -EACCES;
            }
            break;
        case MYIOCTL_CLR:
	    if (mutex_lock_interruptible(&write_lock))
                return -ERESTARTSYS;
            for(i=0; i<MAX_ARRAY_SIZE; i++){
		array[i] = i;
	    }
	    mutex_unlock(&write_lock);		
	    break;
        case MYIOCTL_SET_VALUE:
            if (copy_from_user(&myobj, (myioctl_arg_t *)arg, sizeof(myioctl_arg_t))){
                return -EACCES;
            }
	    if (mutex_lock_interruptible(&write_lock))
                return -ERESTARTSYS;
            array[myobj.index] = myobj.value;
	    mutex_unlock(&write_lock);
            break;
	case MYIOCTL_PRINT:
	     /*printk(KERN_INFO "Current Array.\n");	
	     if (mutex_lock_interruptible(&read_lock))
                return -ERESTARTSYS;
	     for(i=0; i<MAX_ARRAY_SIZE; i++){
	        printk(KERN_INFO "%d ",array[i]);
	     }	
	     mutex_unlock(&read_lock);*/
	     if (mutex_lock_interruptible(&read_lock))
                return -ERESTARTSYS;
	     for(i=0; i<MAX_ARRAY_SIZE; i++){
             	array_obj.array[i] = array[i];
	     }
	     array_obj.size = MAX_ARRAY_SIZE;
             mutex_unlock(&read_lock);

             if (copy_to_user((array_arg *)arg, &array_obj, sizeof(array_arg))){
                 return -EACCES;
             }			
	     break;	
        default:
            return -EINVAL;
    }

    return 0;
}

static struct file_operations myioctl_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl
};

static void setup_array(void){
    int i;
    for(i=0; i<MAX_ARRAY_SIZE; i++){
	array[i] = i;
    }	
}

static int __init myioctl_init(void){
    int ret;
    struct device *dev_ret;

    setup_array();	

    // Dynamically allocate Major number
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, DNAME)) < 0){   
        return ret;
    }
    // Register char device	
    cdev_init(&c_dev, &myioctl_fops);
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0){
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char"))){
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, DEVICE_NAME))){
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
    printk(KERN_INFO "MyIOCTL Module added.\n");	

    return 0;
}

static void __exit myioctl_exit(void){
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    printk(KERN_INFO "MyIOCTL Module removed.\n");	
}

module_init(myioctl_init);
module_exit(myioctl_exit);

MODULE_AUTHOR("Khalid Waseem");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel Programming Exercise 6");
