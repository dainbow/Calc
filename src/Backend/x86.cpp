#include "x86.h"
#include "commands.h"

#define POP_R10 0x5a41
#define POP_R9  0x5941
#define CMP_R9_R10 0xca394d

#define JG  0x8f0f
#define JL  0x8c0f
#define JLE 0x8e0f
#define JE  0x840f
#define JNE 0x850f
#define JGE 0x8d0f

#define  JMP 0xe9
#define CALL 0xe8

void CreateElfHeader(FILE* output, uint64_t byteCodeSize, uint64_t dataSize) {
    fseek(output, 0, SEEK_SET);

    Elf64_Ehdr header = {};

    header.e_ident[0]   = 0x7f;
    header.e_ident[1]   = 0x45; // Взять из elf.h
    header.e_ident[2]   = 0x4c;
    header.e_ident[3]   = 0x46;
    header.e_ident[4]   = 0x02;
    header.e_ident[5]   = 0x01;
    header.e_ident[6]   = 0x01;

    header.e_type      = 0x02;
    header.e_machine   = 0x3e;
    header.e_version   = 0x01;
    header.e_entry     = 0x401000; // Взять константу ~НЕ из elf.h
    header.e_phoff     = 0x40;
    header.e_ehsize    = 0x40;
    header.e_phentsize = 0x38; 
    header.e_phnum     = 0x03;

    fwrite(&header, sizeof(header), 1, output);

    Elf64_Phdr firstProgramHeader = {};
    
    firstProgramHeader.p_type   = 0x01;
    firstProgramHeader.p_flags  = 0x04;
    firstProgramHeader.p_offset = 0x00;
    firstProgramHeader.p_vaddr  = 0x400000;
    firstProgramHeader.p_paddr  = 0x400000;
    firstProgramHeader.p_filesz = sizeof(Elf64_Ehdr) + 3 * sizeof(Elf64_Phdr);
    firstProgramHeader.p_memsz  = sizeof(Elf64_Ehdr) + 3 * sizeof(Elf64_Phdr);
    firstProgramHeader.p_align  = 0x1000;

    fwrite(&firstProgramHeader, sizeof(Elf64_Phdr), 1, output);

    Elf64_Phdr secondProgramHeader = {};
    
    secondProgramHeader.p_type   = 0x01;
    secondProgramHeader.p_flags  = 0x05; // Segment flags: read and execute
    secondProgramHeader.p_offset = 0x1000;
    secondProgramHeader.p_vaddr  = 0x401000;
    secondProgramHeader.p_paddr  = 0x401000;
    secondProgramHeader.p_filesz = byteCodeSize;
    secondProgramHeader.p_memsz  = byteCodeSize;
    secondProgramHeader.p_align  = 0x1000;

    fwrite(&secondProgramHeader, sizeof(Elf64_Phdr), 1, output);

    Elf64_Phdr thirdProgramHeader = {};
    
    thirdProgramHeader.p_type   = 0x01;
    thirdProgramHeader.p_flags  = 0x06; // Segment flags: read and execute
    thirdProgramHeader.p_offset = 0x2000;
    thirdProgramHeader.p_vaddr  = 0x402000;
    thirdProgramHeader.p_paddr  = 0x402000;
    thirdProgramHeader.p_filesz = dataSize;
    thirdProgramHeader.p_memsz  = dataSize;
    thirdProgramHeader.p_align  = 0x1000;

    fwrite(&thirdProgramHeader, sizeof(Elf64_Phdr), 1, output);
  
    fseek(output, ENTRY_POINT, SEEK_SET);
}

