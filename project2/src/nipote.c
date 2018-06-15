#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
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
    if ((sem_id = semget(SEM_KEY, 1, 0666)) == -1) {
        syserr("nipote", "impossibile recuperare semaforo");
    }

    status = (struct Status *)data->s1;
    struct Entry *input = (struct Entry *)(data->s1 + sizeof(struct Status));

    // Get logger queue
    if((queue_id = msgget(QUEUE_KEY, 0666)) == -1) {
        syserr("nipote", "impossibile accedere alla coda");
    }

    int lines = data->lines;
    int my_string = 0;
    while(my_string < lines) {
        lock();

        my_string = status->id_string;
        if (my_string < lines) {
            status->id_string = status->id_string + 1;
            status->grandson = data->id;

            char *grandson = itoa(status->grandson);
            char *idString = itoa(status->id_string);
            char *str1 = strcct("Il thread ", grandson);
            char *str2 = strcct(str1, " sta analizzando la ");
            char *str3 = strcct(str2, idString);
            char *str4 = strcct(str3, "-esima stringa.\n");

            write(data->pipe, str4, strsize(str4));

            free(grandson);
            free(str1);
            free(str2);
            free(str3);
            free(str4);
            free(idString);

            #ifdef THREADS
            kill(getpid(), SIGUSR1);
            #else
            kill(getppid(), SIGUSR1);
            #endif

            unlock();

            struct Entry *this_entry = load_string(my_string, input);
            unsigned key = find_key(this_entry);
            save_key(my_string, key, data->output);
        }
        else {
            unlock();
        }  
    }

    #ifdef THREADS
    pthread_exit(0);
    #else
    exit(0);
    #endif
}

struct Entry *load_string(int id, struct Entry *input) {
    return (input + id);
}

void lock() {
    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = 0;
    sops->sem_op = -1;
    sops->sem_flg = 0;

    if (semop(sem_id, sops, 1) == -1) {
        if (errno == EINTR) {
            lock();
        }
        else {
            syserr("nipote", "impossibile bloccare il semaforo");
        }
    }

    free(sops);
}

void unlock() {
    struct sembuf *sops = (struct sembuf *)malloc(sizeof(struct sembuf));
    sops->sem_num = 0;
    sops->sem_op = 1;
    sops->sem_flg = 0;

    if (semop(sem_id, sops, 1) == -1) {
        if (errno == EINTR) {
            unlock();
        }
        else {
            syserr("nipote", "impossibile sbloccare il semaforo");
        }
    }

    free(sops);
}

time_t current_timestamp() {
    struct timeval timer;
    gettimeofday(&timer, NULL);

    return timer.tv_sec;
}

unsigned find_key(struct Entry *this_entry) {
    time_t start = current_timestamp();

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

    size_t size = sizeof(struct Message) - sizeof(long);
    if (msgsnd(queue_id, &end_message, size, 0) == -1) {
        syserr("nipote", "impossibile inviare messaggio");
    }

    free(timeToString);
    free(concat);
}

void save_key(int my_string, unsigned key, unsigned *output) {
    unsigned *pos = (output + my_string);
    *pos = key;
}
