#include <stdio.h>
#include <unistd.h>

int main() {
    fork();
    printf("Hi, id = %d, pid = %d\n", getpid(), getppid());
    fork();
    printf("Hi, id = %d, pid = %d\n", getpid(), getppid());
    return 0;
}