#include "./Backend/X86_Backend.h"
#include "./Backend/x86.h"

const uint32_t STROUT_SIZE  = 28;
const uint8_t STROUT_FUNC[] =  {0x80, 0x3e, 0x24,       // cmp [rsi], '$'
                                0x74, 0x16,             // je end
                                0xb8, 0x01, 0x00, 0x00, 0x00, // mov eax, 1
                                0xbf, 0x01, 0x00, 0x00, 0x00, // mov edi, 1
                                0xba, 0x01, 0x00, 0x00, 0x00, // mov edx, 1
                                0x0f, 0x05,                   // syscall
                                0x48, 0xff, 0xc6,             // inc rsi
                                0xeb, 0xe5,                   // jmp begin
                                0xc3                          // ret
                                };

const uint32_t OUT_SIZE  = 109;
const uint8_t OUT_FUNC[] = {0x48, 0x31, 0xc9,             // xor rcx, rcx
                            0x49, 0x89, 0xf2,             // mov r10, rsi
                            0x48, 0x83, 0xff, 0x00,       // cmp rdi, 0
                            0x7d, 0x09,                   // jge next
                            0xc6, 0x06, 0x2d,             // mov byte [rsi], '-'
                            0x48, 0xff, 0xc1,             // inc rcx
                            0x48, 0xf7, 0xdf,             // neg rdi
                            0x48, 0x89, 0xf8,             // mov rax, rdi
                            0x48, 0x31, 0xd2,             // xor rdx, rdx
                            0xbb, 0x0a, 0x00, 0x00, 0x00, // mov ebx, 0xa
                            0x48, 0x83, 0xf8, 0x00,       // cmp rax, 0
                            0x74, 0x0b,                   // je endlop
                            0x48, 0xff, 0xc1,             // inc rcx
                            0x48, 0xf7, 0xf3,             // div rbx
                            0x48, 0x31, 0xd2,             // xor rdx, rdx
                            0xeb, 0xef,                   // jmp comp
                            0x48, 0x89, 0xf8,             // mov rax, rdi
                            0x48, 0x83, 0xf8, 0x00,       // cmp rax, 0
                            0x75, 0x06,                   // jne next
                            0xc6, 0x06, 0x30,             // mov byte [rsi], '0'
                            0x48, 0xff, 0xc1,             // inc rcx
                            0x48, 0x01, 0xce,             // add rsi, rcx
                            0x48, 0x83, 0xf8, 0x00,       // cmp rax, 0
                            0x74, 0x11,                   // je endlop1
                            0x48, 0xf7, 0xf3,             // div rbx
                            0x48, 0x83, 0xc2, 0x30,       // add rdx, '0'
                            0x48, 0xff, 0xce,             // dec rsi
                            0x88, 0x16,                   // mov [rsi], dl
                            0x48, 0x31, 0xd2,             // xor rdx, rdx
                            0xeb, 0xe9,                   // jmp com1
                            0xb8, 0x01, 0x00, 0x00, 0x00, // mov eax, 1
                            0xbf, 0x01, 0x00, 0x00, 0x00, // mov edi, 1
                            0x4c, 0x89, 0xd6,             // mov rsi, r10
                            0x48, 0x89, 0xca,             // mov rdx, rcx
                            0x0f, 0x05,                   // syscall
                            0xc3};                        // ret  

const uint32_t NEWLINE_SIZE = 30;
const uint8_t NEWLINE_FUNC[]  = {0xbe, 0x00, 0x20, 0x40, 0x00, // mov rsi, 401000h
                              0x8a, 0x1e,                   // mov bl, [rsi]
                              0xc6, 0x06, 0x0a,             // mov byte [rsi], 10
                              0xb8, 0x01, 0x00, 0x00, 0x00, // mov eax, 1
                              0xbf, 0x01, 0x00, 0x00, 0x00, // mov edi, 1
                              0xba, 0x01, 0x00, 0x00, 0x00, // mov edx, 1
                              0x0f, 0x05,                   // syscall
                              0x88, 0x1e,                   // mov [rsi], bl
                              0xc3};                        // ret                 

const uint32_t HLT_SIZE  = 10;
const uint8_t HLT_FUNC[] = {0xb8, 0x3c, 0x00, 0x00, 0x00,   // mov eax, 0x3c
                            0x48, 0x31, 0xff,               // xor rdi, rdi
                            0x0f, 0x05};                    // syscall

