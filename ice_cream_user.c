#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 


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

//function prototypes
void *customer(void *id);
void *startManufacture(void *);
void *manufacture(void *id);

sem_t waitingLine;	//for line. waiting.
sem_t orderCounter;	//for getting order. means that this order is processing. 

//semaphores for manufactoring process.
sem_t boiler, sugar, flavor, cone, freezing, wrapping;	

sem_t custWait;	//for customer to not exit till order processed.

int allDone = 0;	//not all order have been processed.
int currentOrderRunning;	//global for getting knowledge of current order processing.


//global data so that can be accessed from diff functions.
int Numbers[MAX_ORDERS];
int numOfOrders;
int orderDetails[MAX_ORDERS];

int main()
{
	// pthread_t manufactureThread;
	pthread_t tid[MAX_ORDERS];

	printf("Enter the number of orders: \n");
	scanf("%d", &numOfOrders);
	if (numOfOrders > MAX_ORDERS)
	{
		printf("The number of orders in greater than the MAX capacity. Exiting...\n");
		return 0;
	}

	int i;
	printf("Enter the data of each order: \n");
	for (i = 0; i < numOfOrders; ++i)
	{
		scanf("%d", &orderDetails[i]);
	}

	printf("---DATA\n");
	printf("numOfOrders: %d\n", numOfOrders);
	for (i = 0; i < numOfOrders; i++) printf("%d \n", orderDetails[i]);
	printf("\n");

	printf("\t\t---Solution for Ice cream factory---\n");

	//Numbering each order.
	for (i = 0; i < MAX_ORDERS; ++i)
	{
		Numbers[i] = i + 1;
	}

	//Initializing each semaphore
	sem_init(&waitingLine, 0, numOfOrders);	// waiting line has initial value = numoforders.
	sem_init(&orderCounter, 0, 1);	//1 ordering machine.

	sem_init(&boiler, 0, 3);	//number of boilers = 3.
	sem_init(&sugar, 0, 2);	//number of sugar = 2.
	sem_init(&flavor, 0, 2);	//2 flavour machines.
	sem_init(&cone, 0, 2);	//number of cone = 2.
	sem_init(&freezing, 0, 3);	//number of freezing = 3.
	sem_init(&wrapping, 0, 2);	//number of wrapping = 2.

	sem_init(&custWait, 0, 0);

	//Creating threads for orders.
	for (i = 0; i < numOfOrders; ++i)
	{
		pthread_create(&tid[i], NULL, customer, (void *)&Numbers[i]);
	}

	//Joining each of order/customer thread.
	for (i = 0; i < numOfOrders; ++i)
	{
		pthread_join(tid[i], NULL);
	}


	allDone = 1;
	// pthread_join(manufactureThread, NULL);

	return 0;
}

void *customer(void *id)
{
	int num = *(int *)id;
	printf("The order number %d has been received.\n", num);
	sleep(1);
	sem_wait(&waitingLine);	
	printf("The order number %d is in waiting area now.\n", num);


	sem_wait(&orderCounter);
	sem_post(&waitingLine);
	printf("The order number %d has entered the counter\n", num);
	currentOrderRunning = num;

	printf("---Debug--- currentOrderRunning: %d\n", currentOrderRunning);
	startManufacture((void *)0);

	// sem_wait(&custWait);

	printf("The order #%d has been processed. Order leaving.\n", num);
	sem_post(&orderCounter);

}

void *startManufacture(void *nothing)
{
	
	pthread_t childThreads[orderDetails[currentOrderRunning - 1]];

	int tempNumbers[orderDetails[currentOrderRunning - 1]];

	int i;
	for (i = 0; i < orderDetails[currentOrderRunning - 1]; ++i)
	{
		tempNumbers[i] = i+1;
	}

	for (i = 0; i < orderDetails[currentOrderRunning - 1]; ++i)
	{
		pthread_create(&childThreads[i], NULL, manufacture, (void *)&tempNumbers[i]);
	}

	for (i = 0; i < orderDetails[currentOrderRunning - 1]; ++i)
	{
		pthread_join(childThreads[i], NULL);
	}

	printf("The order # %d is finalized and exiting...\n", currentOrderRunning);

	// sem_post(custWait);

}

void *manufacture(void *id)
{
	int num = *(int *)id;

	sem_wait(&boiler);
	printf("The order number # %d's ice cream # %d is in boiling stage.\n", currentOrderRunning, num);
	sleep(2);
	sem_post(&boiler);

	sem_wait(&sugar);
	printf("The order number # %d's ice cream # %d is in sugar stage.\n", currentOrderRunning, num);
	sleep(1);
	sem_post(&sugar);

	sem_wait(&flavor);
	printf("The order number # %d's ice cream # %d is in flavor stage.\n", currentOrderRunning, num);
	sleep(1);
	sem_post(&flavor);

	sem_wait(&cone);
	printf("The order number # %d's ice cream # %d is in coning stage.\n", currentOrderRunning, num);
	sleep(1);
	sem_post(&cone);

	sem_wait(&freezing);
	printf("The order number # %d's ice cream # %d is in freezing stage.\n", currentOrderRunning, num);
	sleep(2);
	sem_post(&freezing);

	sem_wait(&wrapping);
	printf("The order number # %d's ice cream # %d is in wrapping stage.\n", currentOrderRunning, num);
	sleep(1);
	sem_post(&wrapping);

	printf("The order number # %d's ice cream # %d is finished/made.\n", currentOrderRunning, num);

}

