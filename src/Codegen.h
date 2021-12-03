#pragma once

#include "Tree.h"
#include "Graph.h"

const char ASM_NAME[] = "asm";
const char ASM_PATH[] = "out/";
const char ASM_OUTPUT_FORMAT[] = ".ass";

const int32_t VAR_BEGINNING                = 100;

const int32_t MAX_FUNCTION_RECURSION_DEPTH = 100;
const int32_t MAX_VARIABLES_AMOUNT         = 100;
const int32_t MAX_FUNCTIONS_AMOUNT         = 100;

struct LocalVar {
    int8_t*   name;
    int32_t offset[MAX_FUNCTION_RECURSION_DEPTH];
};

struct Function {
    int8_t* name;
    int32_t paramAmount;
};

struct CodegenContext {
    bool     ifInFunction;
    int32_t  offset;
    int32_t  recursionDepth;

    int32_t functionsAmount;
    int32_t powerAmount;
    int32_t compareAmount;

    Function functions[MAX_FUNCTIONS_AMOUNT];
    LocalVar variables[MAX_VARIABLES_AMOUNT];
};
