#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <tools.h>
#include <nipote.h>
#include <types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <constants.h>

int my_string;
int semid;
struct Status* status;
char* input;
char* output;
int total_lines;
pid_t parent_pid;
int id;

int nipote(pid_t pid, int lines, int uid) {
    parent_pid = pid;
    total_lines = lines;
    id = uid;

    // Get semaphore
    if ((semid = semget(SEM_KEY, 1, 0666)) == -1) {
        syserr("nipote", "impossibile recuperare semaforo");
    }

    // Get and attach to shared memory (s1)
    int shmid_s1;
    if((shmid_s1 = shmget(SHKEY_S1, sizeof(struct Status), 0666)) < 0) {
        syserr("nipote", "shmget");
    }
    void* shm_s1;
    if ((shm_s1 = shmat(shmid_s1, NULL, 0)) == (void*) -1) {
        syserr("nipote", "shmat");
    }
    status = (struct Status*)shm_s1;
    input = (char*)(shm_s1 + sizeof(struct Status));

    // Get and attach to shared memory (s1)
    int shmid_s2;
    if((shmid_s2 = shmget(SHKEY_S2, sizeof(struct Status), 0666)) < 0) {
        syserr("nipote", "shmget");
    }
    void* shm_s2;
    if ((shm_s2 = shmat(shmid_s2, NULL, 0)) == (void*) -1) {
        syserr("nipote", "shmat");
    }

    output = (char*)(shm_s2);

    while(load_string() == 0) {
        find_key();
    }

    shmdt(shm_s1);
    shmdt(shm_s2);
}

int load_string() {
    lock();
    my_string = status->id_string;
    if (my_string == total_lines) {
        unlock();
        return -1;
    }

    status->id_string = my_string++;
    status->grandson = id;
    kill(parent_pid, SIGUSR1);

    unlock();
    return 0;
}

void lock() {
    struct sembuf *sops = (struct sembuf*) malloc(sizeof(struct sembuf));
    sops->sem_num = 0;
    sops->sem_op = -1;
    sops->sem_flg = 0;

    if (semop(semid, sops, 1) == -1) {
        syserr("nipote", "unable to lock semaphore");
    }
}

void unlock() {
    struct sembuf *sops = (struct sembuf*) malloc(sizeof(struct sembuf));
    sops->sem_num = 0;
    sops->sem_op = 1;
    sops->sem_flg = 0;

    if (semop(semid, sops, 1) == -1) {
        syserr("nipote", "unable to unlock semaphore");
    }
}

void find_key() {
    char *line = (char *)(input + (my_string * 1030));
    // Find "middle point"
    int middle_index = 0; 
    while (line[middle_index] != ';') {
        middle_index++;
    }

    char *clear = line;
    char *encrypted = (char *)(line + middle_index);

    for (int i = 0; i < middle_index; i++) {
        char key = (char)0;
        while(clear[i] ^ key != encrypted[i]) {
            key++;
        }

        output[(my_string * 512) + i] = key;
    }

    output[(my_string * 512)] = '\0';
}

void send_timeelapsed() {

}

void save_key() {

}
