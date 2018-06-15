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
#ifdef THREADS
#include <pthread.h>
#endif

#include <tools.h>
#include <figlio.h>
#include <nipote.h>
#include <types.h>
#include <constants.h>

// Variabili globali
struct Status *status;
int sem_id;
int p[2];

void figlio(int lines, void *s1, unsigned *output) {
    // Collego segnale
    signal(SIGUSR1, status_updated);

    // Creo pipe
    try_or_exit(pipe(p), "figlio", "impossibile creare pipe");

    fcntl(p[0], F_SETFL, O_NONBLOCK);
    fcntl(p[1], F_SETFL, O_NONBLOCK);

    // Crea un semaforo per accedere in modo esclusivo a status
    try_or_exit((sem_id = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0666)), "figlio", "impossibile creare il semaforo");

    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = 0;
    sops->sem_op = 1;
    sops->sem_flg = 0;
    try_or_exit(semop(sem_id, sops, 1), "figlio", "impossibile impostare il semaforo a 1");

    free(sops);

    status = (struct Status *)s1;

    // Recupera coda logger
    int queue_id;
    try_or_exit((queue_id = msgget(QUEUE_KEY, (IPC_CREAT | 0666))), "figlio", "impossibile accedere alla coda");

    #ifdef THREADS
    
    struct NephewData data[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        data[i].id = i + 1;
        data[i].s1 = s1;
        data[i].lines = lines;
        data[i].output = output;
        data[i].pipe = p[1];

        try_or_exit(pthread_create(&threads[i], NULL, nipote, (void *)&data[i]), "figlio", "impossibile lanciare thread");
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
        data[i].pipe = p[1];

        try_or_exit((forks[i] = fork()), "figlio", "impossibile creare processo padre");        

        if (forks[i] == 0) {
            nipote((void *)&data[i]);
        }
    }
    
    for (i = 0; i < NUM_NEWPHEWS; i++) {
        wait(&forks[i]);
    }

    #endif

    // Rimuove semafori
    try_or_exit((semctl(sem_id, 1, IPC_RMID)), "figlio", "impossibile rimuovere il semaforo");

    send_terminate(queue_id);
    exit(EXIT_SUCCESS);
}

void status_updated(int sig_num) {
    if (sig_num == SIGUSR1) {
        char buffer[512];
        int n;
        while((n = read(p[0], buffer, 512)) > 0) {
            write(STDOUT, buffer, n);
        }

        try_or_exit(n, "figlio", "impossibile leggere dalla pipe");
    }
}

void send_terminate(int queue_id) {
    struct Message end_message;
    end_message.mtype = 1;
    strcp(end_message.text, "ricerca conclusa.");

    size_t size = sizeof(struct Message) - sizeof(long);
    try_or_exit(msgsnd(queue_id, &end_message, size, 0), "figlio", "impossibile inviare messaggio di terminazione");
}
