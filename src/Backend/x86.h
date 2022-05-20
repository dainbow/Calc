#ifndef _X86_H_
#define _X86_H_

#include "X86_Backend.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <elf.h>

#define LABEL_FLAG {0, 1, 0, 0, 0}

enum X86_MAGICS {
    PUSH_MEM_RBP_MINUS_NUMBER = 1,
    POP_MEM_RBP_MINUS_NUMBER  = 1,
    POP_MEM_RSP_MINUS_NUMBER  = 2,
    PUSH_CONST                = 1,
    MOV_RSI_CONST             = 2,
    SUB_CMD                   = 1,
    SUB_RSP_CONST             = 1,
    ADD_CMD                   = 2,
    ADD_RSP_CONST             = 1,
    MOV                       = 1,
    MOV_RBP                   = 1,
    MOV_RSP                   = 2,
};

enum X86_MY_REG_NUMBERS {
    RAX = 0,
    RBX = 1,
    RDX = 3,
    RBP = 4,
    RDI = 5,
};

const uint32_t ELF_HEADER_SIZE     = 64;
const uint32_t PROGRAM_HEADER_SIZE = 56;
const uint32_t ENTRY_POINT         = 0x1000;
const uint32_t VIRTUAL_ENTRY_POINT = 0x401000;

const uint32_t ONE_BYTE_JUMP_SIZE = 5;
const uint32_t TWO_BYTE_JUMP_SIZE = 6;

void CreateElfHeader(FILE* output, uint64_t byteCodeSize, uint64_t dataSize);

void EmitX86ToMemCmd(uint64_t cmdNum, int8_t regNum, int32_t number, X86_CodegenContext* context);
void EmitX86ConstCmd(int8_t cmdNum, int32_t value, X86_CodegenContext* context);
void EmitX86LabelCmd(uint8_t cmdNum, X86_CodegenContext* context);
void EmitX86RegCmd(int8_t cmdNum, int8_t regNum, X86_CodegenContext* context);
void EmitX86RegAndConstCmd(uint8_t cmdNum, int8_t regNum, int32_t cons, X86_CodegenContext* context);

void EmitX86RegToRegCmd(uint8_t cmdNum, int8_t reg1Num, int8_t reg2Num, X86_CodegenContext* context);

void Emit1ByteJump(uint8_t jumpType, uint64_t dest, X86_CodegenContext* context);
void Emit2ByteJump(uint16_t jumpType, uint64_t dest, X86_CodegenContext* context);

#endif
