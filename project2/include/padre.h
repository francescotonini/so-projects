/// @file
/// @defgroup padre struttura processo padre
/// @{

#include <sys/shm.h>
#include <sys/ipc.h>

/**
 * @brief wrapper del processo padre
 * @param input_path percorso del file di input
 * @param output_path percorsro del file di output
 * @info se il file di output esiste, il processo termina immediatamente
 */
void padre(char *input_path, char *output_path);

/**
 * @brief crea un e collega un segmento di memoria condivisa
 * @param key identificatore segmento di memoria condivisa
 * @param size dimensione segmento
 * @return indirizzo della memoria condivisa appena creata e collegata
 */
void *attach_segments(key_t key, size_t size);

/**
 * @brief scollega e rimuove il segmento di memoria condivisa
 * @param key identificatore segmento di memoria condivisa
 * @param attached_segment indirizzo di memoria condivisa da scollegare e rimuvoere
 */
void detach_segments(key_t key, void *attached_segment);

/**
 * @brief carica il file di input
 * @param name nome file
 * @param ptr indirizzo alla posizione di memoria del segmento condiviso S1 dove sarà copiato il file
 */
struct Entry *load_file(char *name, void *ptr);

/**
 * @brief salva le chiavi trovate sul file di output indicato
 * @param name percorso file di output
 * @param keys indirizzo alla zona di memoria condivisa S2 che contiene tutte le chiavi
 * @param n_of_lines numero di righe del file di input, ovvero il numero di chiavi totali
 */
void save_keys(char *name, unsigned *keys, int n_of_lines);

/**
 * @brief controlla che le chiavi siano corrette per tutta la lunghezza delle stringhe
 * @param input segmento di memoria che contiene il file di input
 * @param output segmento di memoria che contiene le chiavi trovate
 * @param n_of_lines numero di righe del file di input/output
 * @return 0 se tutte le chiavi sono state verificate correttamente; altrimenti -1
 */
int check_keys(struct Entry *input, unsigned *output, int n_of_lines);

/// @}