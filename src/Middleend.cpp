#include "./Middleend/Middleend.h"

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
    
#define STRING(strName)     MakeString(strName, context)

#define SINGULAR_OP(number) EmitCommand(number, context->result)

#define ADD SINGULAR_OP(8)
#define SUB SINGULAR_OP(12)
#define MUL SINGULAR_OP(16)
#define DIV SINGULAR_OP(20)
#define POW SINGULAR_OP(52)

    bool isLog       = 0;
    bool isShowTree  = 0;
    char *outputName = 0;

int main(int argc, char* argv[]) {
    ProcessMiddleendArguments(argc, argv);

    char treeName[MAX_FILE_NAME_LENGTH] = "";
    
    if (outputName == nullptr)
        GenerateOutputName(ASM_NAME, treeName, ASM_PATH, ASM_OUTPUT_FORMAT);
    else 
        strcat(treeName, outputName);
    strcat(treeName, TREE_OUTPUT_FORMAT);
    
    Text treeText = {};
    ReadTextFromFile(&treeText, treeName);

    TreeCtor(ASTREADED);

    uint64_t curByte = 0;
    ASTREADED.root   = ReadTreeFromDisk(&treeText, &curByte);

    if (isShowTree) {
        MakeTreeGraph(&ASTREADED, G_STANDART_NAME);
    }

    GenerateCode(&ASTREADED);

    TreeDtor(&ASTREADED);
}

void ProcessMiddleendArguments(int argc, char* argv[]) {
    for (int32_t curArg = 1; curArg < argc; curArg++) {
        if (!strcmp((const char*)argv[curArg], "-asmS")) {
            isLog = 1;
        }

        if (!strcmp((const char*)argv[curArg], "-middleG")) {
            isShowTree = 1;
        }

        if (!strcmp((const char*)argv[curArg], "-o")) {
            outputName = argv[curArg + 1];
        }
    }
}

void GenerateCode(Tree* AST) {
    assert(AST != nullptr);

    StackCtor(offsetStack);
    char endName[MAX_FILE_NAME_LENGTH] = "";

    if (isLog) GenerateOutputName(ASM_NAME, endName, ASM_PATH, ASM_OUTPUT_FORMAT);
    else strcat(endName, outputName);
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
    if (isLog) fprintf(output, "call main\n");

    HLT;
    if (isLog) fprintf(output, "hlt\n");
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
    if (isLog) fprintf(output, "call main\n");
    HLT;
    if (isLog) fprintf(output, "hlt\n");
    ASTBypass(AST->root, output, context);
    PrintEndOfProgram(output, context);
    if (isLog) fclose(output);

    strcat(endName, ASM_OUTPUT_FORMAT);
    FILE* outputd = fopen(endName, "wb");
    printf("I wrote %llu signature bytes to %s\n",  fwrite(SIGNATURE, 1, SIGNATURE_SIZE, outputd), endName);
    printf("I wrote %llu bytes from array\n", fwrite(context->result->bytesArray, 1, context->result->bytesCount, outputd));
    if (isLog) fclose(outputd);

    StackDtor(&offsetStack2);
    
}

void ASTBypass(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
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
    assert(context != nullptr);

    if (AST->type == NodeDataTypes::TYPE_KEYWORD) {
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
                if (isLog) fprintf(stderr, "UNKNOWN KEYWORD %d\n", AST->data.operation);
                abort();
                break;
        }

        return 1;
    }

    return 0;
}

