#pragma once

#include "../Tree/Tree.h"
#include "../Tree/Graph.h"

#include "../Tree/TreeDiskUtilities.h"
#include "Compilation.h"
#include "../Utilities/Stack.h"

const char ASM_NAME[] = "asm";
const char ASM_PATH[] = "out/";
const char ASM_OUTPUT_FORMAT[]      = ".ass";
const char TREE_OUTPUT_FORMAT[]     = ".notree";
const char ASM_REVERSED_FORMAT[]    = ".reversed";

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

void ContextCtor(CodegenContext* context);
void ContextDtor(CodegenContext* context);

void ProcessBackEndArgs(int argc, char* argv[]);

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
uint32_t MakeLocalArr(int8_t* name, int32_t length, CodegenContext* context);
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
void ProcessShow(Node* AST, FILE* output, CodegenContext* context);

int32_t FindString(int8_t* name, CodegenContext* context);

void PrintEndOfProgram(FILE* output, CodegenContext* context);

void NewArgument(Flags flags, int32_t argConstant, int32_t reg, const char labelName[], int32_t labelNum, const char string[], CodegenContext* context);
void MakeSomethingWithLabel(int8_t commandNum, const char labelName[], uint32_t labelNum, CodegenContext* context);
void MakeString(const char stringName[], CodegenContext* context);
void MakeLabel(const char labelName[], int32_t labelNumber, CodegenContext* context);

void DoWithConstant(int8_t cmdNum, double constant, CodegenContext* context);
void DoToRegister(int8_t cmdNum, int8_t regNum, CodegenContext* context);
void DoToMem(int8_t cmdNum, int8_t regNum, double offset, CodegenContext* context); 
