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
 * @return numero di caratteri nella stringa data (escluso \0)
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

/**
 * @brief controlla che la system call sia andata a buon fine, in caso contrario arresta il programma
 * @param retValue valore di ritorno system call
 * @param prog nome processo
 * @param message messaggio
 */
void try_or_exit(int retValue, char *prog, char *message);

/**
 * @brief rimuove tutte le risorse condivise se non sono state rimosse dai processi prima di uscire
 */
void clean_and_exit();

/**
 * @brief intercetta segnali di arresto e richiama clean_and_exit per pulire le risorse prima di uscire
 * @param sig_num tipo segnale
 */
void signal_error_handler(int sig_num);


/// @}
