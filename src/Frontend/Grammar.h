#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

//#include "TXLib.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../Tree/Differ.h"
#include "../Utilities/Text.h"
#include "../Tree/Tree.h"
#include "../Utilities/Utilities.h"
#include "../Tree/Graph.h"

#define Require(smth)                                                   \
    if (smth) {                                                         \
        fprintf(stderr, "SyntaxError in %s function\n", __func__);      \
        abort();                                                        \
    }                                                                   \
                                                                        \
    (*pointer)++;               

struct Tokens {
    Node*   array;
    int8_t* database;  
};

const int32_t MAX_EXPRESSION_LENGTH = 200;

const char CALC_FILE[] = "expr.txt";
const char KEYS_FILE[] = "keys.txt";

void MakeAST(Tree* AST, Tokens* tokens);

Node* GetG(Node** pointer);
Node* GetExternal(Node** pointer);
Node* GetE(Node** pointer);
Node* GetT(Node** pointer);
Node* GetP(Node** pointer);

Node* GetN(Node** pointer);
Node* GetV(Node** pointer);
Node* GetStr(Node** pointer);
Node* GetD(Node** pointer);

Node* GetS(Node** pointer);
Node* GetF(Node** pointer);
Node* GetK(Node** pointer);
Node* GetDeg(Node** pointer);

void SkipSpaces(int8_t** pointer);
Node* SkipThings(Node** pointer);

void AnalyseText(Text* text, Tokens* tokens, Text* keywords);
int8_t IsKeyword(int8_t* buffer, Text* keywords);

#endif
