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

/**
 * @brief struttura dati per passaggio informazioni tra figlio e nipote/i
 */
struct NephewData {
    /// @param id identificativo univoco del nipote (es. 1, 2, 3...)
    int id;
    /// @param lines numbero di righe del file
    int lines;
    /// @param s1 puntatore alla zona di memoria condivisa che contiene il file di input e la struttura Status
    void *s1;
    /// @param output puntatore alla zona di memoria condivisa che contiene le chiavi trovate
    unsigned *output;
};


#endif

/// @}
