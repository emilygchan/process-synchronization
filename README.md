# process-synchronization
A mutual exclusion algorithm that guarantees only one process access to the critical section portion of the code at a given time. The algorithm is used for a process synchronization problem: multiple bank accounts shared by multiple clients. The clients can perform two different types of transactions with each bank account: deposit and withdraw funds.

input.txt contains a test case. The file includes the initial balances of the bank accounts and the transactions each client performs.

Using input.txt, performs each transaction for each client, while maintaining process synchronization. Outputs the number of accounts, number of clients, and the balances of each bank account after all the transactions have been performed.

## Run project
* Download process_synchronization.c and input.txt and put in the same folder
* Navigate to the folder
    * cd /path/to/files
* Compile the C file
    * gcc -o process_synchronization process_synchronization.c
* Run the executable
    * ./process_synchronization
