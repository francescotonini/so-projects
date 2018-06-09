/// @file

/// @defgroup nipote struttura processo nipote
/// @{

#include <sys/types.h>
#include <types.h>

/**
 * @brief wrapper del processo padre
 * @param ptr puntatore all'enumeratore NephewData
 */
void *nipote(void *ptr);

/**
 * @brief legge la stringa dal segmento S1
 * @return 0 se c'è una stringa da elaborare, altrimenti -1
 */
int load_string();

/**
 * @brief blocca l'accesso esclusivo alla regione critica del campo status
 * @param id indice semaforo
 */
void lock(int id);

/**
 * @brief sblocca l'accesso esclusivo alla regione critica del campo status
 * @param id indice semaforo
 */
void unlock(int id);

/**
 * @brief trova la chiave di cifratura
 * @param my_string indice della linea da elaborare
 * @param input puntatore alla sezione di memoria condivisa S1 che contiene il file di input
 * @return chiave trovata facendo lo XOR tra i primi 4 byte della stringa plain e cifrata
 */
unsigned find_key(int my_string, struct Entry *input);

/**
 * @brief deposita il messaggio "chiave trovata in ", seguito dal numero di secondi passato come parametro, nella coda di messagi del processo logger
 * @param time intervallo di tempo che sarà depositato, insieme a "chiave trovata in ", nella coda di messaggi del processo logger
 */
void send_timeelapsed(time_t time);

/**
 * @brief salva la chiave nel segmento S2
 * @param my_string indice della chiave, ovvero la "riga" del segmento di memoria condivisa S2 dove salvare la chiave
 * @param key chiave da salvare
 * @param output puntatore sezione memoria condivisa S2 dove verrà salvata la chiave passata come parametro
 */
void save_key(int my_string, unsigned key, unsigned *output);

/// @}