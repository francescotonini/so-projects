#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <stdlib.h>

#include <tools.h>
#include <figlio.h>
#include <nipote.h>
#include <types.h>
#include <constants.h>

// Global variables
struct Status *status;
int semid;

void figlio(int lines, void *s1, unsigned *output) {
    // Collego segnale
    signal(SIGUSR1, status_updated);

    // Crea due semafori
    if ((semid = semget(SEM_KEY, 2, IPC_CREAT | 0666)) < 0) {
        syserr("figlio", "impossibile creare i semafori");
    }
    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = 0;
    sops->sem_op = 1;
    sops->sem_flg = 0;
    if (semop(semid, sops, 1) == -1) {
        syserr("figlio", "impossibile impostare il semaforo a 1");
    }
    sops->sem_num = 1;
    sops->sem_op = 0;
    sops->sem_flg = 0;
    if (semop(semid, sops, 1) == -1) {
        syserr("figlio", "impossibile impostare il semaforo a 0");
    }

    status = (struct Status *)s1;

    // Recupera coda logger
    int queue_id;
    if((queue_id = msgget(QUEUE_KEY, (IPC_CREAT | 0666))) == -1) {
        syserr("figlio", "impossibile accedere  alla coda");
    }
    int msg_size = sizeof(struct Message);

    // Crea nipoti
    pid_t nephew1 = fork();
    if(nephew1 == -1) {
        syserr("figlio", "impossibile creare nipote");
    }
    else if(nephew1 == 0) {
        nipote(1, lines, s1, output);
    }

    pid_t nephew2 = fork();
    if(nephew2 == -1) {
        syserr("figlio", "impossibile creare nipote");
    }
    else if(nephew2 == 0) {
        nipote(2, lines, s1, output);
    }

    wait(&nephew1);
    wait(&nephew2);

    // Rimuove semafori
    if (semctl(semid, 2, IPC_RMID) == -1) {
        syserr("figlio", "impossibile rimuovere i semafori");
    }

    send_terminate(queue_id, msg_size);
    exit(0);
}

void status_updated(int sig_num) {
    if (sig_num == SIGUSR1) {
        char *grandson = itoa(status->grandson);
        char *str1 = strcct("Il nipote ", grandson);
        str1 = strcct(str1, " sta analizzando la ");
        str1 = strcct(str1, itoa(status->id_string));
        str1 = strcct(str1, "-esima stringa.");

        println(str1);
        free(grandson);
        free(str1);
        unlock(1);
    }
}

void send_terminate(int queue_id, int msg_size) {
    struct Message end_message;
    end_message.mtype = 1;
    strcp(end_message.text, "ricerca conclusa.");

    if (msgsnd(queue_id, &end_message, msg_size, 0) == -1) {
        syserr("figlio", "impossibile inviare messaggio di terminazione");
    }
}
