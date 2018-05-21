#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <tools.h>
#include <figlio.h>
#include <nipote.h>
#include <types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <constants.h>

struct Status* status;

int figlio(int lines) {
    // Attach signal
    // signal(SIGUSR1, status_updated);

    // Create semaphore and set it to 1
    int semid;
    if ((semid = semget(SEM_KEY, 1, IPC_CREAT | 0666)) == -1) {
        syserr("figlio", "impossibile creare il semaforo");
    }

    struct sembuf *sops = (struct sembuf*) malloc(sizeof(struct sembuf));
    sops->sem_num = 0;
    sops->sem_op = 1;
    sops->sem_flg = 0;

    if (semop(semid, sops, 1) == -1) {
        syserr("figlio", "impossibile impostare il semaforo a 1");
    }

    // Get and attach to shared memory
    int shmid;
    if((shmid = shmget(SHKEY_S1, sizeof(struct Status), 0666)) < 0) {
        syserr("figlio", "shmget");
    }

    void* shm;
    if ((shm = shmat(shmid, NULL, 0)) == (void*) -1) {
        syserr("figlio", "shmat");
    }

    status = (struct Status*)shm;

    // Create nephews
    pid_t my_pid = getpid();
    pid_t nephew1 = fork();
    if(nephew1 == -1) {
        syserr("figlio", "impossibile creare nipote");
    }
    else if(nephew1 == 0) {
        return nipote(my_pid, 1, lines);
    }

    return 0;

    pid_t nephew2 = fork();
    if(nephew2 == -1) {
        syserr("figlio", "impossibile creare nipote");
    }
    else if (nephew2 == 0) {
        return nipote(my_pid, 2, lines);
    }

    wait(&nephew1);
    wait(&nephew2);

    return 0;
}

void status_updated(int sig_num) {
    println("Il nipote sta analizzando la stringa");
}

void send_terminate() {

}
