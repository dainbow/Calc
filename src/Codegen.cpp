#include "Codegen.h"

#define LABEL_FLAGS     {0, 1, 0, 0, 0}
#define NONE_FLAGS      {0, 0, 0, 0, 0}
#define STRING_FLAGS    {1, 0, 0, 0, 0}
#define TO_MEM_FLAGS    {0, 0, 1, 1, 1}
#define CONST_FLAGS     {0, 0, 1, 0, 0}
#define REGISTER_FLAGS  {0, 0, 0, 1, 0}

#define NEW_ARG(flags, constanta, reg, labelFormat, string, ...)                \
    context->arguments[context->amounts.argumentsAmount].argFlags   = flags;                              \
    context->arguments[context->amounts.argumentsAmount].argConst   = constanta;                          \
    context->arguments[context->amounts.argumentsAmount].argReg     = reg;                                \
                                                                                \
    sprintf((char*)context->arguments[context->amounts.argumentsAmount].labelName,  labelFormat __VA_ARGS__);       \
    if (string != nullptr)                                                                                                \
        sprintf((char*)context->arguments[context->amounts.argumentsAmount].stringName, "%s", string);              \
                                                                                        \
    context->amounts.argumentsAmount++;


#define SMTH_WITH_LABEL(number, function, ...)                                 \
    EmitCommand(number, context->result);                       \
    NEW_ARG(LABEL_FLAGS, 0, 0, function, (char*)nullptr, __VA_ARGS__);   \
    EmitArgs(number, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)


#define CALL(function, ...) SMTH_WITH_LABEL(41, function, __VA_ARGS__)

#define JA(function, ...) SMTH_WITH_LABEL(17, function, __VA_ARGS__)

#define JAE(function, ...) SMTH_WITH_LABEL(21, function, __VA_ARGS__)

#define JB(function, ...) SMTH_WITH_LABEL(25, function, __VA_ARGS__)

#define JBE(function, ...) SMTH_WITH_LABEL(29, function, __VA_ARGS__)

#define JE(function, ...) SMTH_WITH_LABEL(33, function, __VA_ARGS__)

#define JNE(function, ...) SMTH_WITH_LABEL(37, function, __VA_ARGS__)

#define JUMP(function, ...) SMTH_WITH_LABEL(9, function, __VA_ARGS__)

#define STROUT(function, ...) SMTH_WITH_LABEL(49, function, __VA_ARGS__)

#define STRING(string, ...) \
    NEW_ARG(STRING_FLAGS, 0, 0, "null", string, __VA_ARGS__);                                                           \
    EmitArgs(36, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)    

#define LABEL(labelFormat, ...)                                 \
    NEW_ARG(LABEL_FLAGS, 0, 0, labelFormat, (char*)nullptr, __VA_ARGS__);     \
    EmitArgs(0, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)

#define HLT                                  \
    EmitCommand(0, context->result)   

#define RET             \
    EmitCommand(44, context->result)    

#define PUSH_CONST(consta)                                                                                      \
    EmitCommand(1, context->result);                                                                            \
    NEW_ARG(CONST_FLAGS, (ProcStackElem)(consta * ACCURACY), 0, "null", (char*)nullptr);                                \
    EmitArgs(1, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)

#define PUSH_REG                                                                                                \
    EmitCommand(1, context->result);                                                                            \
    NEW_ARG(REGISTER_FLAGS, 0, 1, "null", (char*)nullptr);                                                              \
    EmitArgs(1, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)


#define OUT_CONST(consta)                                                                                      \
    EmitCommand(57, context->result);                                                                            \
    NEW_ARG(CONST_FLAGS, (ProcStackElem)(consta * ACCURACY), 0, "null", (char*)nullptr);                                          \
    EmitArgs(57, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)

