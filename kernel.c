#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/time.h>
#include<linux/timer.h>
#include<linux/semaphore.h>
#include<linux/delay.h>

//just to check if compilation working correctly
//#include<stdio.h>

/**
 * DATA about procedures.
 * Boilers = 3. time: 2sec.
 * Sugar = 2. time: 1sec
 * Flavor = 2. time: 1sec
 * Cone = 2. time: 1sec
 * Freezing = 3. time: 2sec
 * Wrapping = 2. time: 1sec
 */

#define MAX_ORDERS 15

static struct semaphore waitingLine;	//for line. waiting.
static struct semaphore orderCounter;	//for getting order. means that this order is processing. 

//semaphores for manufactoring process.
static struct semaphore boiler, sugar, flavor, cone, freezing, wrapping;	

static struct semaphore custWait;	//for customer to not exit till order processed.

int allDone = 0;		//not all order have been processed.
int currentOrderRunning;		//global for getting knowledge of current order processing.

//global data so that can be accessed from diff functions.
int Numbers[MAX_ORDERS];
//int orderDetails[MAX_ORDERS];

#define numOfOrders 4   //scanf data 

int const orderDetails[numOfOrders] = {3,1,2,4}; //scanf data

static struct task_struct *childThreads[MAX_ORDERS];

static struct task_struct *tid[MAX_ORDERS];

// change the return type from void to int as per required signature by kthread_create.
int customer(void *id);
int startManufacture(void *);
int manufacture(void *id);

asmlinkage long sys_icecream(void) {
	int i;
	//scanf("%d", &numOfOrders);
	if (numOfOrders > MAX_ORDERS) {
		printk(KERN_INFO "The number of orders in greater than the MAX capacity. Exiting...\n");
		return 0;
	}

	//printk(KERN_INFO "Enter the data of each order: \n");
	/*for (i = 0; i < numOfOrders; ++i) {
		//scanf("%d", &orderDetails[i]);
	}*/

	printk(KERN_INFO "\t---DATA\n");
	printk(KERN_INFO "numOfOrders: %d\n", numOfOrders);
	for (i = 0; i < numOfOrders; i++) printk(KERN_INFO "%d \n", orderDetails[i]);
	printk(KERN_INFO "-----------\n");

	//Numbering each order.
	printk(KERN_INFO "\t\t---Solution for Ice cream factory---\n");
	for (i = 0; i < MAX_ORDERS; ++i){
		Numbers[i] = i + 1;
	}

	//Initializing each semaphore
	sema_init(&waitingLine,  numOfOrders);	// waiting line has initial value = numoforders.
	sema_init(&orderCounter, 1);	//1 ordering machine.

	sema_init(&boiler, 3); //number of boilers = 3.
	sema_init(&sugar, 2); //number of sugar = 2.
	sema_init(&flavor, 2); //2 flavour machines.
	sema_init(&cone, 2); //number of cone = 2.
	sema_init(&freezing, 3); //number of freezing = 3.
	sema_init(&wrapping, 2); //number of wrapping = 2.

	sema_init(&custWait, 0);

	//Creating threads for orders.
	char our_thread[8]="thread1";
	for (i = 0; i < numOfOrders; ++i) {
		tid[i] = kthread_create(customer,(void*)&Numbers[i],our_thread);
		if ((tid[i])) {
			//printk(KERN_INFO "---Debug--- tid[] not NULL. calling wake_up_process");
			wake_up_process(tid[i]);
			//printk(KERN_INFO "---Debug--- wake_up_process called on tid[].\n");
		}
	}
	msleep(3000);

	//Joining each of order/customer thread.
	/*for (i = 0; i < numOfOrders; ++i) {
		if (tid[i]) {
			kthread_stop(tid[i]);
			printk(KERN_INFO "---Debug--- kthread_stop called on tid[].\n");
		}
	}*/

	allDone = 1;
	printk(KERN_INFO "Program ended\n");
	return 0;
}

int customer(void *id) {
	int num = *(int *)id;
	printk(KERN_INFO "The order number %d has been received.\n", num);
	msleep(10);
	down(&waitingLine);	
	printk(KERN_INFO "The order number %d is in waiting area now.\n", num);

	down(&orderCounter);
	up(&waitingLine);
	printk(KERN_INFO "The order number %d has entered the counter\n", num);
	currentOrderRunning = num;

	//printk(KERN_INFO "---Debug--- currentOrderRunning: %d\n", currentOrderRunning);
	startManufacture((void *)0);

	printk(KERN_INFO "The order #%d has been processed. Order leaving.\n", num);
	up(&orderCounter);
	
	do_exit(0);
	return 0;
}

int startManufacture(void *nothing) {
	// int size1 = orderDetails[currentOrderRunning - 1];
	// static struct task_struct *childThreads = (struct task_struct *)vmalloc(size1*sizeof(struct task_struct));

	int tempNumbers[orderDetails[currentOrderRunning - 1]];

	int i;
	for (i = 0; i < orderDetails[currentOrderRunning - 1]; ++i) {
		tempNumbers[i] = i+1;
	}

	// char our_thread[8]="thread1";
	for (i = 0; i < orderDetails[currentOrderRunning-1]; ++i) {
		childThreads[i] = kthread_create(manufacture,(void*)&tempNumbers[i],"thread");
		if((childThreads[i])){
			// printk(KERN_INFO "---Debug--- Inside childThreads[][]. wakeup calling.\n");
			wake_up_process(childThreads[i]);
			// printk(KERN_INFO "---Debug--- The wake_up_process was called on childThreads[][].\n");
		}
	}
	msleep(500);

	/*for (i = 0; i < orderDetails[currentOrderRunning - 1]; ++i) {
		if (childThreads[i]) {
			kthread_stop(childThreads[i]);
			printk(KERN_INFO "---Debug--- kthread_stop called on childThreads[][].\n");
		}
	}*/

	//printk(KERN_INFO "---Debug--- Near return 0 in startManufacture.\n");
	return 0;
}

int manufacture(void *id) {
	//printk(KERN_INFO "---Debug--- inside manufacture function.\n");
	int num = *(int *)id;

	down(&boiler);
	printk(KERN_INFO "The order number # %d's ice cream # %d is in boiling stage.\n", currentOrderRunning, num);
	msleep(15);
	up(&boiler);

	down(&sugar);
	printk(KERN_INFO "The order number # %d's ice cream # %d is in sugar stage.\n", currentOrderRunning, num);
	msleep(10);
	up(&sugar);

	down(&flavor);
	printk(KERN_INFO "The order number # %d's ice cream # %d is in flavor stage.\n", currentOrderRunning, num);
	msleep(10);
	up(&flavor);

	down(&cone);
	printk(KERN_INFO "The order number # %d's ice cream # %d is in coning stage.\n", currentOrderRunning, num);
	msleep(10);
	up(&cone);

	down(&freezing);
	printk(KERN_INFO "The order number # %d's ice cream # %d is in freezing stage.\n", currentOrderRunning, num);
	msleep(15);
	up(&freezing);

	down(&wrapping);
	printk(KERN_INFO "The order number # %d's ice cream # %d is in wrapping stage.\n", currentOrderRunning, num);
	msleep(10);
	up(&wrapping);

	printk(KERN_INFO "The order number # %d's ice cream # %d is finished/made.\n", currentOrderRunning, num);
	
	do_exit(0);
	return 0;
}
