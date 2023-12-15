#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(){
	long int i = syscall(452); // 451 (sys_hello), 452 (sys_ice_cream_factory)
	printf("System call sys_ice_cream_factory returned %ld\n", i);
	return 0;
}
