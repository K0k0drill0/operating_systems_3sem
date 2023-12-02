#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY_PATHNAME "server"
#define PROJECT_ID 'M'
#define MSG_SIZE 256

enum status_codes {
    ok,
    FTOK_ERROR,
    MSGGET_ERROR,
    MSGRCV_ERROR,
    TERMINATOR,
    MSGSND_ERROR,
    MSGCTL_ERROR,
    UNABLE_TO_OPEN_A_FILE,
    OVERFLOWED,
    INVALID_PRIORITY, 
    INVALID_FILE,
    MEMORY_ISSUES,
    WRONG_AMOUNT_OF_ARGS
};

void print_error(int st) {
    switch (st)
    {
    case FTOK_ERROR:
        printf("Error in ftok.\n");
        break;
    case MSGGET_ERROR:
        printf("Error in msgget.\n");
        break;
    case MSGRCV_ERROR: 
        printf("Error in msgrcv.\n");
        break;
    case TERMINATOR: 
        printf("Met terminator.\n");
        break;
    case MSGSND_ERROR: 
        printf("Error in msgsnd.\n");
        break;
    case UNABLE_TO_OPEN_A_FILE:
        printf("Unable to open a file!\n");
        break;
    case OVERFLOWED:
        printf("Overflowed!\n");
        break;
    case INVALID_PRIORITY:
        printf("Invalid priority!\n");
        break;
    case INVALID_FILE:
        printf("Invalid file!\n");
        break;
    case MEMORY_ISSUES:
        printf("Can not allocate the memory!\n");
        break;
    case WRONG_AMOUNT_OF_ARGS:
        printf("Wrong amount of arguments!\n");
        break;
    default:
        break;
    }
}

typedef struct message_text {
    int qid;
    char buf [MSG_SIZE];
} message_text;

typedef struct message {
    long message_type;
    struct message_text message_text;
} message;

int read_line(FILE* inp, char** line) {
    int arr_max_size = 16;
    *line = (char*)malloc(sizeof(char) * arr_max_size);
    if (*line == NULL) {
        return MEMORY_ISSUES;
    }

    char c = getc(inp);

    int ind = 0;
    while (c != '\n' && c != EOF) {
        if (ind >= arr_max_size - 1) {
            arr_max_size *= 2;
            char *tmp = (char*)realloc(*line, arr_max_size * sizeof(char));
            if (tmp == NULL) {
                //free(str_inp);
                return MEMORY_ISSUES;
            }
            *line = tmp;
        }
        (*line)[ind] = c;
        ind++;
        c = getc(inp);
    }

    (*line)[ind] = '\0';

    if (c == EOF && ind == 0) {
        return EOF;
    }

    return ok;
}

int get_message(FILE* inp, message* msg) {
    char buf[256];
    if (fread(buf, 1, 6, inp) != 6) return EOF;
    
    if (fscanf(inp, "%ld", &(msg->message_type)) != 1) return INVALID_FILE;
    msg->message_type++;
    //msg->message_type = 0;
    // !!!!!!!!!!!!!!
    if (errno == ERANGE) {
        return OVERFLOWED;
    }

    if (msg->message_type < 1 || msg->message_type > 100) {
        return INVALID_PRIORITY;
    }

    if (fread(buf, 1, 7, inp) != 7) return EOF;

    // char c;
    // while ((c = getc(inp)) == '\n' || c == '\t' || c == ' ') {
    //     continue;
    // }

    char c;
    int ind = 0;
    int slashes_cnt = 0;
    while (!((c = getc(inp)) == '"' && !(slashes_cnt&1))) {
        if (ind == 256) {
            return INVALID_FILE;
        }
        if (c != '\\') {
            slashes_cnt = 0;
        }
        else {
            slashes_cnt++;
        }
        msg->message_text.buf[ind] = c;
        ind++;
    }
    msg->message_text.buf[ind] = '\0';

    while (!isalpha(c = getc(inp)) && c != EOF) {
        continue;
    }

    fseek(inp, -1, SEEK_CUR);
    return ok;
}

int main (int argc, char **argv)
{
    // argc = 2;
    // argv[1] = "pathes.txt";
    if (argc != 2) {
        print_error(WRONG_AMOUNT_OF_ARGS);
        return 1;
    }

    FILE* pathes = fopen(argv[1], "r");
    if (!pathes) {
        print_error(UNABLE_TO_OPEN_A_FILE);
        return 1;
    }

    key_t server_queue_key;
    int server_qid, myqid;
    struct message my_message, return_message;

    // create my client queue for receiving messages from server
    if ((myqid = msgget (IPC_PRIVATE, 0660)) == -1) {
        perror ("msgget: myqid");
        fclose(pathes);
        return 1;
    }

    if ((server_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        fclose(pathes);
        return 1;
    }

    if ((server_qid = msgget (server_queue_key, 0)) == -1) {
        perror ("msgget: server_qid");
        fclose(pathes);
        return 1;
    }

    my_message.message_type = 1;
    my_message.message_text.qid = myqid;

    printf ("Please type a message: ");

    int st;
    int sent_cnt = 0;

    char* file_path = NULL;
    while ((st = read_line(pathes, &file_path)) == ok) {
        FILE* inp = fopen(file_path, "r");
        if (inp == NULL) {
            st = UNABLE_TO_OPEN_A_FILE;
            break;
        }

        while ((st = get_message(inp, &my_message)) == ok) {
            my_message.message_text.qid = myqid;

            // send message to server
            if (msgsnd (server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
                perror ("client: msgsnd");
                free(file_path);
                fclose(pathes);
                fclose(inp);
                return 1;
            }
            sent_cnt++;

        }
        sleep(4);
        free(file_path);
        fclose(inp);
    }
    fclose(pathes);

    if (st != ok && st != EOF) {
        print_error(st);
    } 

    while (sent_cnt--) {
        if (msgrcv (myqid, &return_message, sizeof (struct message_text), 0, 0) == -1) {
            perror ("client: msgrcv");
            return 1;
        }

        // process return message from server
        printf ("Message received from server: %s\n\n", return_message.message_text.buf);  
    }

    // remove message queue
    if (msgctl (myqid, IPC_RMID, NULL) == -1) {
            perror ("client: msgctl");
            return 1;
    }

    printf ("Client: bye\n");

    exit (0);
}