#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 2) { 
        printf("Usage: ./%s  <filename>\n", argv[0]);
        return 1;
    }
    
    FILE* fp = fopen(argv[1], "wb");
    if (fp == NULL) {
        printf("Failed to create file %s\n", argv[1]);
        return 1;
    }

    unsigned char bytes[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    fwrite(bytes, sizeof(unsigned char), sizeof(bytes), fp); // добавить проверку

    fclose(fp);
    return 0;
}