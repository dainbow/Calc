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
};

union Data {
    int8_t* expression;
    int8_t  operation;
    int32_t number;
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
const char SUB_OP        = '-';
const char ADD_OP        = '+';
const char MUL_OP        = '*';
const char POW_OP        = '^';
const char SIN_OP        = 's';
const char COS_OP        = 'c';
const char DIV_OP        = '/';
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

Node* MakeNewNode(int32_t number, int8_t* expression, NodeDataTypes type, Node* left, Node* right);

void PrintTreeNodes(Tree* tree, Node* root, FILE* output = stdout);
bool VerifyTree(Node* root);
