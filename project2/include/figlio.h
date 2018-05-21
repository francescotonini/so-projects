/// @file

/// @defgroup figlio
/// @{

/**
 * @brief wrapper del processo padre
 * @param lines numero di linee nel file
 * @return 0 is everything went fine, otherwise 1
 */
int figlio(int lines);

/**
 * @brief wrapper del processo padre
 * @return 0 is everything went fine, otherwise 1
 */
void status_updated(int sig_num);

/**
 * @brief deposita il messaggio di terminazione nella coda di messaggi del processo logger
 */
void send_terminate();

/// @}