void ProcessWhile(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

    PushNode(AST->left, output, context);

    PUSH_CONST(0);
    if (isLog) fprintf(output, "push 0\n");

    JE("whileend", context->amounts.whileAmount);
    if (isLog) fprintf(output, "je whileend%u\n", context->amounts.whileAmount);

    LABEL("while", context->amounts.whileAmount);
    if (isLog) fprintf(output, "while%u:\n", context->amounts.whileAmount);

    ASTBypass(AST->right->left, output, context);

    PushNode(AST->left, output, context);

    PUSH_CONST(0);
    if (isLog) fprintf(output, "push 0\n");

    JNE("while", context->amounts.whileAmount);
    if (isLog) fprintf(output, "jne while%u\n", context->amounts.whileAmount);

    LABEL("whileend", context->amounts.whileAmount);
    if (isLog) fprintf(output, "whileend%u:\n", context->amounts.whileAmount);

    context->amounts.whileAmount++;
    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void ProcessFor(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

    uint32_t fromOffset = MakeLocalVar(AST->left->left->data.expression, context);

    double fromValue = AST->left->right->left->data.number;
    double toValue   = 0;
    double iterValue = 0;

    if (AST->left->right->right->type == NodeDataTypes::TYPE_KEYWORD) {
        toValue   = AST->left->right->right->left->data.number;
        iterValue = AST->left->right->right->right->data.number;
    }
    else {
        toValue   = AST->left->right->right->data.number;
        iterValue = 1;
    }

    PUSH_CONST(fromValue);
    if (isLog) fprintf(output, "push %lf\n", fromValue);

    POP_TO_MEM(MEMORY_CELL_SIZE * fromOffset);
    if (isLog) fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE * fromOffset);

    PUSH_TO_MEM(MEMORY_CELL_SIZE * fromOffset);
    if (isLog) fprintf(output, "push {bx + %u}\n", MEMORY_CELL_SIZE * fromOffset);

    PUSH_TO_MEM(toValue);
    if (isLog) fprintf(output, "push %lf\n", toValue);

    if (iterValue > 0) {
        JA("forend", context->amounts.forAmount);
        if (isLog) fprintf(output, "ja forend%u\n", context->amounts.forAmount);
    }
    else {
        JB("forend", context->amounts.forAmount);
        if (isLog) fprintf(output, "jb forend%u\n", context->amounts.forAmount);
    }
    
    LABEL("for", context->amounts.forAmount);
    if (isLog) fprintf(output, "for%u:\n", context->amounts.forAmount);
    
    ASTBypass(AST->right->left, output, context);

    PUSH_TO_MEM(MEMORY_CELL_SIZE*fromOffset);
    if (isLog) fprintf(output, "push {bx + %u}\n", MEMORY_CELL_SIZE*fromOffset);

    PUSH_CONST(iterValue);
    if (isLog) fprintf(output, "push %lf\n", iterValue);

    ADD;
    if (isLog) fprintf(output, "add\n");

    POP_TO_MEM(MEMORY_CELL_SIZE * fromOffset);
    if (isLog) fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE*fromOffset);

    PUSH_TO_MEM(MEMORY_CELL_SIZE * fromOffset);
    if (isLog) fprintf(output, "push {bx + %u}\n", MEMORY_CELL_SIZE*fromOffset);

    PUSH_CONST(toValue);
    if (isLog) fprintf(output, "push %lf\n", toValue);

    if (iterValue > 0) {
        JB("for", context->amounts.forAmount);
        if (isLog) fprintf(output, "jb for%u\n", context->amounts.forAmount);
    }
    else {
        JA("for", context->amounts.forAmount);
        if (isLog) fprintf(output, "ja for%u\n", context->amounts.forAmount);
    }

    LABEL("forend", context->amounts.forAmount);
    if (isLog) fprintf(output, "forend%u:\n", context->amounts.forAmount);
    
    context->amounts.forAmount++;
    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void ProcessGobble(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = MakeLocalVar(AST->left->data.expression, context);

    IN_TO_MEM(MEMORY_CELL_SIZE * varOffset);
    if (isLog) fprintf(output, "in {bx + %u}\n", MEMORY_CELL_SIZE * varOffset);

    if (AST->right != nullptr) {
        ProcessGobble(AST->right, output, context);
    }
}

void ProcessReturn(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    ASTBypass(AST->left, output, context);

    RET;
    if (isLog) fprintf(output, "ret\n");
}

void ProcessTo(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = MakeLocalVar(AST->right->data.expression, context);
    ASTBypass(AST->left, output, context);

    POP_TO_MEM(MEMORY_CELL_SIZE * varOffset);
    if (isLog) fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE * varOffset);
}

void ProcessIn(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = MakeLocalVar(AST->left->data.expression, context);
    ASTBypass(AST->right, output, context);

    POP_TO_MEM(MEMORY_CELL_SIZE * varOffset);
    if (isLog) fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE * varOffset);
}

