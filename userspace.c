#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(){
	long int i = syscall(452); // 335 (prev ice_cream_facotry), 452 (new ice_cream_facotry), 451 (hello)
	printf("System call sys_ice_cream_factory returned %ld\n",i);
	return 0;
}
