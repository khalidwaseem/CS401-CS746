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
	{ 0xa8eb5c9b, __VMLINUX_SYMBOL_STR(noop_llseek) },
	{ 0xecfdc80b, __VMLINUX_SYMBOL_STR(kobject_add) },
	{ 0xd3842b70, __VMLINUX_SYMBOL_STR(kobject_init) },
	{ 0x3025bf7a, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x43124200, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x93b4e512, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x1bba06d4, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xf867c601, __VMLINUX_SYMBOL_STR(kobject_put) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x4578f528, __VMLINUX_SYMBOL_STR(__kfifo_to_user) },
	{ 0x4ca9669f, __VMLINUX_SYMBOL_STR(scnprintf) },
	{ 0x20c55ae0, __VMLINUX_SYMBOL_STR(sscanf) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x1c4dae57, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x30a80826, __VMLINUX_SYMBOL_STR(__kfifo_from_user) },
	{ 0xc620dd1, __VMLINUX_SYMBOL_STR(mutex_lock_interruptible) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C9A8BE4A361CBCF7EE64E09");
