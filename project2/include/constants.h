/// @file

/// @defgroup costanti costanti globali
/// @{

/**
 * @brief descrittore standard output
 */
#define STDOUT 1
/**
 * @brief descrittore standard error
 */
#define STDERR 2

#ifndef SHKEY_S1
/**
 * @brief id della sezione di memoria condivisa s1
 */
#define SHKEY_S1 11111
#endif

#ifndef SHKEY_S2
/**
 * @brief id della sezione di memoria condivisa s2
 */
#define SHKEY_S2 22222
#endif

#ifndef BUFFER_SIZE
/**
 * @brief dimensione buffer di lettura/scrittura
 */
#define BUFFER_SIZE 512
#endif

#ifndef SEM_KEY
/**
 * @brief id del semaforo
 */
#define SEM_KEY 33333
#endif

#ifndef QUEUE_KEY
/**
 * @brief id della coda
 */
#define QUEUE_KEY 44444
#endif

#ifdef THREADS
#ifndef NUM_THREADS
/**
 * @brief numero di thread
 */
#define NUM_THREADS 32
#endif
#endif

#ifndef THREADS
#ifndef NUM_NEWPHEWS
/**
 * @brief numero di nipoti
 */
#define NUM_NEWPHEWS 16
#endif
#endif

/// @}
