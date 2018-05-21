/// @file
/// @defgroup padre
/// @{

#include <sys/shm.h>
#include <sys/ipc.h>

/**
 * @brief wrapper del processo padre
 * @param input descrittore file input
 * @param output descrittore file output
 * @return 0 is everything went fine, otherwise 1
 */
int padre(char *input, char *output);

/**
 * @brief crea il segmento di memoria condivisa
 * @param key identificatore segmento
 * @param size dimensione segmento
 * @param flags flag 
 * @return indirizzo di memoria condivisa
 */
void* attach_segments(key_t key, size_t size, int flags);

/**
 * @brief elimina il segmento di memoria condivisa
 * @param key identificatore segmento
 * @param attached_segment indirizzo di memoria condivisa
 */
void detach_segments(key_t key, char* attached_segment);

/**
 * @brief carica il file di input
 * @param name nome file
 */
void load_file(char *name, char* segment);

/**
 * @brief salva le chiavi sul file di output
 * @param name nome file
 * @param keys chiavi
 */
void save_keys(char *name, char *keys);

/**
 * @brief controlla che le chiavi siano corrette per tutta la lunghezza delle stringhe
 * @param keys chiavi
 */
void check_keys(char *keys);

/// @}