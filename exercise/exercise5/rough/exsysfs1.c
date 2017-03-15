#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>

struct my_attr {
    struct attribute attr;
    int value;
};

static struct my_attr my_first = {
    .attr = {
		.name = "first",
    		.mode = 0666,
    },		
    .value = 1,
};

static struct my_attr my_second = {
    .attr = { 
		.name = "second",
    		.mode = 0666,
    },
    .value = 2,
};

static struct attribute * myattr[] = {
    &my_first.attr,
    &my_second.attr,
    NULL
};

static ssize_t default_show(struct kobject *kobj, struct attribute *attr,
        char *buf)
{
    struct my_attr *a = container_of(attr, struct my_attr, attr);
    return scnprintf(buf, PAGE_SIZE, "%d\n", a->value);
}

static ssize_t default_store(struct kobject *kobj, struct attribute *attr,
        const char *buf, size_t len)
{
    struct my_attr *a = container_of(attr, struct my_attr, attr);
    sscanf(buf, "%d", &a->value);
    return sizeof(int);
}

static struct sysfs_ops myops = {
    .show = default_show,
    .store = default_store,
};

static struct kobj_type mytype = {
    .sysfs_ops = &myops,
    .default_attrs = myattr,
};

struct kobject *mykobj;
static int __init sysfsexample_module_init(void)
{
    int err = -1;
    mykobj = kzalloc(sizeof(*mykobj), GFP_KERNEL);
    if (mykobj) {
        kobject_init(mykobj, &mytype);
        if (kobject_add(mykobj, NULL, "%s", "sysfs_sample")) {
             err = -1;
             printk("Sysfs creation failed\n");
             kobject_put(mykobj);
             mykobj = NULL;
        }
        err = 0;
    }
    return err;
}

static void __exit sysfsexample_module_exit(void)
{
    if (mykobj) {
        kobject_put(mykobj);
        kfree(mykobj);
    }
}

module_init(sysfsexample_module_init);
module_exit(sysfsexample_module_exit);
MODULE_LICENSE("GPL");