const uint32_t IN_SIZE  = 80;
const uint8_t IN_FUNC[] = {0x49, 0x89, 0xf2,                // mov r10, rsi
                           0x48, 0xff, 0xce,                // dec rsi
                           0x48, 0xff, 0xc6,                // inc rsi
                           0x48, 0x31, 0xc0,                // xor rax, rax
                           0x48, 0x31, 0xff,                // xor rdi, rdi
                           0xba, 0x01, 0x00, 0x00, 0x00,    // mov edx, 1
                           0x0f, 0x05,                      // syscall
                           0x80, 0x3e, 0x0a,                // cmp byte [rsi], 0xa
                           0x75, 0xeb,                      // jne end
                           0x48, 0x31, 0xff,                // xor rdi, rdi
                           0xbb, 0x0a, 0x00, 0x00, 0x00,    // mov ebx, 0xa
                           0xb9, 0x01, 0x00, 0x00, 0x00,    // mov ecx, 1
                           0x48, 0xff, 0xce,                // dec rsi
                           0x8a, 0x06,                      // mov al, [rsi]
                           0x3c, 0x2d,                      // cmp al, '-'
                           0x75, 0x05,                      // jne next
                           0x48, 0xf7, 0xdf,                // neg rdi
                           0xeb, 0x16,                      // jmp end2
                           0x2c, 0x30,                      // sub al, '0'
                           0x48, 0xf7, 0xe1,                // mul rcx
                           0x48, 0x01, 0xc7,                // add rdi, rax
                           0x48, 0x89, 0xc8,                // mov rax, rcx
                           0x48, 0xf7, 0xe3,                // mul rbx
                           0x48, 0x89, 0xc1,                // mov rcx, rax
                           0x4c, 0x39, 0xd6,                // cmp rsi, r10
                           0x77, 0xdc,                      // JG end1
                           0x48, 0x89, 0xf8,                // mov rax, rdi
                           0xc3};                           // ret 

const uint32_t ADD_SIZE = 9;
const uint8_t ADD_X86[] = {0x41, 0x5a, // pop r10              
                           0x41, 0x59, // pop r9
                           0x4d, 0x01, 0xca, // add r10, r9 
                           0x41, 0x52       // push r10
                           };

const uint32_t SUB_SIZE = 9;
const uint8_t SUB_X86[] = {0x41, 0x59, // pop r9
                           0x41, 0x5a, // pop r10                
                           0x4d, 0x29, 0xca, // sub r10, r9 
                           0x41, 0x52        // push r10
                          };

const uint32_t DIV_SIZE = 12;
const uint8_t DIV_X86[] = {0x48, 0x31, 0xd2, // xor rdx, rdx
                           0x41, 0x59,       // pop r9
                           0x58,             // pop rax
                           0x48, 0x99,       // cqo
                           0x49, 0xf7, 0xf9, // idiv r9
                           0x50,             // push rax
                           };

const uint32_t MUL_SIZE = 12;
const uint8_t MUL_X86[] = {0x48, 0x31, 0xd2, // xor rdx, rdx
                           0x58,             // pop rax
                           0x41, 0x59,       // pop r9
                           0x48, 0x99,       // cqo
                           0x49, 0xf7, 0xe9, // imul r9
                           0x50,             // push rax
                           };

#define LABEL_FLAGS     {0, 1, 0, 0, 0}
#define NONE_FLAGS      {0, 0, 0, 0, 0}
#define STRING_FLAGS    {1, 0, 0, 0, 0}
#define TO_MEM_FLAGS    {0, 0, 1, 1, 1}
#define CONST_FLAGS     {0, 0, 1, 0, 0}
#define REGISTER_FLAGS  {0, 0, 0, 1, 0}

#define CALL(function)              X86_ProcessLabelCmd(CMD_call, function, -1, context)

#define JG(function, number)        X86_ProcessLabelCmd(CMD_ja, function, number, context)

#define JGE(function, number)       X86_ProcessLabelCmd(CMD_jae, function, number, context)

#define JL(function, number)        X86_ProcessLabelCmd(CMD_jb, function, number, context)

#define JLE(function, number)       X86_ProcessLabelCmd(CMD_jbe, function, number, context)

#define JE(function, number)        X86_ProcessLabelCmd(CMD_je, function, number, context)

#define JNE(function, number)       X86_ProcessLabelCmd(CMD_jne, function, number, context)

#define JUMP(function, number)      X86_ProcessLabelCmd(CMD_jump, function, number, context)

#define STROUT(function, number)    X86_ProcessLabelCmd(CMD_strout, function, number, context)
    
#define LABEL(labelName, number)    X86_MakeLabel(labelName, number, context)                          

#define HLT                                                                 \
    context->stLib |= X86_HLT_FLAG;                                         \
                                                                        \
    X86_NewArgument(LABEL_FLAG, 0, 0, "hlt", -1, nullptr, context);         \
    EmitX86LabelCmd(CMD_call, context);                                 \
    

#define RET                                                                     \
        *(context->result->bytesArray + context->result->bytesCount) = (int8_t)0xc3;    \
        context->result->bytesCount++;                                          \


#define POP_TO_DX  X86_ProcessRegisterCmd(CMD_pop, RDX, context)
#define PUSH_TO_DX X86_ProcessRegisterCmd(CMD_push, RDX, context)

#define SUB_RSP(number) EmitX86RegAndConstCmd(SUB_CMD, SUB_RSP_CONST, number, context)
#define ADD_RSP(number) EmitX86RegAndConstCmd(ADD_CMD, ADD_RSP_CONST, number, context)

#define PUSH_TO_BX X86_ProcessRegisterCmd(CMD_push, RBX, context)
#define POP_TO_BX  X86_ProcessRegisterCmd(CMD_pop, RBX, context)

#define PUSH_TO_AX X86_ProcessRegisterCmd(CMD_push, RAX, context)
#define POP_TO_AX  X86_ProcessRegisterCmd(CMD_pop, RAX, context)

