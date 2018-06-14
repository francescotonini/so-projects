/// @file

/// @defgroup nipote struttura processo nipote
/// @{

#include <sys/types.h>
#include <types.h>
#include <types.h>

/**
 * @brief wrapper del processo nipote
 * @param ptr puntatore all'enumeratore NephewData
 */
void *nipote(void *ptr);

/**
 * @brief legge la stringa dal segmento S1
 * @param id indice della stringa da leggere
 * @param input puntatore alla zona di memoria condivisa S1 che contiene il file di input
 * @return un puntatore alla struttura contenente i dati della riga da elaborare
 */
struct Entry *load_string(int id, struct Entry *input);

/**
 * @brief blocca l'accesso esclusivo alla regione critica del campo status
 */
void lock();

/**
 * @brief sblocca l'accesso esclusivo alla regione critica del campo status
 */
void unlock();

/**
 * @brief trova la chiave di cifratura
 * @param entry struttura che contiene la riga dal elaborare
 * @return chiave trovata facendo lo XOR tra i primi 4 byte della stringa plain e cifrata
 */
unsigned find_key(struct Entry *entry);

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