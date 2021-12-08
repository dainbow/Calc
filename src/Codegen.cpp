#include "Codegen.h"

#define LABEL_FLAGS     {0, 1, 0, 0, 0}
#define NONE_FLAGS      {0, 0, 0, 0, 0}
#define STRING_FLAGS    {1, 0, 0, 0, 0}
#define TO_MEM_FLAGS    {0, 0, 1, 1, 1}
#define CONST_FLAGS     {0, 0, 1, 0, 0}
#define REGISTER_FLAGS  {0, 0, 0, 1, 0}

#define CALL(function)          MakeSomethingWithLabel(41, function, -1, context)

#define JA(function, number)    MakeSomethingWithLabel(17, function, number, context)

#define JAE(function, number)       MakeSomethingWithLabel(21, function, number, context)

#define JB(function, number)        MakeSomethingWithLabel(25, function, number, context)

#define JBE(function, number)       MakeSomethingWithLabel(29, function, number, context)

#define JE(function, number)        MakeSomethingWithLabel(33, function, number, context)

#define JNE(function, number)       MakeSomethingWithLabel(37, function, number, context)

#define JUMP(function, number)      MakeSomethingWithLabel(9, function, number, context)

#define STROUT(function, number)    MakeSomethingWithLabel(49, function, number, context)
    
#define LABEL(labelName, number)    MakeLabel(labelName, number, context)                          

#define HLT EmitCommand(0, context->result)   
#define RET EmitCommand(44, context->result)    

#define POP_TO_REG  DoToRegister(5, context)
#define PUSH_TO_REG DoToRegister(1, context)

#define PUSH_CONST(constant) DoWithConstant(1, constant, context)
#define OUT_CONST(constant)  DoWithConstant(57, constant, context)

#define PUSH_TO_MEM(offset) DoToMem(1, offset, context)
#define POP_TO_MEM(offset)  DoToMem(5, offset, context)
#define IN_TO_MEM(offset)   DoToMem(13, offset, context)
    
#define STRING(strName) MakeString(strName, context)