#define PUSH_TO_RBP X86_ProcessRegisterCmd(CMD_push, RBP, context)
#define POP_TO_RBP  X86_ProcessRegisterCmd(CMD_pop, RBP, context)

#define PUSH_TO_RDI X86_ProcessRegisterCmd(CMD_push, RDI, context)
#define POP_TO_RDI  X86_ProcessRegisterCmd(CMD_pop, RDI, context)

#define PUSH_CONST(constant) X86_ProcessConstantCmd(CMD_push, constant, context)
#define OUT_CONST(constant)  X86_ProcessConstantCmd(CMD_out, constant, context)
#define MOV_RSI(constant)    X86_ProcessConstantCmd(MOV_RSI_CONST, constant, context)

#define PUSH_TO_MEM(offset)     X86_ProcessMemoryCmd(CMD_push, PUSH_MEM_RBP_MINUS_NUMBER, offset, context)
#define POP_TO_MEM(offset)      X86_ProcessMemoryCmd(CMD_pop, POP_MEM_RBP_MINUS_NUMBER, offset, context)
#define POP_TO_RSP_SUB(offset)  X86_ProcessMemoryCmd(CMD_pop, POP_MEM_RSP_MINUS_NUMBER, offset, context)
#define IN_TO_MEM(offset)       X86_ProcessMemoryCmd(CMD_in, RBX, offset, context)

#define MOV_RBP_RSP EmitX86RegToRegCmd(MOV, MOV_RBP, MOV_RSP, context)
#define MOV_RSP_RBP EmitX86RegToRegCmd(MOV, MOV_RSP, MOV_RBP, context)

#define STRING(strName)     X86_MakeString(strName, context)

#define SINGULAR_OP(number) EmitCommand(number, context->result)

#define ADD                                                                                 \
    memmove(context->result->bytesArray + context->result->bytesCount, ADD_X86, ADD_SIZE);  \
    context->result->bytesCount += ADD_SIZE;                                                
    

#define SUB                                                                                \
    memmove(context->result->bytesArray + context->result->bytesCount, SUB_X86, SUB_SIZE); \
    context->result->bytesCount += SUB_SIZE;                                                
    
#define MUL                                                                                     \
        memmove(context->result->bytesArray + context->result->bytesCount, MUL_X86, MUL_SIZE);  \
        context->result->bytesCount += MUL_SIZE;                                                
    

#define DIV                                                                                     \
        memmove(context->result->bytesArray + context->result->bytesCount, DIV_X86, DIV_SIZE);  \
        context->result->bytesCount += DIV_SIZE;                                                
    

#define POW SINGULAR_OP(52)
#define MKWND SINGULAR_OP(40)
#define DRWPC SINGULAR_OP(4)

void X86_ContextDtor(X86_CodegenContext* context) {
    assert(context != nullptr);

    free(context->arguments);
    
    if (!context->labels->isAllDataRead) {
        free(context->labels);
    }

    free(context->result->bytesArray);
    free(context->result);

    StackDtor(context->offsetStack);
    StackDtor(context->beginStack);
}

void X86_ContextCtor(X86_CodegenContext* context) {
    assert(context != nullptr);

    context->amounts = {};

    Labels* labels = (Labels*)calloc(1, sizeof(labels[0]));

    if (context->labels == nullptr) {
        context->labels         = labels;
        FillLabelsWithPoison(context->labels);
    }
    else {
        free(labels);
    }

    CompileResult* outputB  = (CompileResult*)calloc(1, sizeof(outputB[0]));
    outputB->bytesArray     = (int8_t*)calloc(MAX_OUTPUTFILE_LENGTH, sizeof(outputB->bytesArray[0]));
    context->result         = outputB;
    
    context->arguments      = (Arguments*)calloc(MAX_OUTPUTFILE_LENGTH, sizeof(context->arguments[0]));
    FillArguments(context->arguments);
}

void X86_GenerateCode(Tree* AST, char* outputName) {
    assert(AST != nullptr);
    char endName[X86_MAX_FILE_NAME_LENGTH] = "";

    if (outputName) {
        strcat(endName, outputName);
    }
    else {
        GenerateOutputName(X86_ASM_NAME, endName, X86_ASM_PATH, X86_ASM_OUTPUT_FORMAT);
    }

    FILE* output = fopen(endName, "w");

    X86_CodegenContext* context = (X86_CodegenContext*)calloc(1, sizeof(context[0]));

    StackCtor(offsetStack);
    StackCtor(beginStack);

    context->offsetStack = &offsetStack;
    context->beginStack  = &beginStack;
    X86_ContextCtor(context);
    
    CALL("main");
    // fprintf(output, "call main\n");

    HLT;
    // fprintf(output, "hlt\n");

    X86_ASTBypass(AST->root, output, context);
    
    X86_PrintStandartFuncs(context);
    X86_PrintEndOfProgram(output, context);
    
    context->labels->isAllDataRead = 1;
    X86_ContextDtor(context);

    StackCtor(offsetStack2);
    StackCtor(beginStack2);

    context->offsetStack = &offsetStack2;
    context->beginStack  = &beginStack2;

    X86_ContextCtor(context);

    CALL("main");
    // fprintf(output, "call main\n");

    HLT;
    // fprintf(output, "hlt\n");

    X86_ASTBypass(AST->root, output, context);

    X86_PrintStandartFuncs(context);
    X86_PrintEndOfProgram(output, context);
    // fclose(output);

    strcat(endName, X86_ASM_OUTPUT_FORMAT);
    FILE* outputd = fopen(endName, "wb");

    
    CreateElfHeader(outputd, context->result->executeCount, context->result->bytesCount - ENTRY_POINT);
    printf("I wrote %lu bytes from array\n", fwrite(context->result->bytesArray, 1, context->result->bytesCount, outputd));
    // fclose(outputd);

    context->labels->isAllDataRead = 0;
    X86_ContextDtor(context);

    X86_ChmodOutFile(endName);
}

