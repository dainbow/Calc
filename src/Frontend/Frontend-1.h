#pragma once

#include "TXLib.h"

#include "Grammar.h"
#include "../Tree/TreeDiskUtilities.h"

bool  isShowReversedTree = 0;

char* inputReversedName  = 0;
char* outputReversedName = 0;

const char TREE_OUTPUT_FORMAT[]     = ".notree";
const char ASM_REVERSED_FORMAT[]    = ".noreversed";
const char ASM_NAME[]               = "asm";
const char ASM_PATH[]               = "out/";
const char ASM_OUTPUT_FORMAT[]      = ".noass";

const int32_t MAX_FILE_NAME_LENGTH  = 100;

void ProcessFrontendminus1Arguments(int argc, char* argv[]);

void FrontendMinusOne(Tree* AST);
void PrintAST(Node* node, FILE* output);

int32_t GetOperationPriority(int8_t operation);

void PrintOperationMinus1(Node* node, FILE* output);
void PrintFunction(Node* node, FILE* output);
void PrintKeyword(Node* node, FILE* output);
void PrintString(Node* node, FILE* output);
void PrintArray(Node* node, FILE* output);

