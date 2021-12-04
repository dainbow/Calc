#pragma once

#include "Tree.h"
#include "Graph.h"
#include "Stack.h"

const char ASM_NAME[] = "asm";
const char ASM_PATH[] = "out/";
const char ASM_OUTPUT_FORMAT[] = ".ass";

const int32_t VAR_BEGINNING                = 100;

const int32_t MAX_FUNCTION_RECURSION_DEPTH = 100;
const int32_t MAX_VARIABLES_AMOUNT         = 100;
const int32_t MAX_FUNCTIONS_AMOUNT         = 100;

struct LocalVar {
    int8_t*   name;
    uint32_t offset[MAX_FUNCTION_RECURSION_DEPTH];
};

struct Function {
    int8_t* name;
    int32_t paramAmount;
};

struct CodegenContext {
    bool     ifInFunction;
    uint32_t  offset;
    int32_t  recursionDepth;

    uint32_t functionsAmount;
    uint32_t powerAmount;
    uint32_t compareAmount;
    uint32_t ifAmount;

    Stack* offsetStack;
    Function functions[MAX_FUNCTIONS_AMOUNT];
    LocalVar variables[MAX_VARIABLES_AMOUNT];
};

void GenerateCode(Tree* AST);
void ASTBypass(Node* AST, FILE* output, CodegenContext* context);

void PushNode(Node* AST, FILE* output, CodegenContext* context);
void PrintOperation(Node* node, FILE* output, CodegenContext* context);

void ProcessFunction(Node* AST, FILE* output, CodegenContext* context);
int32_t ExecuteFunction(Node* AST, FILE* output, CodegenContext* context);

void MakeFunc(int8_t* name, CodegenContext* context, int32_t argAmount);
int32_t GetFuncArgAmount(int8_t* name, CodegenContext* context);
int32_t ProcessFuncArguments(Node* node, FILE* output, CodegenContext* context);
void CheckFuncRepetitions(int8_t* name, CodegenContext* context);

uint32_t MakeLocalVar(int8_t* name, CodegenContext* context);
bool CheckVarRepetitions(int8_t* name, CodegenContext* context);
