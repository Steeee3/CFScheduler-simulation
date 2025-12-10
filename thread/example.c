#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *fooLong(void *arg) {
    sleep(2);
    printf("Terminata funzione lunga\n");
    return NULL;
}

void *fooShort(void *arg) {
    printf("Terminata funzione corta\n");
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(
        &t1,
        NULL,
        fooLong,
        NULL
    );
    pthread_create(
        &t2,
        NULL,
        fooShort,
        NULL
    );

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}