#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#ifdef THREADS
#include <pthread.h>
#endif
#include <tools.h>
#include <nipote.h>
#include <types.h>
#include <constants.h>

int sem_id;
struct Status *status;
int queue_id;

void *nipote(void *ptr) {
    struct NephewData *data = (struct NephewData *)ptr;

    // Get semaphore
    if ((sem_id = semget(SEM_KEY, 2, 0666)) == -1) {
        syserr("nipote", "impossibile recuperare semaforo");
    }

    status = (struct Status *)data->s1;
    struct Entry *input = (struct Entry *)(data->s1 + sizeof(struct Status));

    // Get logger queue
    if((queue_id = msgget(QUEUE_KEY, 0666)) == -1) {
        syserr("nipote", "impossibile accedere alla coda");
    }

    int my_string;
    while((my_string = load_string(data->lines, data->id)) >= 0) {
        unsigned key = find_key(my_string, input);
        save_key(my_string, key, data->output);
    }

    #ifdef THREADS
    pthread_exit(0);
    #else
    exit(0);
    #endif
}

int load_string(int lines, int id) {
    lock(0);
    int my_string = status->id_string;
    if (my_string > lines - 1) {
        unlock(0);
        return -1;
    }

    status->id_string = status->id_string + 1;
    status->grandson = id;

    #ifdef THREADS
    kill(getpid(), SIGUSR1);
    #else
    kill(getppid(), SIGUSR1);
    #endif

    lock(1);
    unlock(0);
    return my_string;
}

void lock(int id) {
    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = id;
    sops->sem_op = -1;
    sops->sem_flg = 0;

    if (semop(sem_id, sops, 1) == -1) {
        syserr("nipote", "impossibile bloccare il semaforo");
    }

    free(sops);
}

void unlock(int id) {
    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = id;
    sops->sem_op = 1;
    sops->sem_flg = 0;

    if (semop(sem_id, sops, 1) == -1) {
        syserr("nipote", "impossibile sbloccare il semaforo");
    }

    free(sops);
}

time_t current_timestamp() {
    struct timeval timer;
    gettimeofday(&timer, NULL);

    return timer.tv_sec;
}

unsigned find_key(int my_string, struct Entry *input) {
    time_t start = current_timestamp();

    struct Entry *this_entry = (input + my_string);
    unsigned clear = this_entry->clear[0];
    unsigned encoded = this_entry->encoded[0];
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
    char *text = "chiave trovata in ";
    char *timeToString = itoa(time);
    char *concat = strcct(text, timeToString);
    strcp(end_message.text, concat);

    if (msgsnd(queue_id, &end_message, sizeof(struct Message), 0) == -1) {
        syserr("nipote", "impossibile inviare messaggio");
    }

    free(timeToString);
    free(concat);
}

void save_key(int my_string, unsigned key, unsigned *output) {
    unsigned *pos = (output + my_string);
    *pos = key;
}
