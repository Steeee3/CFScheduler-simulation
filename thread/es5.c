#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h> 

pthread_mutex_t timeD, space;
sem_t semOpen, semClose;
int global = 0;

void *one(void *arg) {
    usleep(rand() % 5000);
    printf("ESEGUO THREAD %d\n", *(int *) arg);

    printf("Thread %d: MUTEX TIME\n", *(int*) arg);
    pthread_mutex_lock(&timeD);
    printf("Thread %d: SUCCESSO MUTEX TIME\n", *(int*) arg);

    sem_post(&semOpen);              // A

    printf("Thread %d: MUTEX SPACE\n", *(int*) arg);
    pthread_mutex_lock(&space);
    printf("Thread %d: SUCCESSO MUTEX SPACE\n", *(int*) arg);

    global = 1;
    printf("Variabile global: %d\n", global);

    printf("Thread %d: FINE MUTEX TIME\n", *(int *) arg);
    pthread_mutex_unlock(&timeD);
    printf("Thread %d: FINE MUTEX SPACE\n", *(int *) arg);
    pthread_mutex_unlock(&space);
    return (void*)1;
}

void *two(void *arg) {
    usleep(rand() % 5000);
    int semOpenValue, semCloseValue;

    printf("ESEGUO THREAD %d\n", *(int *) arg);

    printf("Thread %d: MUTEX TIME\n", *(int*) arg);
    pthread_mutex_lock(&timeD);
    printf("Thread %d: SUCCESSO MUTEX TIME\n", *(int*) arg);

    global = 2;
    printf("Variabile global: %d\n", global);

    sem_getvalue(&semOpen, &semOpenValue);
    printf("Thread %d: semOpen=%d\n", *(int*) arg, semOpenValue);
    sem_wait(&semOpen);              // B
    printf("Thread %d: fine wait\n", *(int*) arg);

    printf("Thread %d: FINE MUTEX TIME\n", *(int *) arg);
    pthread_mutex_unlock(&timeD);

    sem_getvalue(&semClose, &semCloseValue);
    printf("Thread %d: semClose=%d\n", *(int*) arg, semCloseValue);
    sem_wait(&semClose);
    printf("Thread %d: fine wait\n", *(int*) arg);
    return NULL;
}

void *three(void *arg) {
    usleep(rand() % 5000);
    int semCloseValue;

    printf("ESEGUO THREAD %d\n", *(int *) arg);

    printf("Thread %d: MUTEX SPACE\n", *(int*) arg);
    pthread_mutex_lock(&space);
    printf("Thread %d: SUCCESSO MUTEX SPACE\n", *(int*) arg);

    sem_getvalue(&semClose, &semCloseValue);
    printf("Thread %d: semClose=%d\n", *(int*) arg, semCloseValue);
    sem_wait(&semClose);
    printf("Thread %d: fine wait\n", *(int*) arg);

    global = 3;
    printf("Variabile global: %d\n", global);

    pthread_mutex_unlock(&space);
    printf("Thread %d: FINE MUTEX SPACE\n", *(int *) arg);

    sem_post(&semClose);             // C
    return NULL;
}

int main(void) {
    pthread_t th_1, th_2, th_3;
    int id1 = 1, id2 = 2, id3 = 3;

    srand(time(NULL));
    sem_init(&semOpen, 0, 0);
    sem_init(&semClose, 0, 1);
    pthread_create(&th_2, NULL, two, &id2);
    pthread_create(&th_1, NULL, one, &id1);
    pthread_create(&th_3, NULL, three, &id3);
    pthread_join(th_3, NULL);
    printf("MAIN: th_3 è terminato!\n");
    pthread_join(th_1, (void**)&global);   // D
    printf("MAIN: th_1 è terminato!\n");
    pthread_join(th_2, NULL);
    printf("MAIN: th_2 è terminato!\n");
}