void ProcessIf(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);
    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);
    uint32_t ifAmount = context->amounts.ifAmount++;

    PushNode(AST->left, output, context);
    
    PUSH_CONST(0);
    if (isLog) fprintf(output, "push 0\n");

    JE("ifelse", ifAmount);
    if (isLog) fprintf(output, "je ifelse%u\n", ifAmount);

    ASTBypass(AST->right->left, output, context);

    JUMP("ifend", ifAmount);
    if (isLog) fprintf(output, "jump ifend%u\n", ifAmount);

    LABEL("ifelse", ifAmount);
    if (isLog) fprintf(output, "ifelse%u:\n", ifAmount);

    if (AST->right->right != nullptr) {
        ASTBypass(AST->right->right, output, context);
    }

    LABEL("ifend", ifAmount);
    if (isLog) fprintf(output, "ifend%u:\n", ifAmount);

    context->offset = (uint32_t)(int64_t)StackPop(context->offsetStack);
}

void ProcessCry(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    ASTBypass(AST->left, output, context);
    if (AST->left->type != NodeDataTypes::TYPE_STR) {
        OUT_CONST(1);
        if (isLog) fprintf(output, "out 1\n");
    }

    if (AST->right != nullptr) {
        ProcessCry(AST->right, output, context);
    }
}

