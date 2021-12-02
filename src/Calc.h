#pragma once

#include "TXLib.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
const int32_t FAIL = 0;

const char CALC_FILE[] = "expr.txt";
const char KEYS_FILE[] = "keys.txt";

Node* GetG(Node** pointer);
Node* GetE(Node** pointer);
Node* GetT(Node** pointer);
Node* GetP(Node** pointer);

Node* GetN(Node** pointer);
Node* GetV(Node** pointer);
Node* GetS(Node** pointer);

void SkipSpaces(int8_t** pointer);

void AnalyseText(Text* text, Tokens* tokens, Text* keywords);
int8_t IsKeyword(int8_t* buffer, Text* keywords);