#define PUSH_TO_MEM(offset) \
    EmitCommand(1, context->result);                                                                            \
    NEW_ARG(TO_MEM_FLAGS, (ProcStackElem)(offset * ACCURACY), 1, "null", (char*)nullptr);                                          \
    EmitArgs(1, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)

#define POP_TO_MEM(offset) \
    EmitCommand(5, context->result);                                                                            \
    NEW_ARG(TO_MEM_FLAGS, (ProcStackElem)(offset * ACCURACY), 1, "null", (char*)nullptr);                                          \
    EmitArgs(5, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)

#define POP_TO_REG \
    EmitCommand(5, context->result);                                                                            \
    NEW_ARG(REGISTER_FLAGS, 0, 1, "null", (char*)nullptr);                                          \
    EmitArgs(5, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)

#define IN_TO_MEM(offset) \
    EmitCommand(13, context->result);                                                                            \
    NEW_ARG(TO_MEM_FLAGS, (ProcStackElem)(offset * ACCURACY), 1, "null", (char*)nullptr);                                          \
    EmitArgs(13, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels)

#define SINGULAR_OP(number) \
    EmitCommand(number, context->result)

#define ADD SINGULAR_OP(8)
#define SUB SINGULAR_OP(12)
#define MUL SINGULAR_OP(16)
#define DIV SINGULAR_OP(20)
#define POW SINGULAR_OP(52)


void GenerateCode(Tree* AST) {
    assert(AST != nullptr);

    StackCtor(offsetStack);
    char endName[MAX_FILE_NAME_LENGTH] = "";

    GenerateOutputName(ASM_NAME, endName, ASM_PATH, ASM_OUTPUT_FORMAT);
    FILE* output = fopen(endName, "w");

    Labels labels = {};
    FillLabelsWithPoison(&labels);

    CompileResult outputB = {};
    outputB.bytesArray    = (int8_t*)calloc(MAX_OUTPUTFILE_LENGTH, sizeof(outputB.bytesArray[0]));

    CodegenContext* context = (CodegenContext*)calloc(1, sizeof(context[0]));
    context->offsetStack    = &offsetStack;

    context->arguments = {};

    context->result         = &outputB;
    context->labels         = &labels;

    context->arguments      = (Arguments*)calloc(MAX_OUTPUTFILE_LENGTH, sizeof(context->arguments[0]));
    FillArguments(context->arguments);

    CALL("main");
    fprintf(output, "call main\n");
    HLT;
    fprintf(output, "hlt\n");
    ASTBypass(AST->root, output, context);
    PrintEndOfProgram(output, context);

    free(context->arguments);
    StackDtor(&offsetStack);
    free(context);

    StackCtor(offsetStack2);
    context = (CodegenContext*)calloc(1, sizeof(context[0]));

    context->arguments      = (Arguments*)calloc(MAX_OUTPUTFILE_LENGTH, sizeof(context->arguments[0]));
    FillArguments(context->arguments);

    context->offsetStack = &offsetStack2;
    context->labels = &labels;

    free(outputB.bytesArray);
    outputB = {};
    outputB.bytesArray      = (int8_t*)calloc(MAX_OUTPUTFILE_LENGTH, sizeof(outputB.bytesArray[0]));
    context->result         = &outputB;

    context->labels->isAllDataRead = 1;
    CALL("main");
    fprintf(output, "call main\n");
    HLT;
    fprintf(output, "hlt\n");
    ASTBypass(AST->root, output, context);
    PrintEndOfProgram(output, context);
    fclose(output);

    strcat(endName, ASM_OUTPUT_FORMAT);
    FILE* outputd = fopen(endName, "wb");
    printf("I wrote %llu signature bytes\n",  fwrite(SIGNATURE, 1, SIGNATURE_SIZE, outputd));
    printf("I wrote %llu bytes from array\n", fwrite(context->result->bytesArray, 1, context->result->bytesCount, outputd));
    fclose(outputd);

    StackDtor(&offsetStack2);
    
}

