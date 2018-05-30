#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>

#include <tools.h>
#include <nipote.h>
#include <types.h>
#include <constants.h>

int semid;
struct Status *status;
int id;
int msg_size;
int queue_id;

void nipote(int uid, int lines, void *s1, unsigned *output) {
    id = uid;

    // Get semaphore
    if ((semid = semget(SEM_KEY, 2, 0666)) == -1) {
        syserr("nipote", "impossibile recuperare semaforo");
    }

    status = (struct Status *)s1;
    struct Entry *input = (struct Entry *)(s1 + sizeof(struct Status));

    // Get logger queue
    if((queue_id = msgget(QUEUE_KEY, 0666)) == -1) {
        syserr("nipote", "impossibile accedere alla coda");
    }
    msg_size = sizeof(struct Message);

    int my_string;
    while((my_string = load_string(lines)) >= 0) {
        unsigned key = find_key(my_string, input, output);
        save_key(my_string, key, output);
    }

    exit(0);
}

int load_string(int lines) {
    lock(0);
    int my_string = status->id_string;
    if (my_string == lines) {
        unlock(0);
        return -1;
    }

    status->id_string = my_string + 1;
    status->grandson = id;

    kill(getppid(), SIGUSR1);

    lock(1);
    unlock(0);
    return my_string;
}

void lock(int id) {
    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = id;
    sops->sem_op = -1;
    sops->sem_flg = 0;

    if (semop(semid, sops, 1) == -1) {
        syserr("nipote", "impossibile bloccare il semaforo");
    }

    free(sops);
}

void unlock(int id) {
    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = id;
    sops->sem_op = 1;
    sops->sem_flg = 0;

    if (semop(semid, sops, 1) == -1) {
        syserr("nipote", "impossibile sbloccare il semaforo");
    }

    free(sops);
}

time_t current_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    return ts.tv_sec;
}

unsigned find_key(int my_string, struct Entry *input, unsigned *output) {
    time_t start = current_timestamp();

    struct Entry *this_entry = (input + my_string);
    unsigned clear = this_entry->clear;
    unsigned encoded = this_entry->encoded;
    unsigned key = 0;
    while ((clear ^ key) != encoded) {
        key++;
    }

    send_timeelapsed(current_timestamp() - start);
    return key;
}

void send_timeelapsed(time_t time) {
    struct Message end_message;
    end_message.mtype = 2;
    strcp(end_message.text, strcct("chiave trovata in ", itoa(time)));

    if (msgsnd(queue_id, &end_message, msg_size, 0) == -1) {
        syserr("nipote", "impossibile inviare messaggio");
    }
}

void save_key(int my_string, unsigned key, unsigned *output) {
    unsigned *pos = (output + my_string);
    *pos = key;
}
