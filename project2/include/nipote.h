/// @file

/// @defgroup nipote struttura processo nipote
/// @{

#include <sys/types.h>
#include <types.h>

/**
 * @brief wrapper del processo padre
 * @param uid id del nipote
 * @param lines numero righe nel file
 * @param s1 indirizzo di memoria al segmento di memoria condivisa s1
 * @param output indirizzo di memoria al segmento di memoria condivisa s2
 */
void nipote(int uid, int lines, void *s1, unsigned *output);

/**
 * @brief legge la stringa dal segmento S1
 * @return 0 se c'è una stringa da decodificare, altrimenti -1
 */
int load_string();

/**
 * @brief blocca l'accesso esclusivo regione critica
 * @param id identificativo semaforo
 */
void lock(int id);

/**
 * @brief sblocca l'accesso esclusivo regione critica
 * @param id identificativo semaforo
 */
void unlock(int id);

/**
 * @brief trova la chiave
 * @return chiave trovata
 */
unsigned find_key(int my_string, struct Entry *input, unsigned *output);

/**
 * @brief deposita il messaggio "chiave trovata/secondi" nella coda di messagi del processo logger
 */
void send_timeelapsed();

/**
 * @brief salva la chiave nel segmento S2
 * @param key chiave
 * @param output puntatore sezione memoria condivisa
 */
void save_key(int my_string, unsigned key, unsigned *output);

/// @}