void ASTBypass(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if (SkipConk(AST, output, context))
        return;

    if (ProcessFunction(AST, output, context))
        return;

    if (ProcessKeyword(AST, output, context))
        return;
    
    PushNode(AST, output, context);
}

bool ProcessKeyword(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if (AST->type == TYPE_KEYWORD) {
        switch (AST->data.operation) {
            case KEY_GOBBLE:
                ProcessGobble(AST, output, context);
                break;
            case KEY_CRY:
                ProcessCry(AST, output, context);
                break;
            case KEY_IF:
                ProcessIf(AST, output, context);
                break;
            case KEY_FOR:
                ProcessFor(AST, output, context);
                break;
            case KEY_WHILE:
                ProcessWhile(AST, output, context);
                break;
            case KEY_RETURN:
                ProcessReturn(AST, output, context);
                break;
            case KEY_IN:
                ProcessIn(AST, output, context);
                break;
            case KEY_TO:
                ProcessTo(AST, output, context);
                break;
            default:
                fprintf(stderr, "UNKNOWN KEYWORD %d\n", AST->data.operation);
                abort();
                break;
        }

        return 1;
    }

    return 0;
}

void ProcessWhile(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

    PushNode(AST->left, output, context);

    PUSH_CONST(0);
    fprintf(output, "push 0\n");

    JE("whileend%u", , context->amounts.whileAmount);
    fprintf(output, "je whileend%u\n", context->amounts.whileAmount);

    LABEL("while%u", , context->amounts.whileAmount);
    fprintf(output, "while%u:\n", context->amounts.whileAmount);

    ASTBypass(AST->right->left, output, context);

    PushNode(AST->left, output, context);

    PUSH_CONST(0);
    fprintf(output, "push 0\n");

    JNE("while%u", , context->amounts.whileAmount);
    fprintf(output, "jne while%u\n", context->amounts.whileAmount);

    LABEL("whileend%u", , context->amounts.whileAmount);
    fprintf(output, "whileend%u:\n", context->amounts.whileAmount);

    context->amounts.whileAmount++;
    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void ProcessFor(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

    uint32_t fromOffset = MakeLocalVar(AST->left->left->data.expression, context);

    double fromValue = AST->left->right->left->data.number;
    double toValue   = 0;
    double iterValue = 0;

    if (AST->left->right->right->type == TYPE_KEYWORD) {
        toValue   = AST->left->right->right->left->data.number;
        iterValue = AST->left->right->right->right->data.number;
    }
    else {
        toValue   = AST->left->right->right->data.number;
        iterValue = 1;
    }

    PUSH_CONST(fromValue);
    fprintf(output, "push %lf\n", fromValue);

    POP_TO_MEM(MEMORY_CELL_SIZE * fromOffset);
    fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE * fromOffset);

    PUSH_TO_MEM(MEMORY_CELL_SIZE * fromOffset);
    fprintf(output, "push {bx + %u}\n", MEMORY_CELL_SIZE * fromOffset);

    PUSH_TO_MEM(toValue);
    fprintf(output, "push %lf\n", toValue);

    if (iterValue > 0) {
        JA("forend%u", ,context->amounts.forAmount);
        fprintf(output, "ja forend%u\n", context->amounts.forAmount);
    }
    else {
        JB("forend%u", ,context->amounts.forAmount);
        fprintf(output, "jb forend%u\n", context->amounts.forAmount);
    }
    
    LABEL("for%u", ,context->amounts.forAmount);
    fprintf(output, "for%u:\n", context->amounts.forAmount);
    
    ASTBypass(AST->right->left, output, context);

    PUSH_TO_MEM(MEMORY_CELL_SIZE*fromOffset);
    fprintf(output, "push {bx + %u}\n", MEMORY_CELL_SIZE*fromOffset);

    PUSH_CONST(iterValue);
    fprintf(output, "push %lf\n", iterValue);

    ADD;
    fprintf(output, "add\n");

    POP_TO_MEM(MEMORY_CELL_SIZE * fromOffset);
    fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE*fromOffset);

    PUSH_TO_MEM(MEMORY_CELL_SIZE * fromOffset);
    fprintf(output, "push {bx + %u}\n", MEMORY_CELL_SIZE*fromOffset);

    PUSH_CONST(toValue);
    fprintf(output, "push %lf\n", toValue);

    if (iterValue > 0) {
        JB("for%u", , context->amounts.forAmount);
        fprintf(output, "jb for%u\n", context->amounts.forAmount);
    }
    else {
        JA("for%u", , context->amounts.forAmount);
        fprintf(output, "ja for%u\n", context->amounts.forAmount);
    }

    LABEL("forend%u", , context->amounts.forAmount);
    fprintf(output, "forend%u:\n", context->amounts.forAmount);
    
    context->amounts.forAmount++;
    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void ProcessGobble(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = MakeLocalVar(AST->left->data.expression, context);

    IN_TO_MEM(MEMORY_CELL_SIZE * varOffset);
    fprintf(output, "in {bx + %u}\n", MEMORY_CELL_SIZE * varOffset);

    if (AST->right != nullptr) {
        ProcessGobble(AST->right, output, context);
    }
}

