#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h> 
#define MAX_LINE_LENGTH 10000
#define MAX_NUM_ACCOUNTS 1000
#define MAX_NUM_CLIENTS 1000
#define MAX_NUM_TRANSACTIONS 1000

// define struct to hold information about a transaction
struct Transaction {
	char type[10];
	char account_name[5];
	int amount;
};

// define struct to hold information about an account
struct Account {
	char account_name[5];
	int balance;
};

// define struct to hold information about a client
struct Client {
	char client_name[5];
	struct Transaction transactions[MAX_NUM_TRANSACTIONS];
};

pthread_mutex_t lock;

struct Account accounts[MAX_NUM_ACCOUNTS];
struct Client clients[MAX_NUM_CLIENTS];
int num_accounts = 0;
int num_clients = 0;
int num_transactions[MAX_NUM_CLIENTS];

// function that gets the index of the account in the accounts array
int get_account_index(char account_name[]) {
	// if the account is in the accounts array, return the index. Otherwise, return -1
	for (int i = 0; i < num_accounts; i++) {
		if (strcmp(accounts[i].account_name, account_name) == 0) {
			return i;
		}
	}
	return -1;
}

// function executed by the threads, processes the transactions for a client
void *process_transaction(void *arg) {
	int client = *((int*) arg);

	// do all the transactions of the client
	for (int i = 0; i < num_transactions[client]; i++) {
		// get the transaction information
		struct Transaction transaction = clients[client].transactions[i];

		// get the index of the account for the account the transaction is associated with
		int account_index = get_account_index(transaction.account_name);

		pthread_mutex_lock(&lock);  // ENTRY

		/*****CRITICAL SECTION STARTS ******/
		// if the transaction type is deposit, deposits the money into the account
		if ((account_index >= 0) && (strcmp(transaction.type, "deposit") == 0)) {
			accounts[account_index].balance = accounts[account_index].balance + transaction.amount;
		}

		// if the transaction type is withdraw, withdraws the money if there is enough money in the account
		if ((account_index >= 0) && (strcmp(transaction.type, "withdraw") == 0) && (accounts[account_index].balance >= transaction.amount)) {
			accounts[account_index].balance = accounts[account_index].balance - transaction.amount;
		}

		/***** CRITICAL SECTION ENDS ******/
		pthread_mutex_unlock(&lock); // EXIT
	}
	return 0;
}

int main() {
	// define file pointer
	FILE *fp;

	// open file
	fp = fopen("input.txt", "r");

	// opening file unsuccessful
	if (fp == NULL) {
		printf("Error opening file");
		exit(1);
	}

	char line[MAX_LINE_LENGTH];

	// parse the file
	while (fgets(line, sizeof(line), fp) != NULL) {		
		char *ptr = strtok(line, " ");

		// if the line of the file contains information about an account, store the information in the array of struct Account
		if (line[0] == 'A') {
			strcpy(accounts[num_accounts].account_name, ptr);
			ptr = strtok(NULL, " ");
			ptr = strtok(NULL, " ");
			accounts[num_accounts].balance = atoi(ptr);
			ptr = strtok(NULL, " ");

			num_accounts++;
		}

		// if the line of the file contains information about a client, store the information in the array of struct Client
		if (line[0] == 'C') {
			strcpy(clients[num_clients].client_name, ptr);
			ptr = strtok(NULL, " ");

			int transactions = 0;

			// store the information for each transaction of a client
			while (ptr != NULL) {
				strcpy(clients[num_clients].transactions[transactions].type, ptr);
				ptr = strtok(NULL, " ");
				strcpy(clients[num_clients].transactions[transactions].account_name, ptr);
				ptr = strtok(NULL, " ");
				clients[num_clients].transactions[transactions].amount = atoi(ptr);
				ptr = strtok(NULL, " ");

				transactions++;
			}

			num_transactions[num_clients] = transactions;
			num_clients++;
		}
		
	}
	fclose(fp);

	// print the number of accounts and the number of clients
	printf("No. of Accounts: %d\n", num_accounts);
	printf("No. of Clients: %d\n", num_clients);

	pthread_t threads[num_clients];
	int thread_args[num_clients];

	// create threads for the clients
	for (int i = 0; i < num_clients; i++) {
		thread_args[i] = i;
    	int err_thread = pthread_create(&threads[i], NULL, process_transaction, &thread_args[i]); 

		if (err_thread != 0)
			printf("\n Error creating thread %d", i);
	}

	for (int i = 0; i < num_clients; i++) {
		pthread_join(threads[i], NULL);
	}

	// print the balances of all the bank accounts
	for (int i = 0; i < num_accounts; i++) {
		printf("%s balance %d\n", accounts[i].account_name, accounts[i].balance);
	}

	pthread_mutex_destroy(&lock); 
	return 0;
}
