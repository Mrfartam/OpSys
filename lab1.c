#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int volatile ready = 0;
int volatile k = 1;

void* consumer(void* arg){
    while(k <= 10)
    {
        pthread_mutex_lock(&lock);
        while(ready == 0){
            pthread_cond_wait(&cond1, &lock);
            printf("awoke\n");
        }
        ready = 0;
        printf("consumed №%d\n", k);
        k++;
        pthread_mutex_unlock(&lock);
    }
}

void* provider(void* arg){
    while(k <= 10){
        pthread_mutex_lock(&lock);
        if (ready == 1){
            pthread_mutex_unlock(&lock);
            continue;
        }
        ready = 1;
        printf("provided №%d\n", k);
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&lock);
        sleep(1);
    }
}

int main(){
    pthread_t prov, cons;
    
    pthread_create(&prov, NULL, provider, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prov, NULL);
    pthread_join(cons, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond1);

    return 1;
}
