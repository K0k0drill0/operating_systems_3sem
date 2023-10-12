#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

typedef long long ll;

enum status_codes {
    ok,
    INVALID_ARGS,
    INVALID_FILE,
    INVALID_MASK, 
    OVERFLOW
};

int xor8(FILE* inp, int* ans) {
    *ans = 0;
    unsigned char c = 0;

    while (fread(&c, sizeof(unsigned char), 1, inp) > 0) {
        *ans ^= (int)c;
    }

    return ok;
}

int xor32(FILE* inp, int* ans) {
    *ans = 0;
    unsigned int data;

    while (fread(&data, sizeof(unsigned int), 1, inp) > 0) {
        *ans ^= data;
    }  

    return ok;
}

int mask(FILE* inp, int* ans, char* hex) {
    unsigned long mask = strtoul(hex, NULL, 16);
    errno = 0;
    
    if ((errno == ERANGE && (mask == ULONG_MAX || mask == 0))) {
        return OVERFLOW;
    }


    unsigned int data;
    *ans = 0;
    while (fread(&data, sizeof(unsigned int), 1, inp) >= 1) {
        if ((data == mask)) {
            (*ans)++;
        }
        //fseek(inp, -3, SEEK_CUR);
    }
    return ok;
}

void print_error(int st) {
    switch (st)
    {
    case INVALID_ARGS:
        printf("Invalid arguments.\n");
        break;
    case INVALID_FILE:
        printf("Unable to open a file.\n");
        break;
    case INVALID_MASK:
        printf("Invalid mask.\n");
        break;
    default:
        break;
    }
}

int valid_args(int argc, char** argv) {
    if (argc != 3 && argc != 4) {
        return INVALID_ARGS;
    }

    if (strcmp(argv[2], "xor8") && strcmp(argv[2], "xor32") && strcmp(argv[2], "mask")) {
        return INVALID_ARGS;
    }

    if ((!strcmp(argv[2], "xor8") && argc != 3) || (!strcmp(argv[2], "xor32") && argc != 3) 
    || (!strcmp(argv[2], "mask") && argc != 4)) {
        return INVALID_ARGS;
    }

    if (argc == 3) {
        return ok;
    }

    int n = strlen(argv[3]);

    for (int i = 0; i < n; i++) {
        char c = argv[3][i];
        if (isdigit(c)) continue;
        if (!isalpha(c)) {
            return INVALID_MASK;
        }

        int tmp = tolower(c);

        if (tmp == c) {
            return INVALID_MASK;
        }

        if ((int)c - (int)'F' > 0) {
            return INVALID_MASK;
        }
    }

    return ok;
}

int main(int argc, char** argv) {
    int st = valid_args(argc, argv);
    if (st != ok) {
        print_error(st);
        return 1;
    }

    FILE* inp;
    inp = fopen(argv[1], "rb");

    if (!inp) {
        print_error(INVALID_FILE);
        return -1;
    }

    int ans = 0;

    if (!strcmp(argv[2], "xor8")) {
        xor8(inp, &ans);
        printf("Result of xor8 = %d\n", ans);
    }
    else if (!strcmp(argv[2], "xor32")) {
        xor32(inp, &ans);
        printf("Result of xor32 = %d\n", ans);
    }
    else {
        int st =mask(inp, &ans, argv[3]);
        if (st != ok) {
            print_error(st);
            return 1;
        }
        printf("Result of mask = %d\n", ans);
    } 

    return 0;
}