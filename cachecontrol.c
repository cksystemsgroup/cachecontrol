#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("Dual BSD/GPL");

//TODO: enable per-cpu execution

static long cr0_status(void) {

	volatile long cr0;
	__asm__(
			"mov %%cr0, %0;"
			:"=r"(cr0)
			: /* no input registers */
			: /* no explicit clobbered regs */
	       );
	return cr0;
}

static void cachecontrol_disable_caches(void) {

	__asm__(
			"push %rax\n\t"
			"mov %cr0,%rax\n\t"
			"or $(1<<30),%rax\n\t"
			"mov %rax,%cr0\n\t"
			"wbinvd\n\t"
			"pop %rax\n\t");
}

static void cachecontrol_enable_caches(void) {

	__asm__(
			"push %rax\n\t"
			"mov %cr0,%rax\n\t"
			"and $(~(1<<30)),%rax\n\t"
			"mov %rax,%cr0\n\t"
			"wbinvd\n\t"
			"pop %rax\n\t");
}

int cachecontrol_read_cr0(char *buf, char **start, off_t offset,
		int count, int *eof, void *data) {

	int r;
	//r = sprintf(buf, "%02lX\n", cr0_status());
	r = sprintf(buf, "%ld\n", cr0_status());
	*eof = 1;
	return r;
}

int cachecontrol_read_disable(char *buf, char **start, off_t offset,
		int count, int *eof, void *data) {

	int r;
	cachecontrol_disable_caches();
	r = sprintf(buf, "OK: %02lX\n", cr0_status());
	*eof = 1;
	return r;
}

int cachecontrol_read_enable(char *buf, char **start, off_t offset,
		int count, int *eof, void *data) {

	int r;
	cachecontrol_enable_caches();
	r = sprintf(buf, "OK: %02lX\n", cr0_status());
	*eof = 1;
	return r;
}

static int cachecontrol_init(void) {
	printk(KERN_NOTICE "Loading cachecontrol...\n");

	create_proc_read_entry("cachecontrol-cr0", 0, NULL, 
			cachecontrol_read_cr0, NULL);
	create_proc_read_entry("cachecontrol-disable", 0, NULL, 
			cachecontrol_read_disable, NULL);
	create_proc_read_entry("cachecontrol-enable", 0, NULL, 
			cachecontrol_read_enable, NULL);

	return 0;
}

static void cachecontrol_exit(void) {
	printk(KERN_NOTICE "Unloading cachecontrol...\n");
	
	remove_proc_entry("cachecontrol-cr0", NULL);
	remove_proc_entry("cachecontrol-disable", NULL);
	remove_proc_entry("cachecontrol-enable", NULL);
}

module_init(cachecontrol_init);
module_exit(cachecontrol_exit);

