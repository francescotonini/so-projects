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

int s1_id;
int s2_id;

void padre(char *input_path, char *output_path) {
    /*
        * SIGINT  (Ctrl + C)
        * SIGTERM
        * SIGQUIT (Ctrl + \)
        * SIGSEGV
        * SIGPIPE
        * SIGTSTP (Ctrl + Z)
	*/
    signal(SIGINT, signal_error_handler);
	signal(SIGTERM, signal_error_handler);
	signal(SIGQUIT, signal_error_handler);
	signal(SIGSEGV, signal_error_handler);
	signal(SIGPIPE, signal_error_handler);
	signal(SIGTSTP, signal_error_handler);

    // Controlla che il file di output esista. Se così fosse, arresta il processo
    if (access(output_path, F_OK) == 0) {
        printerr("il file di output esiste");
        try_or_exit(-1, "padre", "il figlio di output esiste");
    }

    // Controlla che il file di input esista. Se così NON fosse, arresta il processo
    try_or_exit(access(input_path, F_OK | R_OK), "padre", "impossibile accedere al file di input");

    // Apre il file di input per contare il numero di righe nel file; infine, chiude il file
    int input_fd;
    int n_of_lines = 0;
    int n = 0;
    char buffer[BUFFER_SIZE];
    int offset = 0;
    try_or_exit((input_fd = open(input_path, O_RDONLY, 0644)), "padre", "impossibile aprire il file di input");
    while((n = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        int i;
        for (i = 0; i < n; i++, offset++) {
            if (buffer[i] == '>') {
                n_of_lines++;
                i += offset + 5;
                offset = 0;
                if (i > n) {
                    lseek(input_fd, i - n + 1, SEEK_CUR);
                }
            }
        }
    }
    try_or_exit(close(input_fd), "padre", "impossibile chiudere il file di input");
    
    // Crea, collega il segmento di memoria s1 e imposta il campo id_string dell'enum Status a 0
    void *s1 = attach_segments(SHKEY_S1, sizeof(struct Status) + (n_of_lines * sizeof(struct Entry)));
    struct Status *status = (struct Status *)s1;
    struct Entry *input = (struct Entry *)(s1 + sizeof(struct Status));
    status->id_string = 0;

    // Crea e collega il segmento di memoria s2
    void *s2 = attach_segments(SHKEY_S2, n_of_lines * sizeof(unsigned));
    unsigned *output = (unsigned *)s2;

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

    // Controlla e salva sul file le chiavi trovate
    check_keys(input, output, n_of_lines);
    save_keys(output_path, output, n_of_lines);

    // Scollega e rimuove i segmenti
    detach_segments(SHKEY_S1, s1);
    detach_segments(SHKEY_S2, output);

    // Termina
    exit(0);
}

void *attach_segments(key_t key, size_t size) {
    int shmid;
    try_or_exit((shmid = shmget(key, size, 0600 | IPC_CREAT | IPC_EXCL)), "padre", "errore shmget");

    if (key == SHKEY_S1) {
        s1_id = shmid;
    }
    else if (key == SHKEY_S2) {
        s2_id = shmid;
    }

    void *ptr;
    if ((ptr = shmat(shmid, NULL, 0)) == (void *) -1) {
        syserr("padre", "errore shmat");
    }

    return ptr;
}

void detach_segments(key_t key, void *attached_segment) {
    int shmid;
    try_or_exit((shmid = shmget(key, 0, 0600)), "padre", "errore shmget");

    shmdt(attached_segment);
    
    try_or_exit((shmctl(shmid, IPC_RMID, NULL)), "padre", "errore shmctl");
}

void load_file(char *name, void *ptr) {
    /* Breve descrizione del caricamento in memoria.
        La zona di memoria condivisa S1 è strutturata come un array di enumeratori Entry, dove ogni elemento contiene due array di unsigned di dimensione 128 e una
        variabile intera che indica il numero di elementi dell'array validi (ovvero gli elementi, da 0 a N, che sono da considerarsi parte del file di input).
    
        Il codice che segue apre, legge e chiude il file e nel frattempo converte i char in "blocchi" di variabili unsigned, saltando i caratteri ininfluenti
        usando un contatore assoluto.

        L'idea del ciclo while che legge il file è di avere il buffer sempre puntato al primo carattere della stringa cifrata o chiara, così da facilitare le operazioni
        di copia dei dati nella sezione di memoria condivisa S1. Ciò richiede maggiore logica per "saltare" i caratteri non utili (es. fine riga o caratteri di separazione) 
    */

    // Punto iniziale del segmento di memoria condiviso S1
    struct Entry *segment = (struct Entry *)ptr;

    // Apre il file di input
    int fd;
    try_or_exit((fd = open(name, O_RDONLY, 0644)), "padre", "impossibile creare il file di input");

    // Indica il numero di byte letti nella "read" corrente
    int n;
    // Buffer di lettura/scrittura
    char buffer[BUFFER_SIZE];
    // Indica la struttura che è in elaborazione. Questo puntatore verrà incrementato ogni volta che una riga intera sarà stata analizzata
    struct Entry *current_entry = segment;
    // Posizione corrente del file. E' un contatore assoluto del numero di byte letti
    int current_pos = 1;
    // Variabile di controllo dello stato di lettura della riga. Se la variabile ha valore 0, significa che buffer contiene dati della stringa plain.
    // Se la variabile ha valore 1, significa che buffer contiene dati della stringa encoded
    int current_entry_status = 0;

    // Salto il primo carattere (non utile ai fini del caricamento nel file)
    lseek(fd, current_pos, SEEK_SET);
    while ((n = read(fd, buffer, BUFFER_SIZE)) > 0) {
        if (current_entry_status == 0) {
            // Copia il buffer all'interno del campo "clear" della struttura Entry
            // La copia viene fatta "a blocchi di 4 caratteri"
            int i;
            for (i = 0; i < n && buffer[i] != '>'; i += 4) {
                current_entry->clear[i / 4] = *((unsigned *)(buffer + i));
            }

            current_entry->size = (i / 4);

            // Cerco il carattere che preannuncia la parte cifrata della riga, così da poter "saltare" a quella posizione
            for (i = 0; i < n; i++) {
                current_pos++;
                if (buffer[i] == '<') {
                    i = n;
                }
                else if (i == n - 1) {
                    current_pos += 3;
                }
            }
            lseek(fd, current_pos, SEEK_SET);

            // La prossima lettura riguarderà la parte cifrata della riga, modifico la variabile di conseguenza
            current_entry_status = 1;
        }
        else if (current_entry_status == 1) {
            // La copia viene effettuata a "blocchi di 4 caratteri"
            int i;
            for (i = 0; i < current_entry->size; i ++) {
                current_entry->encoded[i] = *((unsigned *)(buffer + (i * 4)));
            }

            // Salto alla prima posizione della prossima riga da analizzare (se esiste)
            current_pos += (current_entry->size * 4 + 3);
            lseek(fd, current_pos, SEEK_SET);

            // La lettura per la riga attuale è terminata, modifico il puntatore alla zona di memoria condivisa affinché passi alla prossima riga
            current_entry++;

            // La prossima lettura riguarderà la parte in chiaro della riga successiva, modifico la variabile di conseguenza
            current_entry_status = 0;
        }
    }

    if (n == -1) {
        try_or_exit(-1, "padre", "errore durante la lettura del file di input");
    }

    // Chiude il file di input
    try_or_exit(close(fd), "padre", "impossibile chiudere il file di input");
}

void save_keys(char *name, unsigned *keys, int n_of_lines) {
    // Crea e apre il file di output
    int fd;
    try_or_exit((fd = creat(name, O_RDWR | 0644)), "padre", "impossibile creare il file di input");

    int i;
    for (i = 0; i < n_of_lines; i++) {
        unsigned *this = (keys + i);
        char *converted = utoh(*this);

        write(fd, "0x", 2);
        write(fd, converted, 8);
        write(fd, "\r\n", 2);

        free(converted);
    }
    
    // Chiude il file di output
    try_or_exit(close(fd), "padre", "impossibile chiudere il file di output");
}

void check_keys(struct Entry *input, unsigned *output, int n_of_lines) {
    struct Entry *this_entry = input;
    int i;
    for (i = 0; i < n_of_lines; i++, this_entry++) {
        unsigned *key = (output + i);

        int j;
        for (j = 0; j < this_entry->size; j++) {
            unsigned clear = this_entry->clear[j];
            unsigned encoded = this_entry->encoded[j];
            if ((clear ^ *key) != encoded) {
                printerr("trovata una chiave non compatibile!");
            }
        }
    }
}
