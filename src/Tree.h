#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "Text.h"

typedef int32_t TreeElem;
#define TREE_TYPE "%s"

#define _ENABLE_PTR_ADRESSES_VIEW_ 0

#if (_ENABLE_PTR_ADRESSES_VIEW_ == 1)
    #define LEFT_BRANCH  "%lld"
    #define RIGHT_BRANCH "%lld"
    #define LEFT_BRANCH_VALUE  root->left  - pointerAnchor
    #define RIGHT_BRANCH_VALUE root->right - pointerAnchor
#else
    #define LEFT_BRANCH  "l%d"
    #define RIGHT_BRANCH "r%d"
    #define LEFT_BRANCH_VALUE  curNodeNumber
    #define RIGHT_BRANCH_VALUE curNodeNumber
#endif

enum NodeDataTypes {
    TYPE_UNKNOWN = 0,
    TYPE_CONST   = 1,
    TYPE_OP      = 2,
    TYPE_VAR     = 3,
    TYPE_UNO     = 4,
    TYPE_KEYWORD = 5,
    TYPE_FUNC    = 6,
    TYPE_STR     = 7,
};

enum KeyWords {
    KEY_IF = 1,
    KEY_ELSEIF,
    KEY_WHILE,
    KEY_IN,
    KEY_TO,
    KEY_END,
    KEY_LILEND,
    KEY_WITH,
    KEY_BEGIN,
    KEY_ELSE,
    KEY_FOR,
    KEY_FROM,
    KEY_RETURN,
    KEY_CRY,
    KEY_DOT,
    KEY_GOBBLE,
    KEY_DIFF,
};

enum Operators {
    COMMA_OP = ',',
    ADD_OP   = '+',
    SUB_OP   = '-',
    MUL_OP   = '*',
    DIV_OP   = '/',
    POW_OP   = '^',
    NON_OP   = '!',
    L_OP     = '<',
    LEQ_OP   = 120,
    G_OP     = '>',
    GEQ_OP,
    EQ_OP    = '=',
    DEQ_OP   = 100,
    NEQ_OP,
    LEFT_ROUND_OP  = '(',
    RIGHT_ROUND_OP = ')',
    LEFT_SQR_OP    = '[',
    RIGHT_SQR_OP   = ']',
    LEFT_CUR_OP    = '{',
    RIGHT_CUR_OP   = '}',
    EOL_OP         = '$',
};

union Data {
    double number;
    int8_t  operation;
    int8_t* expression;
};

struct Node {
    Data data;
    NodeDataTypes type; 

    uint32_t weight;
    Node* left;
    Node* right;
};

struct Tree {
    Node* root;

    Text qbase;

    int8_t* unsavedQuestions;
    int32_t bufLen;
};

const int32_t MAX_RECURSION_DEPTH = 1000;
const int32_t MAX_BUFFER_SIZE     = 8000;

const char G_STANDART_NAME[] = "graph";

const char OPEN_BRACKET  = '(';
const char CLOSE_BRACKET = ')';
const int8_t E_CONST[]       = "e";
const char SIN_OP        = 's';
const char COS_OP        = 'c';
const char LOG_OP        = 'l';

#define TreeCtor(tree)    \
    Tree tree;            \
    TreeCtor_(&tree)

#define _DEBUG_MODE_ 1

#define IsTreeOk(tree)                          \
    if (VerifyTree(tree)) {                     \
        MakeTreeGraph(tree);                    \
        assert(FAIL && "CHECK TREE DUMP");      \
    }

void TreeCtor_(Tree* tree);

void TreeDtor(Tree* tree);
void NodeDtor(Node* root);

Node* MakeNewNode(int8_t operation, double number, int8_t* expression, NodeDataTypes type, Node* left, Node* right);

void PrintTreeNodes(Tree* tree, Node* root, FILE* output = stdout);
bool VerifyTree(Node* root);