void ProcessReturn(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    ASTBypass(AST->left, output, context);

    RET;
    fprintf(output, "ret\n");
}

void ProcessTo(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = MakeLocalVar(AST->right->data.expression, context);
    ASTBypass(AST->left, output, context);

    POP_TO_MEM(MEMORY_CELL_SIZE * varOffset);
    fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE * varOffset);
}

void ProcessIn(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = MakeLocalVar(AST->left->data.expression, context);
    ASTBypass(AST->right, output, context);

    POP_TO_MEM(MEMORY_CELL_SIZE * varOffset);
    fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE * varOffset);
}

void ProcessIf(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);
    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);
    uint32_t ifAmount = context->amounts.ifAmount++;

    PushNode(AST->left, output, context);
    
    PUSH_CONST(0);
    fprintf(output, "push 0\n");

    JE("ifelse%u", , ifAmount);
    fprintf(output, "je ifelse%u\n", ifAmount);

    ASTBypass(AST->right->left, output, context);

    JUMP("ifend%u", , ifAmount);
    fprintf(output, "jump ifend%u\n", ifAmount);

    LABEL("ifelse%u", , ifAmount);
    fprintf(output, "ifelse%u:\n", ifAmount);

    if (AST->right->right != nullptr) {
        ASTBypass(AST->right->right, output, context);
    }

    LABEL("ifend%u", , ifAmount);
    fprintf(output, "ifend%u:\n", ifAmount);

    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void ProcessCry(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    ASTBypass(AST->left, output, context);
    if (AST->left->type != TYPE_STR) {
        OUT_CONST(1);
        fprintf(output, "out 1\n");
    }

    if (AST->right != nullptr) {
        ProcessCry(AST->right, output, context);
    }
}

bool ProcessFunction(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if (AST->type == TYPE_FUNC) {
        if (!context->ifInFunction) {
            StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

            LABEL("%s", ,AST->data.expression);
            fprintf(output, "%s:\n", AST->data.expression);

            int32_t paramAmount = 0;
            if (AST->left != nullptr) {
                paramAmount += ProcessFuncArguments(AST->left, output, context);
            }

            MakeFunc(AST->data.expression, context, paramAmount);
            context->ifInFunction = 1;
            
            ASTBypass(AST->right, output, context);

            RET;
            fprintf(output, "ret\n");

            context->offset       = (uint32_t)(int64_t)StackPop(context->offsetStack);
            context->ifInFunction = 0;
        }
        else {
            if ((AST->right != nullptr)        &&
                (AST->right->type == TYPE_UNO) &&
                (AST->right->data.operation == '$')) {
                assert(0 && "DECLARATION OF FUNCTION IN FUNCTION BODY");
            }
            else {
                ExecuteFunction(AST, output, context);
            }
        }

        return 1;
    }

    return 0;
}

