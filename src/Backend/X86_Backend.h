#pragma once

#include "../Tree/Tree.h"
#include "../Tree/Graph.h"

#include "../Tree/TreeDiskUtilities.h"
#include "Compilation.h"
#include "../Utilities/Stack.h"

const char X86_ASM_NAME[] = "asm";
const char X86_ASM_PATH[] = "out/";
const char X86_ASM_OUTPUT_FORMAT[]      = ".ass";
const char X86_TREE_OUTPUT_FORMAT[]     = ".notree";
const char X86_ASM_REVERSED_FORMAT[]    = ".reversed";

const int32_t X86_VAR_BEGINNING                = 100;

const int32_t X86_MAX_FUNCTION_RECURSION_DEPTH = 100;
const int32_t X86_MAX_VARIABLES_AMOUNT         = 100;
const int32_t X86_MAX_FUNCTIONS_AMOUNT         = 100;
const int32_t X86_MAX_FILE_NAME_LENGTH         = 100;
const int32_t X86_MAX_STRING_AMOUNT            = 100;

const int32_t X86_STRING_BUFFER_SIZE           = 13;

const uint32_t X86_MEMORY_CELL_SIZE = 4;
const uint32_t X86_X86_CELL_SIZE    = 8;

const uint8_t X86_STROUT_FLAG = 1;
const uint8_t X86_OUT_FLAG    = 2;
const uint8_t X86_IN_FLAG     = 4;
const uint8_t X86_HLT_FLAG    = 8;

struct X86_LocalVar {
    int8_t*   name;
    uint32_t offset[X86_MAX_FUNCTION_RECURSION_DEPTH];
};

struct X86_Function {
    int8_t* name;

    int32_t paramAmount;
    uint32_t variablesAmount;

    bool hasReturn;
};

struct X86_Amounts {
    uint32_t argumentsAmount;
    uint32_t functionsAmount;
    uint32_t powerAmount;
    uint32_t compareAmount;
    uint32_t ifAmount;
    uint32_t forAmount;
    uint32_t whileAmount;
    uint32_t strAmount;
};

struct X86_CodegenContext {
    bool     isReturnNeeded;
    bool     ifInFunction;
    
    uint32_t offset;
    uint32_t beginOffset;

    int32_t  recursionDepth;

    uint8_t stLib;
    int32_t stFuncs[4];

    X86_Amounts amounts;

    Stack*  offsetStack;
    Stack*  beginStack;

    Labels* labels;
    Arguments* arguments;
    CompileResult* result;

    int8_t* stringsArray[X86_MAX_STRING_AMOUNT];
    X86_Function functions[X86_MAX_FUNCTIONS_AMOUNT];
    X86_LocalVar variables[X86_MAX_VARIABLES_AMOUNT];
};

void X86_ContextCtor(X86_CodegenContext* context);
void X86_ContextDtor(X86_CodegenContext* context);

void X86_ProcessBackEndArgs(int argc, char* argv[]);

void X86_GenerateCode(Tree* AST, char* outputName);
void X86_ASTBypass(Node* AST, FILE* output, X86_CodegenContext* context);
bool X86_SkipConk(Node* AST, FILE* output, X86_CodegenContext* context);

void X86_PushNode(Node* AST, FILE* output, X86_CodegenContext* context);
void X86_PrintOperation(Node* node, FILE* output, X86_CodegenContext* context);

bool X86_ProcessFunction(Node* AST, FILE* output, X86_CodegenContext* context);
int32_t X86_ExecuteFunction(Node* AST, FILE* output, X86_CodegenContext* context);

void X86_MakeFunc(int8_t* name, X86_CodegenContext* context);
int32_t X86_GetFuncArgAmount(int8_t* name, X86_CodegenContext* context);
int32_t X86_ProcessFuncArguments(Node* node, FILE* output, X86_CodegenContext* context);
void X86_CheckFuncRepetitions(int8_t* name, X86_CodegenContext* context);

void X86_EnterFuncVisibilityZone(Node* AST, X86_CodegenContext* context);
void X86_ExitFuncVisibilityZone(Node* AST, X86_CodegenContext* context);

uint32_t X86_MakeLocalVar(int8_t* name, X86_CodegenContext* context);
uint32_t X86_MakeLocalArr(int8_t* name, int32_t length, X86_CodegenContext* context);
int32_t X86_GetVarOffset(int8_t* name, X86_CodegenContext* context);

bool X86_ProcessKeyword(Node* AST, FILE* output, X86_CodegenContext* context);

void X86_ProcessWhile(Node* AST, FILE* output, X86_CodegenContext* context);
void X86_ProcessFor(Node* AST, FILE* output, X86_CodegenContext* context);

void X86_ProcessReturn(Node* AST, FILE* output, X86_CodegenContext* context);

void X86_ProcessTo(Node* AST, FILE* output, X86_CodegenContext* context);
void X86_ProcessIn(Node* AST, FILE* output, X86_CodegenContext* context);

void X86_ProcessIf(Node* AST, FILE* output, X86_CodegenContext* context);

void X86_ProcessCry(Node* AST, FILE* output, X86_CodegenContext* context);
void X86_ProcessGobble(Node* AST, FILE* output, X86_CodegenContext* context);
void X86_ProcessShow(Node* AST, FILE* output, X86_CodegenContext* context);

int32_t X86_FindString(int8_t* name, X86_CodegenContext* context);

void X86_PrintEndOfProgram(FILE* output, X86_CodegenContext* context);
void X86_PrintStandartFuncs(X86_CodegenContext* context);

void X86_NewArgument(Flags flags, int32_t argConstant, int32_t reg, const char labelName[], int32_t labelNum, const char string[], X86_CodegenContext* context);
void X86_ProcessLabelCmd(int8_t commandNum, const char labelName[], uint32_t labelNum, X86_CodegenContext* context);
void X86_MakeString(const char stringName[], X86_CodegenContext* context);
void X86_MakeLabel(const char labelName[], int32_t labelNumber, X86_CodegenContext* context);

void X86_ProcessConstantCmd(int8_t cmdNum, double constant, X86_CodegenContext* context);
void X86_ProcessRegisterCmd(int8_t cmdNum, int8_t regNum, X86_CodegenContext* context);
void X86_ProcessMemoryCmd(int8_t cmdNum, int8_t regNum, double offset, X86_CodegenContext* context);

void X86_ChmodOutFile(char* endName);
