#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/kfifo.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_AUTHOR("Khalid Waseem");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel Programming Exercise 5");

#define FIFO_SIZE       1024
#define PROC_READ       "myread"
#define PROC_WRITE      "mywrite"
#define SYSFS_ROOT	"mysysfs"
#define NO_READ_OP	"noread"
#define NO_WRITE_OP	"nowrite"
#define BYTE_READ	"byteread"
#define BYTE_WRITE	"bytewrite"
#define READ_SIZE	"readsize"
#define DEFAULT_RSIZE 	8

static DEFINE_MUTEX(read_lock);
static DEFINE_MUTEX(write_lock);
static DECLARE_KFIFO(fifo_queue, unsigned char, FIFO_SIZE);
struct kobject *mykobj;

int noread = 0;
int nowrite = 0;
int bread = 0;
int bwrite = 0;
int rsize = DEFAULT_RSIZE;

struct my_attr {
    struct attribute attr;
    int value;
};

static struct my_attr mynoread = {
    .attr = {
                .name = NO_READ_OP,
                .mode = 0444,
    },         
    .value = 0,
};

static struct my_attr mynowrite = {
    .attr = {
                .name = NO_WRITE_OP,
                .mode = 0444,
    },
    .value = 0,
};

static struct my_attr mybread = {
    .attr = {
                .name = BYTE_READ,
                .mode = 0444,
    },         
    .value = 0,
};

static struct my_attr mybwrite = {
    .attr = {
                .name = BYTE_WRITE,
                .mode = 0444,
    },
    .value = 0,
};

static struct my_attr myrsize = {
    .attr = {
                .name = READ_SIZE,
                .mode = 0666,
    },
    .value = DEFAULT_RSIZE,
};

static struct attribute * myattr[] = {
    &mynoread.attr,
    &mynowrite.attr,
    &mybread.attr,
    &mybwrite.attr,
    &myrsize.attr,		
    NULL
};

static ssize_t default_show(struct kobject *kobj, struct attribute *attr, char *buf){
    struct my_attr *a = container_of(attr, struct my_attr, attr);
    if(strcmp(a->attr.name,NO_READ_OP)==0){
    	a->value = noread;
    }else if(strcmp(a->attr.name,NO_WRITE_OP)==0){
    	a->value = nowrite; 
    }else if(strcmp(a->attr.name,BYTE_READ)==0){
        a->value = bread;
    }else if(strcmp(a->attr.name,BYTE_WRITE)==0){
        a->value = bwrite;
    }else if(strcmp(a->attr.name,READ_SIZE)==0){
        a->value = rsize;
    }   			
    return scnprintf(buf, PAGE_SIZE, "%d\n", a->value);
}

static ssize_t default_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len){
    struct my_attr *a = container_of(attr, struct my_attr, attr);
    sscanf(buf, "%d", &a->value);
    rsize = a->value;	
    return sizeof(int);
}

static struct sysfs_ops mysysfs_ops = {
    .show = default_show,
    .store = default_store,
};

static struct kobj_type mykobjtype = {
    .sysfs_ops = &mysysfs_ops,
    .default_attrs = myattr,
};

static ssize_t fifo_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos){
        int ret;
        unsigned int copied;

        if (mutex_lock_interruptible(&write_lock))
                return -ERESTARTSYS;
        ret = kfifo_from_user(&fifo_queue, buf, count, &copied);
        mutex_unlock(&write_lock);
	if(copied){
		nowrite++;
		bwrite += copied;
	}
	
        return ret ? ret : copied;
}

static ssize_t fifo_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
	int ret;
        unsigned int copied;

	rsize = rsize<count ? rsize : count;
        if (mutex_lock_interruptible(&read_lock))
                return -ERESTARTSYS;
        ret = kfifo_to_user(&fifo_queue, buf, rsize, &copied);
	mutex_unlock(&read_lock);

	noread++;
        bread += copied;

	buf[copied] = '\0';
        printk(KERN_INFO "Total reads op:%d Total bytes read:%d\n", noread, bread);
        printk(KERN_INFO "%.*s\n", copied, buf);

        return ret ? ret : copied;
}

static const struct file_operations fifo_rfops = {
        .owner          = THIS_MODULE,
        .read           = fifo_read,
        .write          = NULL,
        .llseek         = noop_llseek,
};

static const struct file_operations fifo_wfops = {
        .owner          = THIS_MODULE,
        .read           = NULL,
        .write          = fifo_write,
        .llseek         = noop_llseek,
};

static int create_sysfs(void){
	int err = -1;
    	mykobj = kzalloc(sizeof(*mykobj), GFP_KERNEL);
    	if (mykobj) {
        	kobject_init(mykobj, &mykobjtype);
        	if (kobject_add(mykobj, NULL, "%s", SYSFS_ROOT)) {
             		err = -1;
             		printk("Sysfs creation failed\n");
             		kobject_put(mykobj);
             		mykobj = NULL;
        	}
        	err = 0;
    	}
    	return err;
}

static int create_proc(void){
	if (proc_create(PROC_READ, 0444, NULL, &fifo_rfops) == NULL) {
                return -ENOMEM;
        }
	if (proc_create(PROC_WRITE, 0222, NULL, &fifo_wfops) == NULL) {
                return -ENOMEM;
        }
	return 0;
}

void cleanup_sysfs(void){
	if (mykobj) {
        	kobject_put(mykobj);
        	kfree(mykobj);
    	}
}

void cleanup_proc(void){
        remove_proc_entry(PROC_READ, NULL);
        remove_proc_entry(PROC_WRITE, NULL);
}

static int __init myvfs_start(void){
	printk(KERN_INFO "MyVFS Module loaded.\n");
	// Initialize fifo queue
	INIT_KFIFO(fifo_queue);
	if(create_proc()!=0){
		return -ENOMEM;
	}
	if(create_sysfs()!=0){
		return -1;
	}
	return 0;
}

static void __exit myvfs_end(void){
	cleanup_proc();
	cleanup_sysfs();
        printk(KERN_INFO "MyVFS Module removed.\n");
}

module_init(myvfs_start);
module_exit(myvfs_end);
