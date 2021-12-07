#pragma once

#include "TXLib.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Differ.h"
#include "Text.h"
#include "Tree.h"
#include "Utilities.h"
#include "Graph.h"

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

void AnalyseText(Text* text, Tokens* tokens, Text* keywords);
int8_t IsKeyword(int8_t* buffer, Text* keywords);
