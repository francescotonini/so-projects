/// @file

/// @defgroup figlio struttura processo figlio
/// @{

/**
 * @brief wrapper del processo figlio
 * @param lines numero di linee nel file
 * @param s1 indirizzo di memoria al segmento di memoria condivisa s1
 * @param output indirizzo di memoria al segmento di memoria condivisa s2
 */
void figlio(int lines, void *s1, unsigned *output);

/**
 * @brief intercetta i segnali ricevuti dal processo
 * @param sig_num tipo del segnale
 */
void status_updated(int sig_num);

/**
 * @brief deposita il messaggio di terminazione nella coda di messaggi del processo logger
 * @param queue_id identificativo coda
 * @param msg_size dimensione messaggio
 */
void send_terminate(int queue_id, int msg_size);

/// @}