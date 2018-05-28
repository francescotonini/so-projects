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

int my_string;
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
    char *input = (char *)(s1 + sizeof(struct Status));

    // Get logger queue
    if((queue_id = msgget(QUEUE_KEY, 0666)) == -1) {
        syserr("nipote", "impossibile accedere alla coda");
    }
    msg_size = sizeof(struct Message);

    while(load_string(lines) == 0) {
        unsigned key = find_key(input, output);
        save_key(key, output);
    }

    exit(0);
}

int load_string(int lines) {
    lock(0);
    my_string = status->id_string;
    if (my_string == lines) {
        unlock(0);
        return -1;
    }

    status->id_string = my_string + 1;
    status->grandson = id;

    kill(getppid(), SIGUSR1);

    lock(1);
    unlock(0);
    return 0;
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

unsigned find_key(char *input, unsigned *output) {
    time_t start = current_timestamp();

    char *line = (char *)(input + (my_string * 1030));
    // Find "middle point"
    int middle_index = 0; 
    while (line[middle_index] != ';') {
        middle_index++;
    }

    unsigned *clear = (unsigned *)(line + 1);
    unsigned *encrypted = (unsigned *)(line + middle_index + 2);

    unsigned key = 0;
    while ((*clear ^ key) != *encrypted) {
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

void save_key(unsigned key, unsigned *output) {
    unsigned *pos = (unsigned *)(output + (my_string * sizeof(unsigned)));
    *pos = key;
}
