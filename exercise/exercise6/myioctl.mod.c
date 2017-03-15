#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x578e40e7, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x5b9a44fd, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x69040e3e, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xad527c47, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x63e8acef, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xa1d9633e, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xc87bf279, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x60898bcc, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x1c4dae57, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0xc620dd1, __VMLINUX_SYMBOL_STR(mutex_lock_interruptible) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "68A896F08E65BFA3E810802");
