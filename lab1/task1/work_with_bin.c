#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "rb");

    if (fp == NULL) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }

    unsigned char c;
    while ((fread(&c, sizeof(unsigned char), 1, fp)) == 1) {
        printf("%u, %p\n", c, fp->_Placeholder);
    }

    fclose(fp);

    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("Failed to open file %s\n", argv[1]);
        return -1;
    }

    fseek(fp, 3, SEEK_SET);

    unsigned char buffer[4];

    fread(buffer, sizeof(unsigned char), 4, fp); //добавить проверку

    printf("Buffer: ");
    for (int i = 0; i < 4; i++) {
        printf("%u ", buffer[i]);
    }
    printf("\n");

    fclose(fp);
    return 0;
}