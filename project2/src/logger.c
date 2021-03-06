#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>

#include <tools.h>
#include <logger.h>
#include <types.h>
#include <constants.h>

int msg_size;
int queue_id;

void logger() {
    if((queue_id = msgget(QUEUE_KEY, (IPC_CREAT | 0666))) == -1) {
        syserr("logger", "impossibile creare la coda");
    }

    msg_size = sizeof(struct Message);
    while(polling_receive() != 1) {
        sleep(1);
    }

    if (msgctl(queue_id, IPC_RMID, NULL) == -1) {
        syserr("logger", "impossibile cancellare la coda");
    }

    exit(0);
}

int polling_receive() {
    int result = 0;
    int status = 0;
    struct Message message;

    while (result != 1 && (status = msgrcv(queue_id, &message, msg_size, 0, 0)) >= 0) {
        if (message.mtype == 1) {
            result = 1;
        }

        if (message.text[127] != '\0') {
            // Sostituisce l'ultimo carattere con il carattere di terminazione di stringa
            message.text[127] = '\0';
        }

        println(message.text);
    };

    if (status == -1) {
        syserr("logger", "impossibile leggere dalla coda");
    }

    return result;
}