void X86_ASTBypass(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    if (X86_SkipConk(AST, output, context))
        return;

    if (X86_ProcessFunction(AST, output, context))
        return;

    if (X86_ProcessKeyword(AST, output, context))
        return;
    
    X86_PushNode(AST, output, context);
}

bool X86_ProcessKeyword(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    if (AST->type == NodeDataTypes::TYPE_KEYWORD) {
        switch (AST->data.operation) {
            case KEY_GOBBLE:
                X86_ProcessGobble(AST, output, context);
                break;
            case KEY_CRY:
                context->isReturnNeeded = 1;

                X86_ProcessCry(AST, output, context);
                CALL("nline");

                context->isReturnNeeded = 0;

                break;
            case KEY_IF:
                X86_ProcessIf(AST, output, context);
                break;
            case KEY_FOR:
                X86_ProcessFor(AST, output, context);
                break;
            case KEY_WHILE:
                X86_ProcessWhile(AST, output, context);
                break;
            case KEY_RETURN:
                X86_ProcessReturn(AST, output, context);
                break;
            case KEY_IN:
                X86_ProcessIn(AST, output, context);
                break;
            case KEY_TO:
                X86_ProcessTo(AST, output, context);
                break;
            default:
                // fprintf(stderr, "UNKNOWN KEYWORD %d\n", AST->data.operation);
                abort();
                break;
        }

        return 1;
    }

    return 0;
}

void X86_ProcessWhile(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

    X86_PushNode(AST->left, output, context);

    PUSH_CONST(0);
    // fprintf(output, "push 0\n");

    JE("whileend", context->amounts.whileAmount);
    // fprintf(output, "je whileend%u\n", context->amounts.whileAmount);

    LABEL("while", context->amounts.whileAmount);
    // fprintf(output, "while%u:\n", context->amounts.whileAmount);

    X86_ASTBypass(AST->right->left, output, context);

    X86_PushNode(AST->left, output, context);

    PUSH_CONST(0);
    // fprintf(output, "push 0\n");

    JNE("while", context->amounts.whileAmount);
    // fprintf(output, "jne while%u\n", context->amounts.whileAmount);

    LABEL("whileend", context->amounts.whileAmount);
    // fprintf(output, "whileend%u:\n", context->amounts.whileAmount);

    context->amounts.whileAmount++;
    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void X86_ProcessFor(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);
    uint32_t curForNumber = context->amounts.forAmount++;

    uint32_t fromOffset = X86_MakeLocalVar(AST->left->left->data.expression, context);
    X86_PushNode(AST->left->right->left, output, context);

    
    POP_TO_MEM(X86_X86_CELL_SIZE * (fromOffset + 1));
    // fprintf(output, "pop {bx + %u}\n", X86_MEMORY_CELL_SIZE * fromOffset);

    
    PUSH_TO_MEM(X86_X86_CELL_SIZE * (fromOffset + 1));
    // fprintf(output, "push {bx + %u}\n", X86_MEMORY_CELL_SIZE * fromOffset);

    X86_PushNode(AST->left->right->right, output, context);

    JGE("forend", curForNumber);
    // fprintf(output, "JGe forend%u\n", curForNumber);
    
    LABEL("for", curForNumber);
    // fprintf(output, "for%u:\n", curForNumber);
    
    X86_ASTBypass(AST->right->left, output, context);

    PUSH_TO_MEM(X86_X86_CELL_SIZE * (fromOffset + 1));
    // fprintf(output, "push {bx + %u}\n", X86_MEMORY_CELL_SIZE*fromOffset);

    PUSH_CONST(1);
    // fprintf(output, "push 1\n");

    ADD;
    // fprintf(output, "add\n");

    
    POP_TO_MEM(X86_X86_CELL_SIZE * (fromOffset + 1));
    // fprintf(output, "pop {bx + %u}\n", X86_MEMORY_CELL_SIZE*fromOffset);

    
    PUSH_TO_MEM(X86_X86_CELL_SIZE * (fromOffset + 1));
    // fprintf(output, "push {bx + %u}\n", X86_MEMORY_CELL_SIZE*fromOffset);

    X86_PushNode(AST->left->right->right, output, context); 

    JL("for", curForNumber);
    // fprintf(output, "JL for%u\n", curForNumber);

    LABEL("forend", curForNumber);
    // fprintf(output, "forend%u:\n", curForNumber);
    
    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void X86_ProcessGobble(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = X86_MakeLocalVar(AST->left->data.expression, context);

    
    IN_TO_MEM((double)(X86_X86_CELL_SIZE * (varOffset + 1)));
    // fprintf(output, "in {bx + %u}\n", X86_MEMORY_CELL_SIZE * varOffset);

    if (AST->right != nullptr) {
        X86_ProcessGobble(AST->right, output, context);
    }
}

void X86_ProcessReturn(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    context->isReturnNeeded = 1;

    X86_ASTBypass(AST->left, output, context);
    context->functions[context->amounts.functionsAmount - 1].hasReturn = 1;

    context->isReturnNeeded = 0;

    // fprintf(output, "ret\n");
}

void X86_ProcessTo(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);
    uint32_t varOffset = 0;

    if (AST->type == NodeDataTypes::TYPE_VAR) {
        varOffset = X86_MakeLocalVar(AST->right->data.expression, context);
        X86_ASTBypass(AST->left, output, context);

        POP_TO_MEM(X86_X86_CELL_SIZE * varOffset);
        // fprintf(output, "pop {bx + %u}\n", X86_X86_CELL_SIZE * varOffset);
    }
}

