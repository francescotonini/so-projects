/// @file

/// @defgroup types enumeratori
/// @{

#ifndef TYPES
#define TYPES

/**
 * @brief struttura di controllo dei nipoti
 */
struct Status {
    /// @param grandson identificativo nipote
    int grandson;
    /// @param id_string identificativo posizione corrente file
    int id_string;
};

/**
 * @brief struttura dei messaggi inviati a logger
 */
struct Message {
    /// @param mtype tipo di messaggio
    long mtype;
    /// @param text testo
    char text[128];
};

/**
 * @brief rappresenta ogni riga del file di input
 */
struct Entry {
    /// @param clear parte chiara messaggio
    unsigned clear;
    /// @param encoded parte cifrata del messaggio
    unsigned encoded;
};

#endif

/// @}
