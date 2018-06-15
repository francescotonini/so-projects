/// @file
/// @defgroup constants costanti globali
/// @{

/**
 * @def STDOUT
 * Descrittore standard output
 */
#define STDOUT 1
/**
 * @def STDERR
 * descrittore standard error
 */
#define STDERR 2

#ifndef SHKEY_S1
/**
 * @def SHKEY_S1
 * Id della sezione di memoria condivisa s1
 */
#define SHKEY_S1 11111
#endif

#ifndef SHKEY_S2
/**
 * @def SHKEY_S2
 * Id della sezione di memoria condivisa s2
 */
#define SHKEY_S2 22222
#endif

#ifndef BUFFER_SIZE
/**
 * @def BUFFER_SIZE
 * Dimensione buffer di lettura/scrittura
 */
#define BUFFER_SIZE 512
#endif

#ifndef SEM_KEY
/**
 * @def SEM_KEY
 * id del semaforo
 */
#define SEM_KEY 33333
#endif

#ifndef QUEUE_KEY
/**
 * @def QUEUE_KEY
 * Id della coda
 */
#define QUEUE_KEY 44444
#endif

#ifdef THREADS
#ifndef NUM_THREADS
/**
 * @def NUM_THREADS
 * Numero di thread
 */
#define NUM_THREADS 4
#endif
#endif

#ifndef THREADS
#ifndef NUM_NEWPHEWS
/**
 * @def NUM_NEWPHEWS
 * Numero di nipoti
 */
#define NUM_NEWPHEWS 2
#endif
#endif

/// ID memoria condivisa S1
extern int s1_id;
/// ID memoria condivisa S2
extern int s2_id;
/// ID coda messaggi
extern int queue_id;
/// ID semaforo
extern int sem_id;

/// @}