void X86_ProcessIn(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);
    uint32_t varOffset = 0;

    if (AST->left->type == NodeDataTypes::TYPE_VAR) {
        context->isReturnNeeded = 1;

        varOffset = X86_MakeLocalVar(AST->left->data.expression, context);
        X86_ASTBypass(AST->right, output, context);

        context->isReturnNeeded = 0;

        printf("varOffset + 1 is %u\n", varOffset + 1);
        
        POP_TO_MEM(X86_X86_CELL_SIZE * (varOffset + 1));
        // fprintf(output, "pop {bx + %u}\n", X86_X86_CELL_SIZE * varOffset);
    }
}

void X86_ProcessIf(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);
    uint32_t ifAmount = context->amounts.ifAmount++;

    X86_PushNode(AST->left, output, context);
    
    PUSH_CONST(0);
    // fprintf(output, "push 0\n");

    JE("ifelse", ifAmount);
    // fprintf(output, "je ifelse%u\n", ifAmount);

    X86_ASTBypass(AST->right->left, output, context);

    JUMP("ifend", ifAmount);
    // fprintf(output, "jump ifend%u\n", ifAmount);

    LABEL("ifelse", ifAmount);
    // fprintf(output, "ifelse%u:\n", ifAmount);

    if (AST->right->right != nullptr) {
        X86_ASTBypass(AST->right->right, output, context);
    }

    LABEL("ifend", ifAmount);
    // fprintf(output, "ifend%u:\n", ifAmount);

    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void X86_ProcessCry(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    X86_ASTBypass(AST->left, output, context);
    if (AST->left->type != NodeDataTypes::TYPE_STR) {
        OUT_CONST(1);
        // fprintf(output, "out 1\n");
    }

    if (AST->right != nullptr) {
        X86_ProcessCry(AST->right, output, context);
    }
}

bool X86_ProcessFunction(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    if (AST->type == NodeDataTypes::TYPE_FUNC) {
        if (!context->ifInFunction) {
            StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);
            StackPush(context->beginStack, (StackElem)(int64_t)context->beginOffset);

            context->beginOffset = context->offset;

            LABEL((const char*)AST->data.expression, -1);
            // fprintf(output, "%s:\n", AST->data.expression);

            X86_MakeFunc(AST->data.expression, context);

            int32_t paramAmount = 0;
            if (AST->left != nullptr) {
                paramAmount += X86_ProcessFuncArguments(AST->left, output, context);
            }
            context->functions[context->amounts.functionsAmount - 1].paramAmount = paramAmount;
            
            context->ifInFunction = 1;

            X86_EnterFuncVisibilityZone(AST, context);

            X86_ASTBypass(AST->right, output, context);

            X86_ExitFuncVisibilityZone(AST, context);

            RET;
            // fprintf(output, "ret\n");

            context->ifInFunction = 0;

            context->offset     = (!context->offsetStack->size) ? 0 : (uint32_t)(int64_t)StackPop(context->offsetStack);
            context->beginOffset = (!context->beginStack->size) ? 0 : (uint32_t)(int64_t)StackPop(context->beginStack);
        }
        else {
            if ((AST->right != nullptr)        &&
                (AST->right->type == NodeDataTypes::TYPE_UNO) &&
                (AST->right->data.operation == '$')) {
                assert(0 && "DECLARATION OF FUNCTION IN FUNCTION BODY");
            }
            else {
                
                SUB_RSP(2 * X86_X86_CELL_SIZE);
                X86_ExecuteFunction(AST, output, context);
            }
        }

        return 1;
    }

    return 0;
}

int32_t X86_ExecuteFunction(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    int32_t paramAmount = 0;
    if (AST->left != nullptr) {
        X86_ASTBypass(AST->left, output, context);
        if (AST->left->type != NodeDataTypes::TYPE_STR)
            paramAmount++;
    }

    if (AST->right != nullptr) {
        paramAmount += X86_ExecuteFunction(AST->right, output, context);
    }

    if (AST->type == NodeDataTypes::TYPE_FUNC) {
        if (X86_GetFuncArgAmount(AST->data.expression, context) != paramAmount) {
            assert(0 && "INVALID AMOUNT OF ARGUMENTS");
        }
        
        
        ADD_RSP((2 + paramAmount) * X86_X86_CELL_SIZE);

        CALL((const char*)AST->data.expression);
        // fprintf(output, "call %s\n", AST->data.expression);

        if (context->isReturnNeeded)
            PUSH_TO_AX;
    }

    return paramAmount;
}

