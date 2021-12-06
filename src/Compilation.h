#pragma once

#include <stdlib.h>

#include "Text.h"
#include "Utilities.h"

struct CompileResult {
    size_t bytesCount;
    int8_t* bytesArray;
};

struct Command {
    const char* name;
    uint32_t length;
    uint32_t number;
};

struct Flags {
    uint32_t string   : 1;
    uint32_t label    : 1;
    uint32_t constant : 1;
    uint32_t reg      : 1;
    uint32_t mem      : 1;
};

typedef int32_t ProcStackElem;

struct Arguments {
    Flags argFlags;
    ProcStackElem argConst;
    int32_t argReg;
    char* labelName;
    char* stringName;
};

void FillLabelsWithPoison(Labels* labels);

void EmitCommand(int32_t cmdNum, CompileResult* output);
void EmitArgs(   int32_t cmdNum, CompileResult* output, Arguments* comArgs, Labels* labels);

const char* ShiftAndCheckArgs(String* string);

bool IfLabel(String* string, Labels* labels, size_t curCommandPointer);
ProcStackElem FindLabelByName(int8_t lblName[], Labels* labels);

bool PushArgsFilter(Flags argFlags);
bool DbArgsFilter(  Flags argFlags);
bool PopArgsFilter( Flags argFlags);
bool JumpArgsFilter(Flags argFlags);
bool OutArgsFilter (Flags argFlags);
bool NoArgsFilter  (Flags argFlags);

void FillArguments(Arguments* args);
