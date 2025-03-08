#ifndef EXEC_H
#define EXEC_H

#include "lexer.h"

typedef enum Optr {
    OPTR_APPEND,
    OPTR_WRITE
} Optr;

/* The general form that an executable (such as mkdir and ls) takes. */
typedef bool Exec(Slice *argv);

#endif // EXEC_H