int32_t ExecuteFunction(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    int32_t paramAmount = 0;
    if (AST->left != nullptr) {
        ASTBypass(AST->left, output, context);
        if (AST->left->type != TYPE_STR)
            paramAmount++;
    }

    if (AST->right != nullptr) {
        paramAmount += ExecuteFunction(AST->right, output, context);
    }

    if (AST->type == TYPE_FUNC) {
        if (GetFuncArgAmount(AST->data.expression, context) != paramAmount) {
            assert(0 && "INVALID AMOUNT OF ARGUMENTS");
        }

        EnterFuncVisibilityZone(AST, output, context);

        CALL("%s", , AST->data.expression);
        fprintf(output, "call %s\n", AST->data.expression);
        ExitFuncVisibilityZone(AST, output, context);
    }

    return paramAmount;
}

void PushNode(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if (AST->left != nullptr) {
        ASTBypass(AST->left, output, context);
    }

    if (AST->right != nullptr) {
        ASTBypass(AST->right, output, context);
    }

    if (AST->type == TYPE_CONST) {
        PUSH_CONST(AST->data.number);
        fprintf(output, "push %f\n", (double)AST->data.number);
    }

    if (AST->type == TYPE_STR) {
        int32_t stringNum = 0;

        if ((stringNum = FindString(AST->data.expression, context)) != -1) {
            STROUT("str%d", , stringNum);
            fprintf(output, "strout str%d\n", stringNum);
        }
        else {
            STROUT("str%u", , context->amounts.strAmount);
            fprintf(output, "strout str%u\n", context->amounts.strAmount);

            context->stringsArray[context->amounts.strAmount] = AST->data.expression;
            context->amounts.strAmount++;
        }
    }

    if (AST->type == TYPE_VAR) {
        int32_t varOffset = GetVarOffset(AST->data.expression, context);
        if (varOffset == -1)
            assert(0 && "UNKNOWN VAR");

        PUSH_TO_MEM(MEMORY_CELL_SIZE * varOffset);
        fprintf(output, "push {bx + %u}\n", MEMORY_CELL_SIZE * varOffset);
    }

    if (AST->type == TYPE_OP) {
        PrintOperation(AST, output, context);
    }
}

