#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define LEFT ((int)philosopher_number + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS
#define RIGHT ((int)philosopher_number + 1) % NUM_PHILOSOPHERS

int state[NUM_PHILOSOPHERS];
sem_t mutex;
sem_t semaphores[NUM_PHILOSOPHERS];

void test(int philosopher_number);

void think(int philosopher_number) {
    printf("Philosopher %d is thinking\n", philosopher_number);
    sleep(rand() % 3 + 1);
}

void eat(int philosopher_number) {
    printf("Philosopher %d is eating\n", philosopher_number);
    sleep(rand() % 3 + 1);
}

void* philosopher(void* philosopher_number) {
    int num = *(int*)philosopher_number;

    while (1) {
        think(num);

        sem_wait(&mutex);
        state[num] = HUNGRY;
        printf("Philosopher %d is hungry\n", num);
        test(num);
        sem_post(&mutex);

        sem_wait(&semaphores[num]);

        eat(num);

        sem_wait(&mutex);
        state[num] = THINKING;
        printf("Philosopher %d finished eating and is thinking\n", num);
        test(LEFT);
        test(RIGHT);
        sem_post(&mutex);
    }
}

void test(int philosopher_number) {
    if (state[philosopher_number] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {
        
        state[philosopher_number] = EATING;
        printf("Philosopher %d started eating\n", philosopher_number);
        sem_post(&semaphores[philosopher_number]);
    }
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int philosopher_numbers[NUM_PHILOSOPHERS];

    sem_init(&mutex, 0, 1);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&semaphores[i], 0, 0);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopher_numbers[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &philosopher_numbers[i]);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    sem_destroy(&mutex);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&semaphores[i]);
    }

    return 0;
}