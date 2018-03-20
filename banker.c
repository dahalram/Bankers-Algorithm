#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

pthread_mutex_t avail;
/* the available amount for each resource*/
int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer*/
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer*/
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer*/
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]) {
	
}

int release_resources(int customer_num, int request[]) {

}

void *customer_entry(void *customer_num) {
	int customer_id = (int*)customer_num;
	
	while (1) {
		int i;
		for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
			int tmp = rand() % (available[i] + 1);
			need[customer_id][i] = tmp;
			maximum[customer_id][i] = tmp;
		}

		/*TODO*/ 
		//make requests
	}
}

int main(int argc, int **argv) {

	/* Populate resources into available*/
	int i;
	for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
		available[i] = atoi(argv[i+1]);
	} 
	srand(time(NULL));
	
	pthread_mutex_init(&avail, NULL);
	pthread_t *cust = (pthread_t*)malloc(sizeof(pthread_t)*NUMBER_OF_CUSTOMERS);
	
	/*Threads of customers*/
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		int *id = (int*)malloc(sizeof(int));
		*id = i;
		pthread_create(&cust[i], NULL, customer_entry, (void*)id);
	}
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		pthread_join(cust[i], NULL);
	}

	return 0;
}
