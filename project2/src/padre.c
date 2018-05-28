#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>

#include <tools.h>
#include <padre.h>
#include <figlio.h>
#include <logger.h>
#include <types.h>
#include <constants.h>

// Global variables
int shmid_s1;
int shmid_s2;

void padre(char *input_path, char *output_path) {
    // Controlla che il file di output esista. Se così fosse, arresta il processo
    if (access(output_path, F_OK) == 0) {
        printerr("il file di output esiste");
        exit(1);
    }

    // Controlla che il file di input esista. Se così NON fosse, arresta il processo
    if (access(input_path, F_OK | R_OK) != 0) {
        syserr("padre", "errore nell'accedere file di input");
    }

    // Apre il file di input per contare il numero di righe nel file; infine, chiude il file
    int input_fd;
    int n_of_lines = 0;
    int n = 0;
    char buffer[BUFFER_SIZE];
    if((input_fd = open(input_path, O_RDONLY, 0644)) == -1) {
        syserr("padre", "impossibile aprire il file di input");
    }
    while((n = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n') {
                n_of_lines++;
            }
        }
    }
    if(close(input_fd) == -1) {
        syserr("padre", "impossibile chiudere il file di input");
    }

    // Crea, collega il segmento di memoria s1 e imposta il campo id_string dell'enum Status a 0
    void *s1 = attach_segments(SHKEY_S1, sizeof(struct Status) + (n_of_lines * 1030), IPC_CREAT | 0600);
    struct Status *status = (struct Status *)s1;
    char *input = (char *)(s1 + sizeof(struct Status));
    status->id_string = 0;

    // Crea e collega il segmento di memoria s2
    unsigned *output = (unsigned *)attach_segments(SHKEY_S2, n_of_lines * sizeof(unsigned), IPC_CREAT | 0600);

    // Carica il file in input
    load_file(input_path, input);

    // Crea logger
    pid_t pid_logger;
    if ((pid_logger = fork()) == -1) {
        syserr("padre", "impossibile creare logger");
    }
    else if (pid_logger == 0) {
        logger();
    }

    // Crea figlio
    pid_t pid_figlio;
    if ((pid_figlio = fork()) == -1) {
        syserr("padre", "impossibile creare figlio");
    }
    else if (pid_figlio == 0) {
        figlio(n_of_lines, s1, output);
    }

    // Attende i figli
    wait(&pid_figlio);
    wait(&pid_logger);

    // Controlla le chiavi e salva
    if (check_keys(input, output, n_of_lines) == 0) { 
        save_keys(output_path, output, n_of_lines);
    }

    detach_segments(shmid_s1, status);
    detach_segments(shmid_s2, output);

    exit(0);
}

void *attach_segments(key_t key, size_t size, int flags) {
    int shmid;
    if((shmid = shmget(key, size, flags)) < 0) {
        syserr("padre", "errore shmget");
    }

    void *shm;
    if ((shm = shmat(shmid, NULL, 0)) == (void *) -1) {
        syserr("padre", "errore shmat");
    }

    if (key == SHKEY_S1) {
        shmid_s1 = shmid;
    }
    else if (key == SHKEY_S2) {
        shmid_s2 = shmid;
    }

    return shm;
}

void detach_segments(key_t key, void *attached_segment) {
    shmdt(attached_segment);
    
    if (shmctl(key, IPC_RMID, NULL) == -1) {
        syserr("padre", "errore shmctl");
    }
}

void load_file(char *name, char *segment) {
    // Apre il file di input
    int fd;
    if((fd = open(name, O_RDONLY, 0644)) == -1) {
        syserr("padre", "impossibile creare il file di input");
    }

    // Copia dal file di input nel segmento di memoria
    int n;
    int offset = 0;
    char buffer[BUFFER_SIZE];
    while ((n = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0, j = 0; i < n; i++, j++) {
            int index = offset + j;

            segment[index] = buffer[i];
            if (buffer[i] == '\n') {
                offset += 1030;
                j = -1;
            }
        }
    }

    // Chiude il file di input
    if(close(fd) == -1) {
        syserr("padre", "impossibile chiudere il file di input");
    }
}

void save_keys(char *name, unsigned *keys, int n_of_lines) {
    // Crea e apre il file di output
    int fd;
    if((fd = creat(name, O_RDWR ^ 0644)) == -1) {
        syserr("padre", "impossibile creare il file di output");
    }

    for (int i = 0; i < n_of_lines; i++) {
        unsigned *this = (unsigned *)(keys + (i * sizeof(unsigned)));
        char *converted = utoh(*this);

        write(fd, "0x", 2);
        write(fd, converted, 8);
        write(fd, "\n", 1);

        free(converted);
    }

    // Chiude il file di output
    if(close(fd) == -1) {
        syserr("padre", "impossibile chiudere il file di output");
    }
}

int check_keys(char *input, unsigned *output, int n_of_lines) {
    for (int i = 0; i < n_of_lines; i++) {
        // Find middle point of string
        int middle_index = 0;
        while ((input + (1030 * i))[middle_index] != ';') {
            middle_index++;
        }

        void *base = (void *)(input + (1030 * i));

        unsigned *clear = (unsigned *)(base + 1);
        unsigned *encrypted = (unsigned *)(base + middle_index + 2);
        unsigned *key = (unsigned *)(output + (i * sizeof(unsigned)));

        if ((*clear ^ *key) != *encrypted) {
            println("trovata una chiave non compatibile!");
            return -1;
        }
    }

    return 0;
}
