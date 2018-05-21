/// @file

/// @defgroup nipote
/// @{

#include <sys/types.h>

/**
 * @brief wrapper del processo padre
 * @param pid pid processo padre
 * @param lines numero righe nel file
 * @param uid id del nipote
 * @return 0 is everything went fine, otherwise 1
 */
int nipote(pid_t pid, int lines, int uid);

/**
 * @brief legge la stringa dal segmento S1
 * @return 0 se c'è una stringa da decodificare, altrimenti -1
 */
int load_string();

/*
 * @brief blocca l'accesso esclusivo regione critica
 */
void lock();

/*
 * @brief sblocca l'accesso esclusivo regione critica
 */
void unlock();

/*
 * @brief trova la chiave
 */
void find_key();

/*
 * @brief deposita il messaggio "chiave trovata/secondi" nella coda di messagi del processo logger
 */
void send_timeelapsed();

/*
 * @brief salva la chiave nel segmento S2
 */
void save_key();

/// @}