void EmitX86ToMemCmd(uint64_t cmdNum, int8_t regNum, int32_t number, X86_CodegenContext* context) {
    assert(context);

    uint8_t comandSize = 2;

    switch (cmdNum) {
        case CMD_push:
            switch (regNum) {
                case PUSH_MEM_RBP_MINUS_NUMBER:
                    cmdNum = 0xb5ff;
                    break;
                default:
                    assert(0 && "UNKNOWN REGISTER TO PUSH REG - X");
                    break;
            }
            break;
        case CMD_pop:
            switch (regNum) {
                case POP_MEM_RBP_MINUS_NUMBER:
                    cmdNum = 0x858f; 
                    break;
                case POP_MEM_RSP_MINUS_NUMBER:
                    cmdNum = 0x24848f; 
                    comandSize++;
                    break;
                default:
                    assert(0 && "UNKNOWN REGISTER TO PUSH REG - X");
                    break;
            }
            break;
        default:
            break;
    }

    *((uint64_t*)(context->result->bytesArray + context->result->bytesCount)) = cmdNum;
    context->result->bytesCount += comandSize;

    *((uint32_t*)(context->result->bytesArray + context->result->bytesCount)) = -number;
    context->result->bytesCount += 4;
}

void Emit1ByteJump(uint8_t jumpType, uint64_t dest, X86_CodegenContext* context) {
    dest = dest - (context->result->bytesCount + ONE_BYTE_JUMP_SIZE);
    
    *(context->result->bytesArray + context->result->bytesCount) = jumpType;
    context->result->bytesCount++;
    
    *(((int32_t*)(context->result->bytesArray + context->result->bytesCount))) = (int32_t)dest;
    context->result->bytesCount += 4;
}

void Emit2ByteJump(uint16_t jumpType, uint64_t dest, X86_CodegenContext* context) {
    *(uint16_t*)(context->result->bytesArray + context->result->bytesCount) = POP_R9;
    context->result->bytesCount += 2;

    *(uint16_t*)(context->result->bytesArray + context->result->bytesCount) = POP_R10;
    context->result->bytesCount += 2;

    *(uint32_t*)(context->result->bytesArray + context->result->bytesCount) = CMP_R9_R10;
    context->result->bytesCount += 3;
    
    dest = dest - (context->result->bytesCount + TWO_BYTE_JUMP_SIZE);
    
    *((uint16_t*)(context->result->bytesArray + context->result->bytesCount)) = jumpType;
    context->result->bytesCount += 2;
    
    *(((int32_t*)(context->result->bytesArray + context->result->bytesCount))) = (int32_t)dest;
    context->result->bytesCount += 4;
}

void EmitX86RegCmd(int8_t cmdNum, int8_t regNum, X86_CodegenContext* context) {
    assert(context);

    uint16_t newCmd = 0;

    switch (cmdNum)
    {
    case CMD_push:
        switch (regNum)
        {
        case RAX: // PUSH RAX
            newCmd = 0x50;
            
            break;
        case RBX: // PUSH RBX
            newCmd = 0x53;
            
            break;
        case RDX: // PUSH RDX
            newCmd = 0x52;
            
            break;
        case RBP: { // PUSH RBP
            newCmd = 0x55;
            
            break;
        }
        case RDI: { // PUSH RDI
            newCmd = 0x57;
            
            break;
        }
        
        default:
            assert(0 && "INVALID REGISTER TO PUSH");
            break;
        }

        break;
    case CMD_pop:
        switch (regNum) {
            case RAX: // POP RAX
                newCmd = 0x58;
                
                break;
            case RBX: // POP RBX
                newCmd = 0x5b;
                
                break;
            case RDX: // POP RDX
                newCmd = 0x5a;
                
                break;
            case RBP: { //POP RBP
                newCmd = 0x5d;
                
                break;
            }
            case RDI: { // POP RDI
                newCmd = 0x5f;
                
                break;
            }
            
            default:
                assert(0 && "INVALID REGISTER TO POP");
                break;
        }

        break;
    
    default:
        assert(0 && "INVALID REGISTER OPERATOT");
        break;
    }

    *(context->result->bytesArray + context->result->bytesCount) = (int8_t)newCmd;
    context->result->bytesCount++;
}

void EmitX86ConstCmd(int8_t cmdNum, int32_t value, X86_CodegenContext* context) {
    assert(context);

    uint8_t newCmd = 0;

    switch (cmdNum) {
        case PUSH_CONST:
            newCmd = 0x68; // PUSH CONST
            
            break;
        case MOV_RSI_CONST:
            newCmd = 0xbe; // MOV RSI, CONST
            break;

        default:
            assert(0 && "UNKNOWN INSTRUCTION WITH CONSTANTS");
            break;
    }

    *(context->result->bytesArray + context->result->bytesCount) = newCmd;
    context->result->bytesCount++;

    *((int32_t*)(context->result->bytesArray + context->result->bytesCount)) = value;
    context->result->bytesCount += 4;
}

