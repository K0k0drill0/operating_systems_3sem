#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // мьютекс для синхронизации доступа к переменным
pthread_cond_t woman_cond = PTHREAD_COND_INITIALIZER; // условная переменная для ожидания женщин
pthread_cond_t man_cond = PTHREAD_COND_INITIALIZER; // условная переменная для ожидания мужчин

int num_women = 0; // количество женщин в ванной комнате
int num_men = 0; // количество мужчин в ванной комнате
int state = 0; // состояние ванной комнаты: 0 - никого нет, 1 - только женщины, 2 - только мужчины

void woman_wants_to_enter() {
    pthread_mutex_lock(&mutex);
    while (state == 2 || num_men > 0) { // если в ванной только мужчины или есть мужчины, женщина ждет
        pthread_cond_wait(&woman_cond, &mutex);
    }
    num_women++;
    if (num_women == 1) { // если женщина первая вошла, меняем состояние ванной комнаты
        state = 1;
    }
    printf("Woman entered the bathroom. Women: %d, men: %d\n", num_women, num_men);
    pthread_mutex_unlock(&mutex);
}

void man_wants_to_enter() {
    pthread_mutex_lock(&mutex);
    while (state == 1 || num_women > 0) { // если в ванной только женщины или есть женщины, мужчина ждет
        pthread_cond_wait(&man_cond, &mutex);
    }
    num_men++;
    if (num_men == 1) { // если мужчина первый вошел, меняем состояние ванной комнаты
        state = 2;
    }
    printf("Man entered the bathroom. Women: %d, men: %d\n", num_women, num_men);
    pthread_mutex_unlock(&mutex);
}

void woman_leaves() {
    pthread_mutex_lock(&mutex);
    num_women--;
    if (num_women == 0) { // если женщина последняя вышла, меняем состояние ванной комнаты
        state = 0;
        pthread_cond_broadcast(&man_cond); // оповещаем мужчин, что они могут зайти
    }
    printf("Woman left the bathroom. Women: %d, men: %d\n", num_women, num_men);
    pthread_mutex_unlock(&mutex);
}

void man_leaves() {
    pthread_mutex_lock(&mutex);
    num_men--;
    if (num_men == 0) { // если мужчина последний вышел, меняем состояние ванной комнаты
        state = 0;
        pthread_cond_broadcast(&woman_cond); // оповещаем женщин, что они могут зайти
    }
    printf("Man left the bathroom. Women: %d, men: %d\n", num_women, num_men);
    pthread_mutex_unlock(&mutex);
}

void *thread_func(void *arg) {
    int gender = *(int *)arg;
    if (gender == 0) { // женщина
        woman_wants_to_enter();
        sleep(rand() % 5); // имитация пребывания в ванной комнате
        woman_leaves();
    } else { // мужчина
        man_wants_to_enter();
        sleep(rand() % 5); // имитация пребывания в ванной комнате
        man_leaves();
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]); // количество потоков
    pthread_t threads[n];
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        int *gender = malloc(sizeof(int));
        *gender = rand() % 2; // случайно выбираем пол
        pthread_create(&threads[i], NULL, thread_func, gender);
        sleep(rand() % 2);
    }
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}