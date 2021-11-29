#pragma once

#include "Graph.h"
#include "Stack.h"

struct DiffContext {
    Text*  mathPhrases;
    Text*  greekSymbols;
    Stack* bigStack;
};

const char TEX_PATH[]              = "texs\\";

const char TEX_OUTPUT_FORMAT[]     = ".pdf";
const char LOG_FORMAT[]            = ".log";
const char AUX_FORMAT[]            = ".aux";

const int32_t TEX_MAX_NAME_LENGTH  = 100;
const int32_t MAX_NODE_WEIGHT_DIFF = 10;

char* MakeLatexTitle(Tree* tree, char const* name, DiffContext* DiffContext);

void PrintTexTree(bool bigCheckFlag, Node* node, FILE* output, DiffContext* diffContext);
void PrintGreekCharacter(FILE* output, uint32_t index, Text* greekSymbols);
void PrintBigNodes(FILE* output, DiffContext* diffContext);

int32_t CompareOperations(int8_t firstOper, int8_t secondOper);
int32_t GiveOperationPriority(int8_t operation);