void PrintOperation(Node* node, FILE* output, CodegenContext* context) {
    assert(node    != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    switch (node->data.operation) {
    case ADD_OP:
        ADD;
        fprintf(output, "add\n");
        break;
    case SUB_OP:
        SUB;
        fprintf(output, "sub\n");
        break;
    case DIV_OP:
        DIV;
        fprintf(output, "div\n");
        break;
    case MUL_OP:
        MUL;
        fprintf(output, "mul\n");
        break;
    case POW_OP:
        POW;
        fprintf(output, "pow\n");
        break;
    case L_OP:
        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JB("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "jb compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case LEQ_OP:
        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        ADD;
        fprintf(output, "add\n");

        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JB("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "jb compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case G_OP:
        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JA("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "ja compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case GEQ_OP:
        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        SUB;
        fprintf(output, "sub\n");

        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JA("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "ja compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case DEQ_OP:
        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JE("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "je compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero%u", ,context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case NEQ_OP:
        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JNE("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "jne compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg%u", , context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero%u", , context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    default:
        break;
    }
}

int32_t GetFuncArgAmount(int8_t* name, CodegenContext* context) {
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

int32_t GetVarOffset(int8_t* name, CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curVar = 0; curVar < context->offset; curVar++) {
        if (!strcmp((const char*)context->variables[curVar].name, (const char*)name)) {
            return curVar;
        }
    }

    return -1;
}

int32_t ProcessFuncArguments(Node* node, FILE* output, CodegenContext* context) {
    assert(node    != nullptr);
    assert(context != nullptr);

    int32_t rememberOffset = context->offset;
    int32_t paramAmount = 0;

    if (node->left != nullptr) {
        MakeLocalVar(node->left->data.expression, context);

        paramAmount++;
    }

    if (node->right != nullptr) {
        paramAmount += ProcessFuncArguments(node->right, output, context);
    }

    if (node->left != nullptr) {
        POP_TO_MEM(MEMORY_CELL_SIZE * rememberOffset);
        fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE * rememberOffset);
    }

    return paramAmount;
}

void MakeFunc(int8_t* name, CodegenContext* context, int32_t argAmount) {
    assert(name    != nullptr);
    assert(context != nullptr);

    CheckFuncRepetitions(name, context);

    context->functions[context->amounts.functionsAmount] = {name, argAmount};
    context->amounts.functionsAmount++;
}

uint32_t MakeLocalVar(int8_t* name, CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);
    int32_t repState = GetVarOffset(name, context);

    if (repState == -1) {
        context->variables[context->offset].name = name; 
        context->variables[context->offset].offset[context->recursionDepth] = context->offset;

        context->offset++;

        printf("Made local var %s with offset %u\n", name, context->offset);
        return (context->offset - 1); 
    }
    else {
        printf("Var %s already exist at %u offset\n", name, context->offset);
        return (uint32_t)repState;
    }
}

void CheckFuncRepetitions(int8_t* name, CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curFunc = 0; curFunc < context->amounts.functionsAmount; curFunc++) {
        if (!strcmp((const char*)context->functions[curFunc].name, (const char*)name)) {
            assert(0 && "TWO VARS WITH EQUAL NAMES");
        }
    }
}

bool SkipConk(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if (((AST->type == TYPE_UNO)       &&
         (AST->data.operation == EOL_OP)) ||
        ((AST->type == TYPE_KEYWORD) &&
         (AST->data.operation == KEY_LILEND))) {
        if (AST->left != nullptr) {
            ASTBypass(AST->left, output, context);
        }

        if (AST->right != nullptr) {
            ASTBypass(AST->right, output, context);
        }

        return 1;
    }

    return 0;
}

void EnterFuncVisibilityZone(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

    PUSH_CONST(MEMORY_CELL_SIZE * context->offset);
    fprintf(output, "push %u\n", MEMORY_CELL_SIZE * context->offset);

    PUSH_REG;
    fprintf(output, "push bx\n");

    ADD;
    fprintf(output, "add\n");

    POP_TO_REG;
    fprintf(output, "pop bx\n");
}

void ExitFuncVisibilityZone(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    context->offset = (!context->offsetStack->size) ? 0 : (uint32_t)(int64_t)StackPop(context->offsetStack);

    PUSH_REG;
    fprintf(output, "push bx\n");

    PUSH_CONST(MEMORY_CELL_SIZE * context->offset);
    fprintf(output, "push %u\n", MEMORY_CELL_SIZE * context->offset);

    SUB;
    fprintf(output, "sub\n");

    POP_TO_REG;
    fprintf(output, "pop bx\n");
}

int32_t FindString(int8_t* name, CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curStr = 0; curStr < context->amounts.strAmount; curStr++) {
        if (!strcmp((const char*)name, (const char*)context->stringsArray[curStr])) {
            return curStr;
        }
    }

    return -1;
}

void PrintEndOfProgram(FILE* output, CodegenContext* context) {
    assert(output  != nullptr);
    assert(context != nullptr);

    for (uint32_t curStr = 0; curStr < context->amounts.strAmount; curStr++) {
        LABEL("str%u", , curStr);
        fprintf(output, "str%u:\n", curStr);

        STRING(context->stringsArray[curStr]);
        fprintf(output, "db %s\n\n", context->stringsArray[curStr]);
    }
}