#define SINGULAR_OP(number) EmitCommand(number, context->result)

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

    JE("whileend", context->amounts.whileAmount);
    fprintf(output, "je whileend%u\n", context->amounts.whileAmount);

    LABEL("while", context->amounts.whileAmount);
    fprintf(output, "while%u:\n", context->amounts.whileAmount);

    ASTBypass(AST->right->left, output, context);

    PushNode(AST->left, output, context);

    PUSH_CONST(0);
    fprintf(output, "push 0\n");

    JNE("while", context->amounts.whileAmount);
    fprintf(output, "jne while%u\n", context->amounts.whileAmount);

    LABEL("whileend", context->amounts.whileAmount);
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
        JA("forend", context->amounts.forAmount);
        fprintf(output, "ja forend%u\n", context->amounts.forAmount);
    }
    else {
        JB("forend", context->amounts.forAmount);
        fprintf(output, "jb forend%u\n", context->amounts.forAmount);
    }
    
    LABEL("for", context->amounts.forAmount);
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
        JB("for", context->amounts.forAmount);
        fprintf(output, "jb for%u\n", context->amounts.forAmount);
    }
    else {
        JA("for", context->amounts.forAmount);
        fprintf(output, "ja for%u\n", context->amounts.forAmount);
    }

    LABEL("forend", context->amounts.forAmount);
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

    JE("ifelse", ifAmount);
    fprintf(output, "je ifelse%u\n", ifAmount);

    ASTBypass(AST->right->left, output, context);

    JUMP("ifend", ifAmount);
    fprintf(output, "jump ifend%u\n", ifAmount);

    LABEL("ifelse", ifAmount);
    fprintf(output, "ifelse%u:\n", ifAmount);

    if (AST->right->right != nullptr) {
        ASTBypass(AST->right->right, output, context);
    }

    LABEL("ifend", ifAmount);
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

            LABEL((const char*)AST->data.expression, -1);
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

        CALL((const char*)AST->data.expression);
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
            STROUT("str", stringNum);
            fprintf(output, "strout str%d\n", stringNum);
        }
        else {
            STROUT("str", context->amounts.strAmount);
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

        JB("compareg", context->amounts.compareAmount);
        fprintf(output, "jb compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
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

        JB("compareg", context->amounts.compareAmount);
        fprintf(output, "jb compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case G_OP:
        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JA("compareg", context->amounts.compareAmount);
        fprintf(output, "ja compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
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

        JA("compareg", context->amounts.compareAmount);
        fprintf(output, "ja compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case DEQ_OP:
        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JE("compareg", context->amounts.compareAmount);
        fprintf(output, "je compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case NEQ_OP:
        SUB;
        fprintf(output, "sub\n");

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JNE("compareg", context->amounts.compareAmount);
        fprintf(output, "jne compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
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

        return (context->offset - 1); 
    }
    else {
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

    PUSH_TO_REG;
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

    PUSH_TO_REG;
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
        LABEL("str", curStr);
        fprintf(output, "str%u:\n", curStr);

        STRING((const char*)context->stringsArray[curStr]);
        fprintf(output, "db %s\n\n", context->stringsArray[curStr]);
    }
}

void FrontendMinusOne(Tree* AST) {
    assert(AST != nullptr);

    char endName[MAX_FILE_NAME_LENGTH] = "";
    GenerateOutputName(ASM_NAME, endName, ASM_PATH, ASM_OUTPUT_FORMAT);

    strcat(endName, ASM_REVERSED_FORMAT);
    FILE* outputReversed = fopen(endName, "w");

    PrintAST(AST->root, outputReversed);
    fprintf(outputReversed, "bigEnd\n");

    fclose(outputReversed);
}

void PrintAST(Node* node, FILE* output) {
    assert(node    != nullptr);
    assert(output != nullptr);

    switch (node->type) {
        case TYPE_FUNC:
            PrintFunction(node, output);
            break;
        case TYPE_KEYWORD:
            PrintKeyword(node, output);
            break;
        case TYPE_UNO: 
        case TYPE_OP:
            PrintOperationMinus1(node, output);
            break;
        case TYPE_STR:
            PrintString(node, output);
            break;
        case TYPE_VAR:
            fprintf(output, "%s ", node->data.expression);
            break;
        case TYPE_CONST:
            fprintf(output, "%lf ", node->data.number);
            break;
        case TYPE_UNKNOWN:
            assert(FAIL && "TYPE IS UNKNOWN");
            break;
        default:
            assert(FAIL && "UNKNOWN TYPE");
            break;
    }
}

void PrintString(Node* node, FILE* output) {
    assert(node   != nullptr);
    assert(output != nullptr);

    fprintf(output, "\"%s\" ", node->data.expression);
}

#define BRACKETS_IF(DISTONATION, BRACKETS_TYPE)                             \
    if ((node-> DISTONATION ->type == TYPE_OP) &&                           \
        (GetOperationPriority(node-> DISTONATION-> data.operation) <        \
        GetOperationPriority(node->data.operation))) {                      \
        fprintf(output, BRACKETS_TYPE);                                     \
    }

void PrintOperationMinus1(Node* node, FILE* output) {
    assert(node   != nullptr);
    assert(output != nullptr);

    if (node->left != nullptr) {
        BRACKETS_IF(left, "(")

        PrintAST(node->left, output);

        BRACKETS_IF(left, ")")
    }

    switch (node->data.operation) {
        case COMMA_OP:
            if ((node->left != nullptr) && (node->right != nullptr))
                fprintf(output, ", ");
            break;
        case ADD_OP:
            fprintf(output, "+ ");
            break;
        case SUB_OP: 
            fprintf(output, "- ");
            break;
        case MUL_OP:
            fprintf(output, "* ");
            break;
        case DIV_OP: 
            fprintf(output, "/ ");
            break;
        case POW_OP:
            fprintf(output, "^ ");
            break;
        case NON_OP:
            fprintf(output, "! ");
            break;          
        case L_OP:    
            fprintf(output, "< ");
            break;
        case LEQ_OP:
            fprintf(output, "<= ");
            break;
        case G_OP:
            fprintf(output, "> ");
            break;
        case GEQ_OP:
            fprintf(output, ">= ");
            break;
        case EQ_OP:
            fprintf(output, "= ");
            break;
        case DEQ_OP:
            fprintf(output, "== ");
            break;
        case NEQ_OP:
            fprintf(output, "!= ");
            break;
        case LEFT_ROUND_OP: 
            fprintf(output, "(");
            break;
        case RIGHT_ROUND_OP:
            fprintf(output, ") ");
            break;
        case LEFT_SQR_OP:
            fprintf(output, "[");
            break;
        case RIGHT_SQR_OP:
            fprintf(output, "] ");
            break;
        case LEFT_CUR_OP:
            fprintf(output, "{");
            break;
        case RIGHT_CUR_OP:
            fprintf(output, "} ");
            break;
        case EOL_OP:
            if  (!((node->left->type == TYPE_KEYWORD)       &&
                ((node->left->data.operation == KEY_FOR)    ||
                 (node->left->data.operation == KEY_CRY)    ||
                 (node->left->data.operation == KEY_GOBBLE) ||
                 (node->left->data.operation == KEY_IF))))
                fprintf(output, "$\n");
            break;
        default:
            assert(FAIL && "UNKNOWN OPERATOR");
            break;
    }

    if (node->right != nullptr) {
        BRACKETS_IF(right, "(")

        PrintAST(node->right, output);

        BRACKETS_IF(right, ")")
    }
}

int32_t GetOperationPriority(int8_t operation) {
    switch (operation) {
        case POW_OP:
            return 4;
        case MUL_OP:
        case DIV_OP:
            return 3;
        case ADD_OP:
        case SUB_OP:
            return 2;
        default:
            return 1;
            break;
    }
}

void PrintFunction(Node* node, FILE* output) {
    assert(node   != nullptr);
    assert(output != nullptr);

    if ((node->left  == nullptr) &&
        (node->right == nullptr)) {
        fprintf(output, "%s() ", node->data.expression);
    }
    else if (node->left != nullptr) {
        if ((node->left->type           == TYPE_KEYWORD) &&
            (node->left->data.operation == KEY_WITH)) {
            fprintf(output, "%s with ", node->data.expression);
            
            PrintAST(node->left->left, output);
            if (node->left->right != nullptr) {
                fprintf(output, ", ");
                PrintAST(node->left->right, output);
            }

            fprintf(output, "begin \n");

            PrintAST(node->right, output);
        }
        else {
            fprintf(output, "%s( ", node->data.expression);
            
            PrintAST(node->left,  output);
            if (node->right != nullptr) {
                fprintf(output, ", ");
                PrintAST(node->right, output);
            }

            fprintf(output, ")");
        }
    }
    else {
        fprintf(output, "%s ", node->data.expression);  
        fprintf(output, "begin \n");

        PrintAST(node->right, output);
    }
}

void PrintKeyword(Node* node, FILE* output) {
    assert(node   != nullptr);
    assert(output != nullptr);

    switch(node->data.operation) {
        case KEY_IF:
            fprintf(output, "vasyaSniff ");
            PrintAST(node->left, output);

            fprintf(output, "begin\n");

            PrintAST(node->right->left, output);
            fprintf(output, "lilEnd\n");
            
            if (node->right->right != nullptr) {
                fprintf(output, "someoneSniff begin\n");
                PrintAST(node->right->right, output);
                fprintf(output, "lilEnd\n");
            }
            break;
        case KEY_BEGIN:
            fprintf(output, "begin\n");

            if (node->left != nullptr)
                PrintAST(node->left, output);

            if (node->right != nullptr)
                PrintAST(node->right, output);
            break;
        case KEY_CRY:
            fprintf(output, "cry ");

            if (node->left != nullptr)
                PrintAST(node->left, output);
            if (node->right != nullptr) { 
                fprintf(output, ", ");
                PrintAST(node->right, output);
            }

            fprintf(output, "dot\n");
            break;
        case KEY_DIFF:
            assert(FAIL && "MAKE DIFF");
            break;
        case KEY_DOT:
            fprintf(output, "dot\n");
            break;
        case KEY_ELSE:
            assert(FAIL && "MALE ELSE");
            break;
        case KEY_ELSEIF:
            assert(FAIL && "MAKE ELSEIF");
            break;
        case KEY_END:
            assert(FAIL && "MAKE BIGEND");
            break;
        case KEY_FOR:
            fprintf(output, "hire ");

            PrintAST(node->left, output);
            PrintAST(node->right, output);
            break;
        case KEY_FROM:
            PrintAST(node->left, output);

            fprintf(output, "from ");
            
            PrintAST(node->right, output);
            break;
        case KEY_GOBBLE:
            fprintf(output, "gobble ");

            if (node->left != nullptr)
                PrintAST(node->left, output);
            if (node->right != nullptr) {
                fprintf(output, ", ");
                PrintAST(node->right, output);
            }

            fprintf(output, "dot\n");
            break;
        case KEY_WITH:
            if (node->left != nullptr)
                PrintAST(node->left, output);

            fprintf(output, "with ");
            
            if (node->right != nullptr)
                PrintAST(node->right, output);
            break;
        case KEY_LILEND:
            if (node->left != nullptr)
                PrintAST(node->left, output);

            fprintf(output, "lilEnd\n");
            
            if (node->right != nullptr)
                PrintAST(node->right, output);
            break;
        case KEY_RETURN:
            fprintf(output, "spit ");
            
            if (node->left != nullptr)
                PrintAST(node->left, output);

            if (node->right != nullptr)
                PrintAST(node->right, output);
            break;
        case KEY_TO:
            if (node->left != nullptr)
                PrintAST(node->left, output);

            fprintf(output, "to ");
            
            if (node->right != nullptr)
                PrintAST(node->right, output);
            break;
        case KEY_IN:
            if (node->left != nullptr)
                PrintAST(node->left, output);

            fprintf(output, "in ");
            
            if (node->right != nullptr)
                PrintAST(node->right, output);
            break;   
        case KEY_WHILE:
            fprintf(output, "homyak ");

            PrintAST(node->left, output);

            fprintf(output, "begin\n");
            PrintAST(node->right, output);
            break;
        default:
            fprintf(stderr, "UNKNOWN KEY %d\n", node->data.operation);
            abort();
            break;
    }
}

void NewArgument(Flags flags, int32_t argConstant, int32_t reg, const char labelName[], int32_t labelNum, const char string[], CodegenContext* context) {
    assert(context   != nullptr);

    context->arguments[context->amounts.argumentsAmount].argFlags   = flags;                              
    context->arguments[context->amounts.argumentsAmount].argConst   = argConstant;                          
    context->arguments[context->amounts.argumentsAmount].argReg     = reg;                                

    if (labelName != nullptr) {                                                                 
        if (labelNum != -1) 
            sprintf((char*)context->arguments[context->amounts.argumentsAmount].labelName,  "%s%d", labelName, labelNum);   
        else   
            sprintf((char*)context->arguments[context->amounts.argumentsAmount].labelName,  "%s", labelName);      
    }
    
    if (string != nullptr)                                                                                                
        sprintf((char*)context->arguments[context->amounts.argumentsAmount].stringName, "%s", string);              
                                                                                        
    context->amounts.argumentsAmount++;
}

void MakeSomethingWithLabel(int8_t commandNum, const char labelName[], uint32_t labelNum, CodegenContext* context) {
    assert(labelName != nullptr);

    EmitCommand(commandNum, context->result);                       
    NewArgument(LABEL_FLAGS, 0, 0, labelName, labelNum, nullptr, context);   
    EmitArgs(commandNum, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels);
}

void MakeString(const char stringName[], CodegenContext* context) {
    assert(stringName != nullptr);
    assert(context    != nullptr);

    NewArgument(STRING_FLAGS, 0, 0, nullptr, 0, stringName, context);                                                           
    EmitArgs(36, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels);
}

void MakeLabel(const char labelName[], int32_t labelNumber, CodegenContext* context) {
    assert(context   != nullptr);
    assert(labelName != nullptr);

    NewArgument(LABEL_FLAGS, 0, 0, labelName, labelNumber, (char*)nullptr, context);     
    EmitArgs(0, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels);
}

void DoWithConstant(int8_t cmdNum, double constant, CodegenContext* context) {
    assert(context != nullptr);

    EmitCommand(cmdNum, context->result);                                                                            
    NewArgument(CONST_FLAGS, (ProcStackElem)(constant * ACCURACY), 0, nullptr, -1, nullptr, context);                                
    EmitArgs(cmdNum, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels);
}

void DoToRegister(int8_t cmdNum, CodegenContext* context) {
    EmitCommand(cmdNum, context->result);                                                                            
    NewArgument(REGISTER_FLAGS, 0, 1, nullptr, -1, nullptr, context);                                                              
    EmitArgs(cmdNum, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels);
}

void DoToMem(int8_t cmdNum, double offset, CodegenContext* context) {
    assert(context != nullptr);

    EmitCommand(cmdNum, context->result);                                                                            
    NewArgument(TO_MEM_FLAGS, (ProcStackElem)(offset * ACCURACY), 1, nullptr, -1, (char*)nullptr, context);                                          
    EmitArgs(cmdNum, context->result, context->arguments + context->amounts.argumentsAmount - 1, context->labels);
}

