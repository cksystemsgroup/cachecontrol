#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int cachecontrol_init(void) {
	printk(KERN_ALERT "Disable caches\n");

	__asm__(
			"push %rax\n\t"
			"mov %cr0,%rax\n\t"
			"or $(1<<30),%rax\n\t"
			"mov %rax,%cr0\n\t"
			"wbinvd\n\t"
			"pop %rax\n\t");
			

	return 0;
}
static void cachecontrol_exit(void) {
	printk(KERN_ALERT "Enable caches\n");

	__asm__(
			"push %rax\n\t"
			"mov %cr0,%rax\n\t"
			"and $~(1<<30),%rax\n\t"
			"mov %rax,%cr0\n\t"
			"pop %rax\n\t");
			
}

module_init(cachecontrol_init);
module_exit(cachecontrol_exit);

