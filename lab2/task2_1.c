#include <stdio.h>
#include <unistd.h>

int main() {
    int pid;
    pid=fork();
    printf("%d\n", pid);
    printf("fuck\n");
    return 0;

    /*
    Сначала эта программа создает дочерний процесс (копия родительского). 
    После этого продолжает выполняться родительский процесс, и выводится айди
    дочернего процесса. Далее выполняется дочерний процесс
    */
}