void EmitX86RegAndConstCmd(uint8_t cmdNum, int8_t regNum, int32_t cons, X86_CodegenContext* context) {
    assert(context);

    switch (cmdNum) {
        case SUB_CMD:
            switch (regNum) {
            case SUB_RSP_CONST: 
                *((uint32_t*)(context->result->bytesArray + context->result->bytesCount)) = 0xec8148; 
                context->result->bytesCount += 3;

                *((int32_t*)(context->result->bytesArray + context->result->bytesCount)) = cons;
                context->result->bytesCount += 4;

                break;
            
            default:
                assert(0 && "INVALID SUB REGISTER");

                break;
            }
            break;
        case ADD_CMD:
            switch (regNum) {
                case ADD_RSP_CONST: // RSP
                    *((uint32_t*)(context->result->bytesArray + context->result->bytesCount)) = 0xc48148; //ADD RSP
                    context->result->bytesCount += 3;

                    *((int32_t*)(context->result->bytesArray + context->result->bytesCount)) = cons;
                    context->result->bytesCount += 4;

                    break;
                
                default:
                    assert(0 && "INVALID SUB REGISTER");

                    break;
                }
            break;
    
    default:
        assert(0 && "INVALID INSTRUCTION");

        break;
    }
}

void EmitX86RegToRegCmd(uint8_t cmdNum, int8_t reg1Num, int8_t reg2Num, X86_CodegenContext* context) {
    assert(context);

    switch (cmdNum) {
    case MOV:
        if ((reg1Num == MOV_RBP) && (reg2Num == MOV_RSP)) { // MOV RBP, RSP
            *((uint32_t*)(context->result->bytesArray + context->result->bytesCount)) = 0xe58948;
            context->result->bytesCount += 3;
        }
        else if ((reg1Num == MOV_RSP) && (reg2Num == MOV_RBP)) { // MOV RSP, RBP
            *((uint32_t*)(context->result->bytesArray + context->result->bytesCount)) = 0xec8948;
            context->result->bytesCount += 3;
        }
        break;
    
    default:
        break;
    }
}

void EmitX86LabelCmd(uint8_t cmdNum, X86_CodegenContext* context) {
    assert(context);

    uint32_t labelDest = 0;

    if (context->labels->isAllDataRead == 1) {
        labelDest = FindLabelByName((int8_t*)(context->arguments + context->amounts.argumentsAmount - 1)->labelName, context->labels);
        printf("FOUND LABEL %s with go to %x\n", (int8_t*)(context->arguments + context->amounts.argumentsAmount - 1)->labelName, labelDest);
    }

    switch (cmdNum) {
        case CMD_ja:
            Emit2ByteJump(JG, labelDest, context);
            break;
        case CMD_call:
            Emit1ByteJump(CALL, labelDest, context);
            break; 
        case CMD_jae:
            Emit2ByteJump(JGE, labelDest, context);
            break;
        case CMD_jb:
            Emit2ByteJump(JL, labelDest, context);
            break;
        case CMD_jbe:
            Emit2ByteJump(JLE, labelDest, context);
            break;
        case CMD_je:
            Emit2ByteJump(JE, labelDest, context);
            break;
        case CMD_jne:
            Emit2ByteJump(JNE, labelDest, context);
            break;
        case CMD_jump:
            Emit1ByteJump(JMP, labelDest, context);
            break;
        case CMD_strout:
            X86_ProcessConstantCmd(MOV_RSI_CONST, VIRTUAL_ENTRY_POINT + labelDest + 2, context);

            context->stLib |= X86_STROUT_FLAG;

            X86_NewArgument(LABEL_FLAG, 0, 0, "strout", -1, nullptr, context);

            labelDest = FindLabelByName((int8_t*)(context->arguments + context->amounts.argumentsAmount - 1)->labelName, context->labels);
            Emit1ByteJump(CALL, labelDest, context);
            break;
        default:
            assert(0 && "INVALID JUMP OPERATOR");
            break;
    }
}
