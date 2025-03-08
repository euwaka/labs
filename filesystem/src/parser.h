#ifndef PARSER_H
#define PARSER_H

#include "shell.h"

#include <stdbool.h>


typedef enum CmdTreeType {
    CMD_TREE_OPTR,
    CMD_TREE_EXEC,
    CMD_TREE_STREAM
} CmdTreeType;

typedef struct CmdTreeNode *CmdTree;
typedef struct CmdTreeNode {
    union {
        struct {
            Optr optr;
            CmdTree left;
            CmdTree right;
        };

        struct {
            Exec  *exec;
            Slice *argv;
        };
    };

    CmdTreeType type;
} CmdTreeNode;


#endif // PARSER_H
