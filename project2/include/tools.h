/// @file

/// @defgroup tools struttura file di supporto
/// @{

#ifndef TOOLS
#define TOOLS

/**
 * @brief descrittore standard output
 */
#define STDOUT 1
/**
 * @brief descrittore standard error
 */
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

/**
 * @brief conta il nuovo di caratteri in una stringa
 * @param str puntatore a array di caratteri
 * @return numero di caratteri nella string adata
 */
int strsize(char *str);

/**
 * @brief converte un intero in array di caratteri
 * @param n numero intero
 * @return numero convertito in array di char
 */
char *itoa(int n);

/**
 * @brief concatena due array di caratteri
 * @param str1 array di caratteri
 * @param str2 array di caratteri
 * @return concatenazione str1 + str2
 */
char *strcct(char *str1, char *str2);

/**
 * @brief copia il secondo parametro sul primo parametro
 * @param str1 puntatore all'array di caratteri finale
 * @param str2 puntatore all'array di caratteri da copiare
 */
void strcp(char *str1, char *str2);

/**
 * @brief converte un numero senza codice in array di caratteri
 * @param value numero senza segno
 * @return puntatore ad array di caratteri
 */
char *utoh(unsigned value);

#endif

/// @}
