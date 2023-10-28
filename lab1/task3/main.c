#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

enum status_codes {
    INVALID_ARGS,
    INVALID_FILE, 
    MEMORY_ISSUES
};

void print_error(int st) {
    switch (st)
    {
    case INVALID_ARGS:
        printf("Invalid arguments.\n");
        break;
    case INVALID_FILE:
        printf("Unable to open a file.\n");
        break;
    case MEMORY_ISSUES:
        printf("No memory.\n");
        break;
    default:
        break;
    }
}


int main(int argc, char** argv) {
    if (argc != 3) {
        print_error(INVALID_ARGS);
        return 1;
    }

    FILE* inp;
    FILE* outp;
    inp = fopen(argv[1], "rb");
    outp = fopen(argv[2], "wb+");

    if (!inp || !outp) {
        fclose(inp);
        fclose(outp);
        print_error(INVALID_FILE);
        return -1;
    }

    unsigned char buffer = 0;

    while (fread(&buffer, sizeof(buffer), 1, inp) > 0) {
        if (fwrite(&buffer, sizeof(buffer), 1, outp) < 1) {
            print_error(MEMORY_ISSUES);
            return 2;
        }
    }

    printf("File %s copied to %s\n", argv[1], argv[2]);

    fclose(inp);
    fclose(outp);

    return 0;

}