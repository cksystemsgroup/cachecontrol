#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

cpu_set_t startup_set;

static void where_am_i(void) {

	int i;
	cpu_set_t cs;
	
	if (sched_getaffinity(0, sizeof(cpu_set_t), &cs)) {
		perror("sched_getaffinity");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < CPU_SETSIZE; ++i) {
		if (CPU_ISSET(i, &cs)) {
			printf("I can run on core %d\n", i);
		}
	}
}

void select_cpu(int cpu_id) {
	cpu_set_t cs;

	CPU_ZERO(&cs);
	CPU_SET(cpu_id, &cs);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &cs)) {
		perror("sched_setaffinity");
		exit(EXIT_FAILURE);
	}
}

void restore_startup_set(void) {
	if (sched_setaffinity(0, sizeof(cpu_set_t), &startup_set)) {
		perror("sched_setaffinity");
		exit(EXIT_FAILURE);
	}
}

void disable_cache(int cpu_id) {

	select_cpu(cpu_id);
	
	const char *filename = "/proc/cachecontrol-disable";
	FILE *fp = fopen(filename, "r");
	
	if (fp == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	char *buffer = NULL;
	size_t len = 0;
	size_t bytes_read;
        if ((bytes_read = getline(&buffer, &len, fp)) == -1) {
		perror("getline");
		exit(EXIT_FAILURE);
	}

	printf("Disabled caches at %d: %s\n", cpu_id, buffer);

	free(buffer);
	fclose(fp);

	restore_startup_set();
}
void enable_cache(int cpu_id) {

	select_cpu(cpu_id);
	
	const char *filename = "/proc/cachecontrol-enable";
	FILE *fp = fopen(filename, "r");
	
	if (fp == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	char *buffer = NULL;
	size_t len = 0;
	size_t bytes_read;
        if ((bytes_read = getline(&buffer, &len, fp)) == -1) {
		perror("getline");
		exit(EXIT_FAILURE);
	}

	printf("Enabled caches at %d: %s\n", cpu_id, buffer);

	free(buffer);
	fclose(fp);
	
	restore_startup_set();
}

/**
 * returns 0 if caches are enabled
 * returns 1 if caches are disabled
 */
int caches_disabled(void) {

	const char *filename = "/proc/cachecontrol-cr0";
	FILE *cr0 = fopen(filename, "r");
	
	if (cr0 == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	char *buffer = NULL;
	size_t len = 0;
	size_t bytes_read;
        if ((bytes_read = getline(&buffer, &len, cr0)) == -1) {
		perror("getline");
		exit(EXIT_FAILURE);
	}

	long cl = atol(buffer);
	//printf("%ld\n", cl);

	free(buffer);
	fclose(cr0);

	return (cl & (1<<30));
}

void print_cache_status(void) {
	
	if (caches_disabled()) {
		printf("caches disabled\n");
	} else {
		printf("caches enabled\n");
	}
}


void print_usage(void) {

	printf("usage:	cpucache --enable|--disable cpuid\n");
	printf("	cpucache --list\n");
	exit(EXIT_FAILURE);
}

int get_arg_cpu_id(int argc, char **argv) {

	if (argc < 3) {
		print_usage();
		return -1;
	} else {
		return atoi(argv[2]);
	}
}

void list_cache_status(void) {
	int i;
	for (i = 0; i < CPU_SETSIZE; ++i) {
		if (CPU_ISSET(i, &startup_set)) {
			select_cpu(i);
			printf("%d: ", i);
			print_cache_status();
		}
	}
	restore_startup_set();
}

int main(int argc, char **argv) {

	int cpu_id;

	if (argc < 2) {
		print_usage();
	}

	//store original cpu map
	if (sched_getaffinity(0, sizeof(cpu_set_t), &startup_set)) {
		perror("sched_getaffinity");
		exit(EXIT_FAILURE);
	}


	if (strncmp("--enable", argv[1], 10) == 0) {
		cpu_id = get_arg_cpu_id(argc, argv);
		enable_cache(cpu_id);	

	} else if (strncmp("--disable", argv[1], 10) == 0) {
		cpu_id = get_arg_cpu_id(argc, argv);	
		disable_cache(cpu_id);	

	} else if (strncmp("--list", argv[1], 10) == 0) {
		list_cache_status();
	} else {
		print_usage();
	}

	return EXIT_SUCCESS;
}