void X86_PushNode(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    if (AST->left != nullptr) {
        X86_ASTBypass(AST->left, output, context);
    }

    if (AST->right != nullptr) {
        X86_ASTBypass(AST->right, output, context);
    }

    if (AST->type == NodeDataTypes::TYPE_CONST) {
        PUSH_CONST(AST->data.number);
        // fprintf(output, "push %f\n", (double)AST->data.number);
    }

    if (AST->type == NodeDataTypes::TYPE_STR) {
        int32_t stringNum = 0;

        if ((stringNum = X86_FindString(AST->data.expression, context)) != -1) {
            STROUT("str", stringNum);
            // fprintf(output, "strout str%d\n", stringNum);
        }
        else {
            STROUT("str", context->amounts.strAmount);
            // fprintf(output, "strout str%u\n", context->amounts.strAmount);

            context->stringsArray[context->amounts.strAmount] = AST->data.expression;
            context->amounts.strAmount++;
        }
    }

    if (AST->type == NodeDataTypes::TYPE_VAR) {
        int32_t varOffset = X86_GetVarOffset(AST->data.expression, context);
        if (varOffset == -1)
            assert(0 && "UNKNOWN VAR");

        
        PUSH_TO_MEM(X86_X86_CELL_SIZE * (varOffset + 1));
        // fprintf(output, "push {bx + %u}\n", X86_X86_CELL_SIZE * varOffset);
    }

    if (AST->type == NodeDataTypes::TYPE_OP) {
        X86_PrintOperation(AST, output, context);
    }
}

void X86_PrintOperation(Node* node, FILE* output, X86_CodegenContext* context) {
    assert(node    != nullptr);
    assert(context != nullptr);

    switch (node->data.operation) {
    case ADD_OP:
        ADD;
        // fprintf(output, "add\n");
        break;
    case SUB_OP:
        SUB;
        // fprintf(output, "sub\n");
        break;
    case DIV_OP:
        DIV;
        // fprintf(output, "div\n");
        break;
    case MUL_OP:
        MUL;
        // fprintf(output, "mul\n");
        break;
    case POW_OP:
        POW;
        // fprintf(output, "pow\n");
        break;
    case L_OP:
        SUB;
        // fprintf(output, "sub\n");

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JL("compareg", context->amounts.compareAmount);
        // fprintf(output, "JL compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        // fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        // fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        // fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        // fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case LEQ_OP:
        PUSH_CONST(1);
        // fprintf(output, "push 1\n");

        ADD;
        // fprintf(output, "add\n");

        SUB;
        // fprintf(output, "sub\n");

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JL("compareg", context->amounts.compareAmount);
        // fprintf(output, "JL compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        // fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        // fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        // fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        // fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case G_OP:
        SUB;
        // fprintf(output, "sub\n");

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JG("compareg", context->amounts.compareAmount);
        // fprintf(output, "JG compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        // fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        // fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        // fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        // fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case GEQ_OP:
        PUSH_CONST(1);
        // fprintf(output, "push 1\n");

        SUB;
        // fprintf(output, "sub\n");

        SUB;
        // fprintf(output, "sub\n");

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JG("compareg", context->amounts.compareAmount);
        // fprintf(output, "JG compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        // fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        // fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        // fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        // fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case DEQ_OP:
        SUB;
        // fprintf(output, "sub\n");

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JE("compareg", context->amounts.compareAmount);
        // fprintf(output, "je compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        // fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        // fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        // fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        // fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case NEQ_OP:
        SUB;
        // fprintf(output, "sub\n");

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JNE("compareg", context->amounts.compareAmount);
        // fprintf(output, "jne compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        // fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        // fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        // fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        // fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        // fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    default:
        break;
    }
}

int32_t X86_GetFuncArgAmount(int8_t* name, X86_CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curFunc = 0; curFunc < context->amounts.functionsAmount; curFunc++) {
        if (!strcmp((const char*)context->functions[curFunc].name, (const char*)name)) {
            return context->functions[curFunc].paramAmount;
        }
    }

    assert(0 && "UNKNOWN FUNCTION");
    return -1;
}

int32_t X86_GetVarOffset(int8_t* name, X86_CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curVar = context->beginOffset; curVar < context->offset; curVar++) {
        if (!strcmp((const char*)context->variables[curVar].name, (const char*)name)) {
            return curVar - context->beginOffset;
        }
    }

    return -1;
}

int32_t X86_ProcessFuncArguments(Node* node, FILE* output, X86_CodegenContext* context) {
    assert(node    != nullptr);
    assert(context != nullptr);

    int32_t paramAmount = 0;

    if (node->left != nullptr) {
        X86_MakeLocalVar(node->left->data.expression, context);

        paramAmount++;
    }

    if (node->right != nullptr) {
        paramAmount += X86_ProcessFuncArguments(node->right, output, context);
    }

    return paramAmount;
}

