#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

int max;
pthread_mutex_t avail;
/* the available amount for each resource*/
int available[NUMBER_OF_RESOURCES];

/* the maximum demand of each customer*/
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer*/
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer*/
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

bool is_greater(int *x, int *y) {
	int i;
	for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
		if (a[i] <= b[i]) {
			return false;
		}
	}
	return true;
}

bool is_request_safe() {
	int *finish = (int*) malloc(NUMBER_OF_CUSTOMERS * sizeof(int));
	int *work = (int*) malloc(NUMBER_OF_RESOURCES * sizeof(int));
	int completed;
	int i, j;
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		finish[i] = 0;
	}
	for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
		work[i] = available[i];
	}
	
	/* Check where need is smaller than or equal to work and is not finished*/
	int c = 0;
	do {
		for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
			if (!is_greater(need[i], work) && finish[i]) {
				for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
					work[j] += allocation[i][j];
				}
				finish[i] = 1;
			} else {
				completed = 0;
				for (j = 0; j < NUMBER_OF_CUSTOMERS; j++) {
					completed += finish[j];
				}
				if (completed == NUMBER_OF_CUSTOMERS) {
					return true;
				} 
			}
		}
		c++;
	} while(c < NUMBER_OF_CUSTOMERS);

	free(work);
	free(finish);
	return false;
}

int request_resources(int customer_num, int request[]) {
	pthread_mutex_lock(&avail);
	printf("%lu, %d, ", time(NULL), customer_num);
	int i;
	for(i=0; i < NUMBER_OF_RESOURCES; i++) {
		printf("%d requested resource type %d " request[i], i);
	}
	
	/* Check if the resources are available */
	if (is_greater(request, need[customer_num]) || is_greater(request, available)) {
		printf("\n %lu, %d, request denied \n", time(NULL), customer_num);
		pthread_mutex_unlock(&avail);
		return -1;
	}
	
	/*If available check of it's safe*/
	for(i= 0; i < NUMBER_OF_RESOURCES; i++) {
		available[i] -= request[i];
		allocation[customer_num][i] += request[i];
		need[customer_num][i] -= request[i];
	}

	if (is_request_safe()) {
		printf("\n %lu, %d, request satisfied \n", time(NULL), customer_num);
		pthread_mutex_unlock(&avail);
		return 0;
	} else {
		for(i= 0; i < NUMBER_OF_RESOURCES; i++) {
			available[i] += request[i];
			allocation[customer_num][i] -= request[i];
			need[customer_num][i] += request[i];
		}
		printf("\n %lu, %d, request denied \n", time(NULL), customer_num);
		pthread_mutex_unlock(&avail);
		return -1;
	}
}

int release_resources(int customer_num) {
	pthread_mutex_lock(&avail);
	int i;
	for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
		available[i] += allocation[customer_num][i];
		allocation[customer_num][i] = 0;
	}

	printf("%lu, %d, resource released \n", time(NULL), customer_num);
	pthread_mutex_unlock(&avail);
	return 0;
}

bool needs(int customer_id) {
	int i;
	for (i= 0; i < NUMBER_OF_RESOURCES; i++) {
		if(need[customer_id][i] != 0) {
			return false;
		}
	}
	return true;
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

		//make requests
		int *request = (int*)malloc(sizeof(int)*NUMBER_OF_RESOURCES);
		int approve;
		while(needs(customer_id)) {
			for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
				request[i] = rand() % (need[customer_id][i]+1);
			}
			approve = -1;
			while(approve == -1) {
				approve = request_resources(customer_id, request);
				// If request is denied, go to sleep				
				if (approve == -1) {
					sleep(rand()%1001);
				} // request approved but still has need 
				else if (approve == 0 && needs(customer_id)) {
					sleep(rand()%1001);
				}
			}
		}

		free(request);
		sleep(rand()%1000);
		release_resources(customer_id);
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
	pthread_t *customer_threads = (pthread_t*)malloc(sizeof(pthread_t)*NUMBER_OF_CUSTOMERS);
	
	/*Threads of customers*/
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		int *id = (int*)malloc(sizeof(int));
		*id = i;
		pthread_create(&customer_threads[i], NULL, customer_entry, (void*)id);
	}
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		pthread_join(customer_threads[i], NULL);
	}

	return 0;
}
