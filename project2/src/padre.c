#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <tools.h>
#include <padre.h>
#include <figlio.h>
#include <types.h>
#include <fcntl.h>
#include <constants.h>

// Global variables
int shmid_s1;
int shmid_s2;
int input_fd;

int padre(char* input, char* output) {
    // Check if output file exists. If so, quit
    if (access(output, F_OK) == 0) {
        printerr("il file di output esiste");
        return 1;
    }

    // Check if input file exists. If not so, quit
    if (access(input, F_OK | R_OK) != 0) {
        syserr("padre", "errore file di input");
    }

    // Count number of lines
    if((input_fd = open(input, O_RDONLY, 0644)) == -1) {
        syserr("padre", "impossibile aprire il file");
    }

    int lines = 0;
    int n = 0;
    char buffer[BUFFER_SIZE];
    while((n = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n') {
                lines++;
            }
        }
    }

    lseek(input_fd, 0, SEEK_SET);

    void* s1 = attach_segments(SHKEY_S1, sizeof(struct Status) + (lines * 1030) + 1, IPC_CREAT | 0666);    
    struct Status* status = (struct Status*)s1;
    status->id_string = 0;
    char* file = (char*)(s1 + sizeof(struct Status));
    load_file(input, file);

    char* s2 = (char*)attach_segments(SHKEY_S2, lines * 1030, IPC_CREAT | 0666);

    // Create "figlio"
    pid_t pid_figlio;
    if ((pid_figlio = fork()) == -1) {
        syserr("padre", "impossibile creare figlio");
    }

    if (pid_figlio == 0) {
        // Figlio!
        return figlio(lines);
    }
    else {
        // Padre
        wait(&pid_figlio);

        // check_keys()
        save_keys(output, file);

        detach_segments(shmid_s1, s1);
        detach_segments(shmid_s2, s2);
    }

    return 0;
}

void* attach_segments(key_t key, size_t size, int flags) {
    int shmid;
    if((shmid = shmget(key, size, flags)) < 0) {
        syserr("padre", "shmget");
    }

    void* shm;
    if ((shm = shmat(shmid, NULL, 0)) == (void*) -1) {
        syserr("padre", "shmat");
    }

    if (key == SHKEY_S1) {
        shmid_s1 = shmid;
    }
    else if (key == SHKEY_S2) {
        shmid_s2 = shmid;
    }

    return shm;
}

void detach_segments(key_t key, char* attached_segment) {
    shmdt(attached_segment);
    
    if (shmctl(key, IPC_RMID, NULL) == -1) {
        syserr("padre", "shmctl");
    }
}

void load_file(char *name, char *segment) {
    int n;
    int offset = 0;
    char buffer[BUFFER_SIZE];
    while ((n = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0, j = 0; i < n; i++, j++) {
            int index = offset + j;

            segment[index] = buffer[i];
            if (buffer[i] == '\n') {
                offset += 1030;
                j = -1;
            }
        }
    }

    segment[offset] = '\0';

    if(close(input_fd) == -1) {
        syserr("padre", "impossibile chiudere il file");
    }
}

void save_keys(char *name, char *keys) {
    int fd;
    if((fd = creat(name, O_RDWR ^ 0644)) == -1) {
        syserr("padre", "impossibile creare il file");
    }

    // Copy chars on buffer
    int offset = 0;
    char buffer[BUFFER_SIZE];
    while (keys[offset] != '\0') {
        int i;
        for (i = 0; i < BUFFER_SIZE && keys[offset + i] != '\n'; i++) {
            int index = i + offset;
            buffer[i] = keys[index];
        }

        write(fd, buffer, i);
        offset += 1030;
    }

    if(close(fd) == -1) {
        syserr("padre", "impossibile chiudere il file");
    }
}

void check_keys(char *keys) {

}