void X86_MakeFunc(int8_t* name, X86_CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    X86_CheckFuncRepetitions(name, context);

    context->functions[context->amounts.functionsAmount].name      = name;

    context->amounts.functionsAmount++;
}

uint32_t X86_MakeLocalArr(int8_t* name, int32_t length, X86_CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);
    int32_t repState = X86_GetVarOffset(name, context);

    if (repState == -1) {
        context->variables[context->offset].name = name; 
        context->variables[context->offset].offset[context->recursionDepth] = context->offset;

        context->offset += length;
        
        return (context->offset - length); 
    }
    else {
        return (uint32_t)repState;
    }
}

uint32_t X86_MakeLocalVar(int8_t* name, X86_CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);
    int32_t repState = X86_GetVarOffset(name, context);

    if (repState == -1) {
        context->variables[context->offset].name = name; 
        context->variables[context->offset].offset[context->recursionDepth] = context->offset;

        context->offset++;

        if (context->labels->isAllDataRead == 0) {
            context->functions[context->amounts.functionsAmount - 1].variablesAmount++;
        }

        // printf("Returning %u in MakeLocalVar\n", context->offset - 1);/
        return (context->offset - 1); 
    }
    else {
        return (uint32_t)repState;
    }
}

void X86_CheckFuncRepetitions(int8_t* name, X86_CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curFunc = 0; curFunc < context->amounts.functionsAmount; curFunc++) {
        if (!strcmp((const char*)context->functions[curFunc].name, (const char*)name)) {
            assert(0 && "TWO VARS WITH EQUAL NAMES");
        }
    }
}

bool X86_SkipConk(Node* AST, FILE* output, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    if (((AST->type == NodeDataTypes::TYPE_UNO)       &&
         (AST->data.operation == EOL_OP)) ||
        ((AST->type == NodeDataTypes::TYPE_KEYWORD) &&
         (AST->data.operation == KEY_LILEND))) {
        if (AST->left != nullptr) {
            X86_ASTBypass(AST->left, output, context);
        }

        if (AST->right != nullptr) {
            X86_ASTBypass(AST->right, output, context);
        }

        return 1;
    }

    return 0;
}

void X86_EnterFuncVisibilityZone(Node* AST, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    PUSH_TO_RBP;
    MOV_RBP_RSP;

    printf("func idx is %u\n", context->amounts.functionsAmount - 1);
    printf("SUB RSP, %u\n", context->functions[context->amounts.functionsAmount - 1].variablesAmount * 8);
    SUB_RSP(context->functions[context->amounts.functionsAmount - 1].variablesAmount * 8);
}

void X86_ExitFuncVisibilityZone(Node* AST, X86_CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    if (context->functions[context->amounts.functionsAmount - 1].hasReturn == 0)
        PUSH_CONST(0);

    POP_TO_AX;

    MOV_RSP_RBP;
    POP_TO_RBP;    
}

int32_t X86_FindString(int8_t* name, X86_CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curStr = 0; curStr < context->amounts.strAmount; curStr++) {
        if (!strcmp((const char*)name, (const char*)context->stringsArray[curStr])) {
            return curStr;
        }
    }

    return -1;
}

void X86_PrintEndOfProgram(FILE* output, X86_CodegenContext* context) {
    assert(context != nullptr);

    for (uint32_t curStr = 0; curStr < context->amounts.strAmount; curStr++) {
        LABEL("str", curStr);
        // fprintf(output, "str%u:\n", curStr);

        STRING((const char*)context->stringsArray[curStr]);
        // fprintf(output, "db %s\n\n", context->stringsArray[curStr]);
    }
}

void X86_NewArgument(Flags flags, int32_t argConstant, int32_t reg, const char labelName[], int32_t labelNum, const char string[], X86_CodegenContext* context) {
    assert(context   != nullptr);

    context->arguments[context->amounts.argumentsAmount].argFlags   = flags;                              
    context->arguments[context->amounts.argumentsAmount].argConst   = argConstant;                          
    context->arguments[context->amounts.argumentsAmount].argReg     = reg;                                

    if (labelName != nullptr) {                                                                 
        if (labelNum != -1) {
            sprintf((char*)context->arguments[context->amounts.argumentsAmount].labelName,  "%s%d", labelName, labelNum);  
        } 
        else {
            sprintf((char*)context->arguments[context->amounts.argumentsAmount].labelName,  "%s", labelName); 
        }    
    }
    
    if (string != nullptr)                                                                                                
        sprintf((char*)context->arguments[context->amounts.argumentsAmount].stringName, "%s", string);              
                                                                                        
    context->amounts.argumentsAmount++;
}

void X86_ProcessLabelCmd(int8_t commandNum, const char labelName[], uint32_t labelNum, X86_CodegenContext* context) {
    assert(labelName != nullptr);

    X86_NewArgument(LABEL_FLAGS, 0, 0, labelName, labelNum, nullptr, context);

    EmitX86LabelCmd(commandNum, context);
}

void X86_MakeString(const char stringName[], X86_CodegenContext* context) {
    assert(stringName != nullptr);
    assert(context    != nullptr);

    X86_NewArgument(STRING_FLAGS, 0, 0, nullptr, 0, stringName, context);                                                           
    EmitArgs(36, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels);
}

