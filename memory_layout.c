#include <stdio.h> // standard io
#include <stdlib.h> // C standard library
#include <pthread.h> // threading
#include <unistd.h> // unix standard library
#include <sys/types.h> // system types for linux

// getchar basically is like "read"
// it prompts the user for input
// in this case, the input is thrown away
// which makes similar to a "pause" continuation primitive 
// but a pause that is resolved through user input, which we promptly throw away!
void * thread_func (void * arg) {

    printf("Before malloc in thread 1\n");
    getchar();
    char * addr = (char *) malloc(1000);
    printf("After malloc and before free in thread 1\n");
    getchar();
    free(addr);
    printf("After free in thread 1\n");
    getchar();

}

int main () {

    char * addr;
    printf("Welcome to per thread arena example::%d\n", getpid());
    printf("Before malloc in the main thread\n");
    getchar();
    addr = (char *) malloc(1000);
    printf("After malloc and before free in main thread\n");
    getchar();
    free(addr);
    printf("After free in main thread\n");
    getchar();

    // pointer to the thread 1
    pthread_t thread_1;
    // pthread_* functions return 0 upon succeeding, and other numbers upon failing
    int pthread_status;

    pthread_status = pthread_create(&thread_1, NULL, thread_func, NULL);
    
    if (pthread_status != 0) {
        printf("Thread creation error\n");
        return -1;
    }

    // returned status code from thread_1
    void * thread_1_status;

    pthread_status = pthread_join(thread_1, &thread_1_status);
    
    if (pthread_status != 0) {
        printf("Thread join error\n");
        return -1;
    }

    return 0;

}
