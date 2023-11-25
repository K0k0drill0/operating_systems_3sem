#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY_PATHNAME "server"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0660
#define MSG_SIZE 256
#define TERMINATION_PRIORITY 1
#define MAX_PRIOR 100

typedef struct message_text {
    int qid;
    char buf [MSG_SIZE];
} message_text;

typedef struct message {
    long message_type;
    struct message_text message_text;
} message;

void process_message(message* msg) {
    // Заменяем подряд идущие символы-разделители одним символом пробела
    char* ptr = msg->message_text.buf;
    char* result = msg->message_text.buf;

    while (*ptr != '\0') {
        if (*ptr == ' ' || *ptr == '\t') {
            // Заменяем подряд идущие символы-разделители на один символ пробела
            *result++ = ' ';
            while (*ptr == ' ' || *ptr == '\t') {
                ptr++;
            }
        } else {
            *result++ = *ptr++;
        }
    }
    *result = '\0';
    sleep(1);
}

int main (int argc, char **argv)
{
    key_t msg_queue_key;
    int qid;
    struct message message;

    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (1);
    }

    printf ("Server: Hello, World!\n");

    while (1) {
        // read an incoming message
        if (msgrcv (qid, &message, sizeof (struct message_text), -MAX_PRIOR, 0) == -1) {
            perror ("msgrcv");
            exit (1);
        }
        printf("%ld\n", message.message_type);

        if (message.message_type == TERMINATION_PRIORITY) {
            printf("Terminator has come!\n");
            exit (1);
        }

        printf ("Server: message received.\n");

        process_message(&message);

        int client_qid = message.message_text.qid;
        //message.message_text.qid = qid;

        // send reply message to client
        if (msgsnd (client_qid, &message, sizeof (struct message_text), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }

        printf ("Server: response sent to client.\n");
    }
    return 0;
}