void X86_MakeLabel(const char labelName[], int32_t labelNumber, X86_CodegenContext* context) {
    assert(context   != nullptr);
    assert(labelName != nullptr);

    X86_NewArgument(LABEL_FLAGS, 0, 0, labelName, labelNumber, (char*)nullptr, context);     
    EmitArgs(0, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels);
}

void X86_ProcessConstantCmd(int8_t cmdNum, double constant, X86_CodegenContext* context) {
    assert(context != nullptr);
    
    if (cmdNum != CMD_out) {
        printf("IN FUNCTION OUT BUFFER PLACE IS %x\n", (int32_t)constant);
        EmitX86ConstCmd(cmdNum, (int32_t)constant, context);
    }
    else {
        POP_TO_RDI;

        context->stLib |= X86_OUT_FLAG;                                         
                                                                        
        X86_NewArgument(LABEL_FLAG, 0, 0, "out", -1, nullptr, context);    
        EmitX86LabelCmd(CMD_call, context);
    }
}

void X86_ProcessRegisterCmd(int8_t cmdNum, int8_t regNum, X86_CodegenContext* context) {
    EmitX86RegCmd(cmdNum, regNum, context);
}

void X86_ProcessMemoryCmd(int8_t cmdNum, int8_t regNum, double offset, X86_CodegenContext* context) {
    assert(context != nullptr);

    if (cmdNum != CMD_in)
        EmitX86ToMemCmd(cmdNum, regNum, (int32_t)offset, context);
    else {
        context->stLib |= X86_IN_FLAG;                                         
                                                                        
        X86_NewArgument(LABEL_FLAG, 0, 0, "in", -1, nullptr, context);    
        EmitX86LabelCmd(CMD_call, context);

        PUSH_TO_AX;
        POP_TO_MEM(offset);
    }
}

void X86_PrintStandartFuncs(X86_CodegenContext* context) {
    if (context->stLib & X86_STROUT_FLAG) {
        context->labels->array[context->labels->curLbl].name = (int8_t*)calloc(7, sizeof(char));
        sprintf((char*)context->labels->array[context->labels->curLbl].name, "strout");

        context->labels->array[context->labels->curLbl].go   = context->result->bytesCount;
        context->labels->curLbl++;

        memmove(context->result->bytesArray + context->result->bytesCount, STROUT_FUNC, STROUT_SIZE);
        context->result->bytesCount += STROUT_SIZE;
    }

    if (context->stLib & X86_IN_FLAG) {
        context->labels->array[context->labels->curLbl].name = (int8_t*)calloc(3, sizeof(char));
        sprintf((char*)context->labels->array[context->labels->curLbl].name, "in");

        context->labels->array[context->labels->curLbl].go   = context->result->bytesCount;
        context->labels->curLbl++;

        MOV_RSI((double)(VIRTUAL_ENTRY_POINT + ENTRY_POINT + ((context->stLib & X86_OUT_FLAG) != 0) * X86_STRING_BUFFER_SIZE));

        memmove(context->result->bytesArray + context->result->bytesCount, IN_FUNC, IN_SIZE);
        context->result->bytesCount += IN_SIZE;
    }

    if (context->stLib & X86_HLT_FLAG) {
        context->labels->array[context->labels->curLbl].name = (int8_t*)calloc(4, sizeof(char));
        sprintf((char*)context->labels->array[context->labels->curLbl].name, "hlt");

        context->labels->array[context->labels->curLbl].go   = context->result->bytesCount;
        context->labels->curLbl++;

        memmove(context->result->bytesArray + context->result->bytesCount, HLT_FUNC, HLT_SIZE);
        context->result->bytesCount += HLT_SIZE;
    }

    if (context->stLib & X86_OUT_FLAG) {
        context->labels->array[context->labels->curLbl].name = (int8_t*)calloc(4, sizeof(char));
        sprintf((char*)context->labels->array[context->labels->curLbl].name, "out");

        context->labels->array[context->labels->curLbl].go   = context->result->bytesCount;
        context->labels->curLbl++;

        MOV_RSI((double)(VIRTUAL_ENTRY_POINT + ENTRY_POINT));
        memmove(context->result->bytesArray + context->result->bytesCount, OUT_FUNC, OUT_SIZE);
        context->result->bytesCount += OUT_SIZE;

        context->labels->array[context->labels->curLbl].name = (int8_t*)calloc(6, sizeof(char));
        sprintf((char*)context->labels->array[context->labels->curLbl].name, "nline");

        context->labels->array[context->labels->curLbl].go   = context->result->bytesCount;
        context->labels->curLbl++;
        memmove(context->result->bytesArray + context->result->bytesCount, NEWLINE_FUNC, NEWLINE_SIZE);
        context->result->bytesCount += NEWLINE_SIZE;        
    }

    context->result->executeCount = context->result->bytesCount;
    context->result->bytesCount   = ENTRY_POINT;

    if (context->stLib & X86_OUT_FLAG) {
        STRING("0000000000");
    }

    if (context->stLib & X86_IN_FLAG) {
        STRING("0000000000");
    }
}

void X86_ChmodOutFile(char* endName) {
    char command[MAX_COMMAND_NAME] = "";
    sprintf(command, "chmod +x %s", endName);

    system(command);
}
