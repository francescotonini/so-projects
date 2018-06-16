#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <tools.h>
#include <logger.h>
#include <types.h>
#include <constants.h>

int queue_id;

void catch_alarm(int sig_num) {
    if (sig_num == SIGALRM) {
        return;
    }
}

void logger() {
    signal(SIGALRM, catch_alarm);

    try_or_exit((queue_id = msgget(QUEUE_KEY, (IPC_CREAT | 0666))), "logger", "impossibile creare coda");

    while(polling_receive() != 1) {
        alarm(1);
        pause();
        alarm(0);
    }

    try_or_exit(msgctl(queue_id, IPC_RMID, NULL), "logger", "impossibile cancellare la coda");

    exit(EXIT_SUCCESS);
}

int polling_receive() {
    int result = 0;
    struct Message message;

    size_t size = sizeof(struct Message) - sizeof(long);
    int bytes_read;
    while ((bytes_read = msgrcv(queue_id, &message, size, 0, IPC_NOWAIT)) > 0) {
        if (message.mtype == 1) {
            result = 1;
        }

        if (message.text[127] != '\0') {
            // Sostituisce l'ultimo carattere con il carattere di terminazione di stringa
            message.text[127] = '\0';
        }

        println(message.text);
    };

    if (bytes_read == -1 && errno != ENOMSG) {
        try_or_exit(bytes_read, "logger", "impossibile leggere dalla coda");
    }

    return result;
}
