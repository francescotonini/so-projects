/// @file
/// @defgroup logger struttura processo logger
/// @{

/**
 * @brief wrapper del processo logger
 */
void logger();

/**
 * @brief scarica la coda di messaggi e la stampa su STDOUT
 * @return 0 se il logger deve continuare a stampare la coda, 1 se il logger va arrestato
 */
int polling_receive();

/// @}
