#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

pthread_mutex_t GATE = PTHREAD_MUTEX_INITIALIZER;
sem_t GO;
int global = 0;

void * FIRST (void * arg) {
    printf("ESEGUO FIRST\n");

    int goValue;
    sem_getvalue(&GO, &goValue);
    printf("Thread %d: GO=%d\n", *(int*) arg, goValue);
    sem_wait(&GO);                  /* istruzione A */
    printf("Thread %d: fine wait\n", *(int*) arg);
    global = 1;
    printf("Thread %d: MUTEX GATE\n", *(int*) arg);
    pthread_mutex_lock(&GATE);
    printf("Thread %d: SUCCESSO MUTEX GATE\n", *(int*) arg);
    sem_post(&GO);
    pthread_mutex_unlock(&GATE);
    return NULL;

} /* end FIRST */


void * LAST (void * arg) {

    if (global == 0) {
        printf("ESEGUO LAST global==%d\n", global);
        global = 2;

        printf("Thread %d: MUTEX GATE\n", *(int*) arg);
        pthread_mutex_lock(&GATE);  /* istruzione B */
        printf("Thread %d: SUCCESSO MUTEX GATE\n", *(int*) arg);
        int goValue;
        sem_getvalue(&GO, &goValue);
        printf("Thread %d: GO=%d\n", *(int*) arg, goValue);
        sem_wait(&GO);
        printf("Thread %d: fine wait\n", *(int*) arg);
        pthread_mutex_unlock(&GATE);
        sem_post(&GO);

    } else {
        printf("ESEGUO LAST global==%d\n", global);
        global = 3;

        int goValue;
        sem_getvalue(&GO, &goValue);
        printf("Thread %d: GO=%d\n", *(int*) arg, goValue);
        sem_wait(&GO);              /* istruzione C */
        printf("Thread %d: fine wait\n", *(int*) arg);
    } /* if */

    return NULL;

} /* end LAST */


void main() {

    pthread_t TH_1, TH_2, TH_3;
    int id1 = 1, id2 = 2, id3 = 3;

    sem_init(&GO, 0, 1);
    pthread_create(&TH_1, NULL, FIRST, &id1);
    pthread_create(&TH_2, NULL, LAST, &id2);
    pthread_join(TH_1, NULL);
    pthread_create(&TH_3, NULL, FIRST, &id3);

    pthread_join(TH_3, NULL);       /* istruzione D */
    pthread_join(TH_2, NULL);

} /* end main */
