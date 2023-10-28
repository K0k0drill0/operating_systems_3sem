#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

enum status_code {
    ok,
    WRONG_AMOUNT_OF_ARGUMENTS,
    UNABLE_TO_OPEN_A_FILE,
    MEMORY_ISSUES,
    EQUAL,
    NOT_EQUAL,
    FOUND,
    NOT_FOUND,
    FAILED_FORK
};

void print_error(int st) {
    switch (st)
    {
    case WRONG_AMOUNT_OF_ARGUMENTS:
        printf("Wrong amount of arguments!\n");
        break;
    case UNABLE_TO_OPEN_A_FILE:
        printf("Unable to open a file!\n");
        break;
    case MEMORY_ISSUES:
        printf("Can not allocate the memory!\n");
        break;
    case FAILED_FORK:
        printf("Failed to fork!\n");
        break;
    default:
        break;
    }
}


typedef struct result_list {
    char* filename;
    int line;
    int ind_in_line;
    struct result_list* next;
    struct result_list* prev;
} result_list;

int create_result_list(result_list** head, char* filename) {
    *head = (result_list*)malloc(sizeof(result_list));
    if (head == NULL) {
        return MEMORY_ISSUES;
    }
    (*head)->filename = filename;
    (*head)->next = NULL;
    (*head)->prev = NULL;
    return ok;
}

int add_node(result_list*** prev, char* filename) {
    if (*prev == NULL) {
        return MEMORY_ISSUES;
    }
    result_list* new_el;
    new_el = (result_list*)malloc(sizeof(result_list));
    if (!new_el) {
        return MEMORY_ISSUES;
    }
    new_el->filename = filename;
    new_el->next = NULL;
    new_el->prev = **prev;
    if (**prev != NULL) {
        (**prev)->next = new_el;
    }
    **prev = new_el;
    return ok;
}

void free_list(result_list* tail) {
    if (tail == NULL) {
        return;
    }
    result_list* tmp = tail->prev;
    free(tail->filename);
    free(tail);
    free_list(tmp);
}

void print_list(result_list* tail) {
    if (tail == NULL) {
        printf("There is no such file!\n");
        return;
    }
    result_list* tmp = tail;
    while ((tmp->prev) != NULL) {
        tmp = tmp->prev; 
    }
    while (tmp != NULL) {
        printf("The name of file is %s\n", tmp->filename);
        printf("\n");
        tmp = tmp->next;
    }
}

int is_separator(char c) {
    return (c == ' ' || c == '\t' || c == '\n');
}

int read_line(FILE* inp, char** str_inp) {
    int arr_max_size = 16;
    *str_inp = (char*)malloc(sizeof(char) * arr_max_size);
    if (*str_inp == NULL) {
        return MEMORY_ISSUES;
    }

    char c;
    c = getc(inp);
    int ind = 0;
    while (is_separator(c) && c != EOF) {
        c = getc(inp);
    }

    if (c == EOF) {
        free(*str_inp);
        return EOF;
    }

    while (!is_separator(c) && c != EOF) {
        if (ind >= arr_max_size - 1) {
            arr_max_size *= 2;
            char *tmp = (char*)realloc(*str_inp, arr_max_size * sizeof(char));
            if (tmp == NULL) {
                free(str_inp);
                return MEMORY_ISSUES;
            }
            *str_inp = tmp;
        }
        (*str_inp)[ind] = c;
        ind++;
        c = getc(inp);
    }
    
    (*str_inp)[ind] = '\0';
    return ok;
}

int str_cmp(char* str, FILE* inp) {
    int n = strlen(str);
    char c;
    int st = EQUAL;
    int i = 1;
    while (i < n) {
        c = getc(inp);
        if (c == EOF) {
            st = EOF;
            break;
        }
        if (c != str[i]) {
            st = NOT_EQUAL;
            break;
        }
        i++;
    }

    fseek(inp, -i, SEEK_CUR);
    return st;
}

int searching_str_in_file(FILE* inp, char* substr) {

    int n = strlen(substr);

    char c = getc(inp);
    while (c != EOF) {
        if (c != substr[0]) {
            c = getc(inp);
            continue;
        }
        int st = str_cmp(substr, inp);
        if (st == EQUAL) {
            return FOUND;
        }
        else if (st == EOF) {
            break;
        }
        c = getc(inp);
    }
    return NOT_FOUND;
}

// char* get_filename(FILE* inp_file, int num) {
//     char* filename;
//     int st;
//     while ((st = read_line(inp_file, &filename)) == ok && num > 1) {
//         free(filename);
//         num--;
//     }
//     // printf("%s\n", filename);
//     if (st != ok) {
//         free(filename);
//         return NULL;
//     }
//     return filename;
// }

int go_on_files(FILE* inp_file, char* str, result_list** ans) {
    char* filename;
    int amount = 0;
    while (read_line(inp_file, &filename) == ok) {

        FILE* file_to_work_with = fopen(filename, "r");
        if (!file_to_work_with) {
            free(filename);
            return UNABLE_TO_OPEN_A_FILE;
        }

        pid_t pid = fork();
        if (pid == -1) {
            free(filename);
            fclose(file_to_work_with);
            return FAILED_FORK;
        }
        else if (pid == 0) {
            if (searching_str_in_file(file_to_work_with, str) == FOUND) {
                free(filename);
                fclose(file_to_work_with);
                _Exit(1);
            }
            else {
                free(filename);
                fclose(file_to_work_with);
                _Exit(0);
            }
        }

        int status;
        while((pid = wait(&status)) > 0) {
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                amount += exit_status;
                if (exit_status > 0) {
                    if (*ans == NULL) {

                        int st = create_result_list(ans, filename);
                        if (st != ok) {
                            free(filename);
                            fclose(file_to_work_with);
                            return st;
                        }
                    }
                    else {
                        int st = add_node(&ans, filename);
                        if (st != ok) {
                            free(filename);
                            fclose(file_to_work_with);
                            return st;
                        }
                    } 
                }
                else {
                    free(filename);
                }
            }
            fclose(file_to_work_with);
        }
        //free(filename);
        //fclose(file_to_work_with);
    }
    return ok;
}

int main(int argc, char** argv) { // передаем путь к файлу с путями к файлам и искомую подстроку
    // if (argc != 3) {
    //     print_error(WRONG_AMOUNT_OF_ARGUMENTS);
    //     return 1;
    // }

    argv[1] = "names_of_files.txt";
    argv[2] = "aba";

    FILE* inp = fopen(argv[1], "r");
    if (!inp) {
        print_error(UNABLE_TO_OPEN_A_FILE);
        return -1;
    }

    char* inp_str = argv[2];

    result_list* list = NULL;
    int st = go_on_files(inp, inp_str, &list);
    if (st != ok) {
        print_error(st);
        free_list(list);
        fclose(inp);
        return 1;
    }

    print_list(list);
    free_list(list);
    fclose(inp);
    return 0;
}