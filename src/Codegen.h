#pragma once

#include "Compilation.h"
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
const int32_t MAX_FILE_NAME_LENGTH         = 100;
const int32_t MAX_STRING_AMOUNT            = 100;

const uint32_t MEMORY_CELL_SIZE = 4;

struct LocalVar {
    int8_t*   name;
    uint32_t offset[MAX_FUNCTION_RECURSION_DEPTH];
};

struct Function {
    int8_t* name;
    int32_t paramAmount;
};

struct Amounts {
    uint32_t argumentsAmount;
    uint32_t functionsAmount;
    uint32_t powerAmount;
    uint32_t compareAmount;
    uint32_t ifAmount;
    uint32_t forAmount;
    uint32_t whileAmount;
    uint32_t strAmount;
};

struct CodegenContext {
    bool     ifInFunction;
    uint32_t offset;
    int32_t  recursionDepth;

    Amounts amounts;

    Stack*  offsetStack;
    Labels* labels;
    Arguments* arguments;
    CompileResult* result;

    int8_t* stringsArray[MAX_STRING_AMOUNT];
    Function functions[MAX_FUNCTIONS_AMOUNT];
    LocalVar variables[MAX_VARIABLES_AMOUNT];
};

void GenerateCode(Tree* AST);
void ASTBypass(Node* AST, FILE* output, CodegenContext* context);
bool SkipConk(Node* AST, FILE* output, CodegenContext* context);

void PushNode(Node* AST, FILE* output, CodegenContext* context);
void PrintOperation(Node* node, FILE* output, CodegenContext* context);

bool ProcessFunction(Node* AST, FILE* output, CodegenContext* context);
int32_t ExecuteFunction(Node* AST, FILE* output, CodegenContext* context);

void MakeFunc(int8_t* name, CodegenContext* context, int32_t argAmount);
int32_t GetFuncArgAmount(int8_t* name, CodegenContext* context);
int32_t ProcessFuncArguments(Node* node, FILE* output, CodegenContext* context);
void CheckFuncRepetitions(int8_t* name, CodegenContext* context);

void EnterFuncVisibilityZone(Node* AST, FILE* output, CodegenContext* context);
void ExitFuncVisibilityZone(Node* AST, FILE* output, CodegenContext* context);

uint32_t MakeLocalVar(int8_t* name, CodegenContext* context);
int32_t GetVarOffset(int8_t* name, CodegenContext* context);

bool ProcessKeyword(Node* AST, FILE* output, CodegenContext* context);

void ProcessWhile(Node* AST, FILE* output, CodegenContext* context);
void ProcessFor(Node* AST, FILE* output, CodegenContext* context);

void ProcessReturn(Node* AST, FILE* output, CodegenContext* context);

void ProcessTo(Node* AST, FILE* output, CodegenContext* context);
void ProcessIn(Node* AST, FILE* output, CodegenContext* context);

void ProcessIf(Node* AST, FILE* output, CodegenContext* context);

void ProcessCry(Node* AST, FILE* output, CodegenContext* context);
void ProcessGobble(Node* AST, FILE* output, CodegenContext* context);

int32_t FindString(int8_t* name, CodegenContext* context);

void PrintEndOfProgram(FILE* output, CodegenContext* context);
