#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static inline void cr0_status(void) {

	long cr0;

	__asm__(
			"mov %%cr0, %0;"
			:"=r"(cr0)
			: /* no input registers */
			: /* no explicit clobbered regs */
	       );

	printk(KERN_DEBUG "cr0: %02lX\n", cr0);
}



static int cachecontrol_init(void) {

	cr0_status();

	printk(KERN_NOTICE "Disabling caches...\n");
	__asm__(
			"push %rax\n\t"
			"mov %cr0,%rax\n\t"
			"or $(1<<30),%rax\n\t"
			"mov %rax,%cr0\n\t"
			"wbinvd\n\t"
			"pop %rax\n\t");
			

	printk(KERN_NOTICE "Disabling caches...DONE\n");
	
	cr0_status();
	return 0;
}
static void cachecontrol_exit(void) {
	
	cr0_status();
	
	printk(KERN_NOTICE "Enabling caches...\n");
	
	__asm__(
			"push %rax\n\t"
			"mov %cr0,%rax\n\t"
			"and $(~(1<<30)),%rax\n\t"
			"mov %rax,%cr0\n\t"
			"pop %rax\n\t");

	printk(KERN_NOTICE "Enabling caches...DONE\n");
	cr0_status();		
}

module_init(cachecontrol_init);
module_exit(cachecontrol_exit);

