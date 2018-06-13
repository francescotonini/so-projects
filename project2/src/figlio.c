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
#include <pthread.h>

#include <tools.h>
#include <figlio.h>
#include <nipote.h>
#include <types.h>
#include <constants.h>

// Variabili globali
struct Status *status;
int sem_id;

void figlio(int lines, void *s1, unsigned *output) {
    // Collego segnale
    signal(SIGUSR1, status_updated);

    // Crea due semafori. Il primo impostato a 1 mentre il secondo a 0
    if ((sem_id = semget(SEM_KEY, 2, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
        syserr("figlio", "impossibile creare i semafori");
    }
    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = 0;
    sops->sem_op = 1;
    sops->sem_flg = 0;
    if (semop(sem_id, sops, 1) == -1) {
        syserr("figlio", "impossibile impostare il semaforo a 1");
    }
    sops->sem_num = 1;
    sops->sem_op = 0;
    sops->sem_flg = 0;
    if (semop(sem_id, sops, 1) == -1) {
        syserr("figlio", "impossibile impostare il semaforo a 0");
    }

    free(sops);

    status = (struct Status *)s1;

    // Recupera coda logger
    int queue_id;
    if((queue_id = msgget(QUEUE_KEY, (IPC_CREAT | 0666))) == -1) {
        syserr("figlio", "impossibile accedere  alla coda");
    }
    int msg_size = sizeof(struct Message);

    #ifdef THREADS
    
    struct NephewData data[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        data[i].id = i + 1;
        data[i].s1 = s1;
        data[i].lines = lines;
        data[i].output = output;

        if (pthread_create(&threads[i], NULL, nipote, (void *)&data[i])) {
            syserr("figlio", "impossibile lanciare thread");
        }
    }
    
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    #else

    struct NephewData data[NUM_NEWPHEWS];
    pid_t forks[NUM_NEWPHEWS];

    int i;
    for (i = 0; i < NUM_NEWPHEWS; i++) {
        data[i].id = i + 1;
        data[i].s1 = s1;
        data[i].lines = lines;
        data[i].output = output;

        if ((forks[i] = fork()) == -1) {
            syserr("figlio", "impossibile creare processo nipote");
        }

        if (forks[i] == 0) {
            nipote((void *)&data[i]);
        }
    }
    
    for (i = 0; i < NUM_NEWPHEWS; i++) {
        wait(&forks[i]);
    }

    #endif

    // Rimuove semafori
    if (semctl(sem_id, 2, IPC_RMID) == -1) {
        syserr("figlio", "impossibile rimuovere i semafori");
    }

    send_terminate(queue_id, msg_size);
    exit(0);
}

void status_updated(int sig_num) {
    if (sig_num == SIGUSR1) {
        char *grandson = itoa(status->grandson);
        char *idString = itoa(status->id_string);
        #ifdef THREADS
        char *str1 = strcct("Il thread ", grandson);
        #else
        char *str1 = strcct("Il nipote ", grandson);
        #endif
        char *str2 = strcct(str1, " sta analizzando la ");
        char *str3 = strcct(str2, idString);
        char *str4 = strcct(str3, "-esima stringa.");

        println(str4);
        free(grandson);
        free(str1);
        free(str2);
        free(str3);
        free(str4);
        free(idString);
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
