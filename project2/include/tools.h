/// @file

/// @defgroup tools struttura file di supporto
/// @{

/**
 * @brief funzione di errore, stampa un messaggio personalizzato e l'errore di sistema
 */
void syserr(char *prog, char *msg);

/**
 * @brief stampa in STDOUT e va a capo
 * @param str stringa da stampare
 */
void println(char *str);

/**
 * @brief stampa in STDERR e va a capo
 * @param str stringa da stampare
 */
void printerr(char *str);

/**
 * @brief conta il numero di caratteri in una stringa
 * @param str puntatore ad un array di caratteri
 * @return numero di caratteri nella string adata (escluso \0)
 */
int strsize(char *str);

/**
 * @brief converte un intero in array di caratteri
 * @param n numero intero
 * @return numero convertito in array di char (escluso \0 finale)
 */
char *itoa(int n);

/**
 * @brief concatena due array di caratteri
 * @param str1 array di caratteri
 * @param str2 array di caratteri
 * @return concatenazione str1 + str2 (con \0 finale)
 */
char *strcct(char *str1, char *str2);

/**
 * @brief copia il secondo parametro sul primo parametro
 * @param str1 puntatore all'array di caratteri finale
 * @param str2 puntatore all'array di caratteri da copiare
 */
void strcp(char *str1, char *str2);

/**
 * @brief converte un numero intero senza segno in array di caratteri con il numero convertito in esadecimale
 * @param value numero senza segno
 * @return puntatore ad array di caratteri con il numero convertito in esadecimale
 */
char *utoh(unsigned value);

/// @}
