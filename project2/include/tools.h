/// @file

/// @defgroup tools
/// @{

#ifndef TOOLS
#define TOOLS

#define STDOUT 1
#define STDERR 2

/**
 * @brief funzione di errore alternativa
 */
void syserr(char *prog, char *msg);

/**
 * @brief stampa in STDOUT e va a capo
 * @param str stringa
 */
void println(char *str);

/**
 * @brief stampa in STDERR
 * @param str stringa
 */
void printerr(char *str);


#endif

/// @}
