#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    printf("Process ID: %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());
    printf("Group ID: %d\n", getgid());
    printf("Real User ID: %d\n", getuid());
    printf("Real Group ID: %d\n", getgid());
    printf("Effective User ID: %d\n", geteuid());
    printf("Effective Group ID: %d\n", getegid());
    return 0;
}