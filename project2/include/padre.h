/// @file
/// @defgroup padre struttura processo padre
/// @{

#include <sys/shm.h>
#include <sys/ipc.h>

/**
 * @brief wrapper del processo padre
 * @param input_path nome del file di input
 * @param output_path nome del file di output
 */
void padre(char *input_path, char *output_path);

/**
 * @brief crea il segmento di memoria condivisa
 * @param key identificatore segmento
 * @param size dimensione segmento
 * @param flags flag 
 * @return indirizzo di memoria condivisa
 */
void *attach_segments(key_t key, size_t size);

/**
 * @brief elimina il segmento di memoria condivisa
 * @param key identificatore segmento
 * @param attached_segment indirizzo di memoria condivisa
 */
void detach_segments(key_t key, void *attached_segment);

/**
 * @brief carica il file di input
 * @param name nome file
 * @param segment indirizzo alla posizione di memoria del segmento condiviso
 */
struct Entry *load_file(char *name, void *ptr);

/**
 * @brief salva le chiavi sul file di output
 * @param name nome file
 * @param keys chiavi
 * @param n_of_lines numero di righe nel file di input
 */
void save_keys(char *name, unsigned *keys, int n_of_lines);

/**
 * @brief controlla che le chiavi siano corrette per tutta la lunghezza delle stringhe
 * @param input segmento di memoria che contiene il file di input
 * @param output segmento di memoria che contiene le chiavi calcolate
 * @param n_of_lines numero di righe del file di input/output
 * @return 0 se tutte le chiavi sono state verificate correttamente; altrimenti -1
 */
int check_keys(struct Entry *input, unsigned *output, int n_of_lines);

/// @}