bool ProcessFunction(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    if (AST->type == NodeDataTypes::TYPE_FUNC) {
        if (!context->ifInFunction) {
            StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

            LABEL((const char*)AST->data.expression, -1);
            if (isLog) fprintf(output, "%s:\n", AST->data.expression);

            int32_t paramAmount = 0;
            if (AST->left != nullptr) {
                paramAmount += ProcessFuncArguments(AST->left, output, context);
            }

            MakeFunc(AST->data.expression, context, paramAmount);
            context->ifInFunction = 1;
            
            ASTBypass(AST->right, output, context);

            RET;
            if (isLog) fprintf(output, "ret\n");

            context->offset       = (uint32_t)(int64_t)StackPop(context->offsetStack);
            context->ifInFunction = 0;
        }
        else {
            if ((AST->right != nullptr)        &&
                (AST->right->type == NodeDataTypes::TYPE_UNO) &&
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
    assert(context != nullptr);

    int32_t paramAmount = 0;
    if (AST->left != nullptr) {
        ASTBypass(AST->left, output, context);
        if (AST->left->type != NodeDataTypes::TYPE_STR)
            paramAmount++;
    }

    if (AST->right != nullptr) {
        paramAmount += ExecuteFunction(AST->right, output, context);
    }

    if (AST->type == NodeDataTypes::TYPE_FUNC) {
        if (GetFuncArgAmount(AST->data.expression, context) != paramAmount) {
            assert(0 && "INVALID AMOUNT OF ARGUMENTS");
        }

        EnterFuncVisibilityZone(AST, output, context);

        CALL((const char*)AST->data.expression);
        if (isLog) fprintf(output, "call %s\n", AST->data.expression);
        ExitFuncVisibilityZone(AST, output, context);
    }

    return paramAmount;
}

void PushNode(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    if (AST->left != nullptr) {
        ASTBypass(AST->left, output, context);
    }

    if (AST->right != nullptr) {
        ASTBypass(AST->right, output, context);
    }

    if (AST->type == NodeDataTypes::TYPE_CONST) {
        PUSH_CONST(AST->data.number);
        if (isLog) fprintf(output, "push %f\n", (double)AST->data.number);
    }

    if (AST->type == NodeDataTypes::TYPE_STR) {
        int32_t stringNum = 0;

        if ((stringNum = FindString(AST->data.expression, context)) != -1) {
            STROUT("str", stringNum);
            if (isLog) fprintf(output, "strout str%d\n", stringNum);
        }
        else {
            STROUT("str", context->amounts.strAmount);
            if (isLog) fprintf(output, "strout str%u\n", context->amounts.strAmount);

            context->stringsArray[context->amounts.strAmount] = AST->data.expression;
            context->amounts.strAmount++;
        }
    }

    if (AST->type == NodeDataTypes::TYPE_VAR) {
        int32_t varOffset = GetVarOffset(AST->data.expression, context);
        if (varOffset == -1)
            assert(0 && "UNKNOWN VAR");

        PUSH_TO_MEM(MEMORY_CELL_SIZE * varOffset);
        if (isLog) fprintf(output, "push {bx + %u}\n", MEMORY_CELL_SIZE * varOffset);
    }

    if (AST->type == NodeDataTypes::TYPE_OP) {
        PrintOperation(AST, output, context);
    }
}

void PrintOperation(Node* node, FILE* output, CodegenContext* context) {
    assert(node    != nullptr);
    assert(context != nullptr);

    switch (node->data.operation) {
    case ADD_OP:
        ADD;
        if (isLog) fprintf(output, "add\n");
        break;
    case SUB_OP:
        SUB;
        if (isLog) fprintf(output, "sub\n");
        break;
    case DIV_OP:
        DIV;
        if (isLog) fprintf(output, "div\n");
        break;
    case MUL_OP:
        MUL;
        if (isLog) fprintf(output, "mul\n");
        break;
    case POW_OP:
        POW;
        if (isLog) fprintf(output, "pow\n");
        break;
    case L_OP:
        SUB;
        if (isLog) fprintf(output, "sub\n");

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JB("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jb compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        if (isLog) fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case LEQ_OP:
        PUSH_CONST(1);
        if (isLog) fprintf(output, "push 1\n");

        ADD;
        if (isLog) fprintf(output, "add\n");

        SUB;
        if (isLog) fprintf(output, "sub\n");

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JB("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jb compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        if (isLog) fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case G_OP:
        SUB;
        if (isLog) fprintf(output, "sub\n");

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JA("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "ja compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        if (isLog) fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case GEQ_OP:
        PUSH_CONST(1);
        if (isLog) fprintf(output, "push 1\n");

        SUB;
        if (isLog) fprintf(output, "sub\n");

        SUB;
        if (isLog) fprintf(output, "sub\n");

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JA("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "ja compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        if (isLog) fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case DEQ_OP:
        SUB;
        if (isLog) fprintf(output, "sub\n");

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JE("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "je compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        if (isLog) fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

        context->amounts.compareAmount++;
        break;
    case NEQ_OP:
        SUB;
        if (isLog) fprintf(output, "sub\n");

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JNE("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jne compareg%u\n", context->amounts.compareAmount);

        PUSH_CONST(0);
        if (isLog) fprintf(output, "push 0\n");

        JUMP("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "jump comparezero%u\n", context->amounts.compareAmount);

        LABEL("compareg", context->amounts.compareAmount);
        if (isLog) fprintf(output, "compareg%u:\n", context->amounts.compareAmount);

        PUSH_CONST(1);
        if (isLog) fprintf(output, "push 1\n");

        LABEL("comparezero", context->amounts.compareAmount);
        if (isLog) fprintf(output, "comparezero%u:\n", context->amounts.compareAmount);

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
        if (isLog) fprintf(output, "pop {bx + %u}\n", MEMORY_CELL_SIZE * rememberOffset);
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
    assert(context != nullptr);

    if (((AST->type == NodeDataTypes::TYPE_UNO)       &&
         (AST->data.operation == EOL_OP)) ||
        ((AST->type == NodeDataTypes::TYPE_KEYWORD) &&
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
    assert(context != nullptr);

    StackPush(context->offsetStack, (StackElem)(int64_t)context->offset);

    PUSH_CONST(MEMORY_CELL_SIZE * context->offset);
    if (isLog) fprintf(output, "push %u\n", MEMORY_CELL_SIZE * context->offset);

    PUSH_TO_REG;
    if (isLog) fprintf(output, "push bx\n");

    ADD;
    if (isLog) fprintf(output, "add\n");

    POP_TO_REG;
    if (isLog) fprintf(output, "pop bx\n");
}

void ExitFuncVisibilityZone(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(context != nullptr);

    context->offset = (!context->offsetStack->size) ? 0 : (uint32_t)(int64_t)StackPop(context->offsetStack);

    PUSH_TO_REG;
    if (isLog) fprintf(output, "push bx\n");

    PUSH_CONST(MEMORY_CELL_SIZE * context->offset);
    if (isLog) fprintf(output, "push %u\n", MEMORY_CELL_SIZE * context->offset);

    SUB;
    if (isLog) fprintf(output, "sub\n");

    POP_TO_REG;
    if (isLog) fprintf(output, "pop bx\n");
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
    assert(context != nullptr);

    for (uint32_t curStr = 0; curStr < context->amounts.strAmount; curStr++) {
        LABEL("str", curStr);
        if (isLog) fprintf(output, "str%u:\n", curStr);

        STRING((const char*)context->stringsArray[curStr]);
        if (isLog) fprintf(output, "db %s\n\n", context->stringsArray[curStr]);
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

