#include "Codegen.h"

void GenerateCode(Tree* AST) {
    assert(AST != nullptr);

    StackCtor(offsetStack);
    char* endName = 0;

    GenerateOutputName(ASM_NAME, endName, ASM_PATH, ASM_OUTPUT_FORMAT);
    FILE* output = fopen(endName, "w");

    CodegenContext* context = (CodegenContext*)calloc(1, sizeof(context[0]));
    context->offsetStack    = &offsetStack; 

    ASTBypass(AST->root, output, context);

    StackDtor(&offsetStack);
    fclose(output);
}

void ASTBypass(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    SkipStrConk(AST, output, context);

    ProcessFunction(AST, output, context);
    ProcessKeyword(AST, output, context);
}

void ProcessKeyword(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if (AST->type == TYPE_KEYWORD) {
        switch (AST->data.operation) {
            case KEY_IF:
                ProcessIf(AST, output, context);
                break;
            case KEY_FOR:
                ProcessFor(AST, output, context);
                break;
            /*case KEY_WHILE:
                ProcessWhile(AST, output, context);
                break;
            case KEY_RETURN:
                ProcessReturn(AST, output, context);
                break;*/
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
    }
}

void ProcessReturn(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    PushNode(AST->right, output, context);
    fprintf(output, "ret\n");
}

void ProcessTo(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = MakeLocalVar(AST->right->data.expression, context);
    PushNode(AST->left, output, context);

    fprintf(output, "pop {bx + %u}\n", varOffset);
}

void ProcessIn(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    uint32_t varOffset = MakeLocalVar(AST->left->data.expression, context);
    PushNode(AST->right, output, context);

    fprintf(output, "pop {bx + %u}\n", varOffset);
}

void ProcessIf(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);
    StackPush(context->offsetStack, context->offset);

    PushNode(AST->left, output, context);
    
    fprintf(output, "push 0\n");
    fprintf(output, "je ifend%u\n", context->ifAmount);

    ASTBypass(AST->right->left, output, context);

    fprintf(output, "ifend%u:\n", context->ifAmount);
    context->ifAmount++;

    if (AST->right->right != nullptr) {
        ASTBypass(AST->right->right, output, context);
    }

    context->offset = StackPop(context->offsetStack);
}

void ProcessFunction(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if (AST->type == TYPE_FUNC) {
        if (!context->ifInFunction) {
            StackPush(context->offsetStack, context->offset);

            fprintf(output, "%s:", AST->data.expression);

            int32_t paramAmount = 0;
            if (AST->left != nullptr) {
                paramAmount += ProcessFuncArguments(AST->left, output, context);
            }

            MakeFunc(AST->data.expression, context, paramAmount);
            context->ifInFunction = 1;
            ASTBypass(AST->right, output, context);

            context->offset = StackPop(context->offsetStack);
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
    }
}

int32_t ExecuteFunction(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    int32_t paramAmount = 0;
    if (AST->left != nullptr) {
        PushNode(AST->left, output, context);
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
        PushNode(AST->left, output, context);
    }

    if (AST->right != nullptr) {
        PushNode(AST->right, output, context);
    }

    if (AST->type == TYPE_CONST) {
        fprintf(output, "push %f\n", AST->data.number);
    }

    if (AST->type == TYPE_VAR) {
        fprintf(output, "push {bx + %d}\n", GetVarOffset(AST->data.expression, context));
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
        fprintf(output, "add\n");
        break;
    case SUB_OP:
        fprintf(output, "sub\n");
        break;
    case DIV_OP:
        fprintf(output, "div\n");
        break;
    case MUL_OP:
        fprintf(output, "mul\n");
        break;
    case POW_OP:
        fprintf(output, "pop ax\n");
        fprintf(output, "pop cx\n");

        fprintf(output, "push cx\n");
        fprintf(output, "pow%u:\n", context->powerAmount);
        
        fprintf(output, "push ax\n");
        fprintf(output, "push 1\n");
        fprintf(output, "sub\n");
        fprintf(output, "pop ax");

        fprintf(output, "push cx\n");
        fprintf(output, "mul\n");

        fprintf(output, "push ax\n");
        fprintf(output, "push 1\n");
        fprintf(output, "ja pow%u\n", context->powerAmount);

        context->powerAmount++;
        break;
    case L_OP:
        fprintf(output, "sub\n");

        fprintf(output, "jb compareg%u\n", context->compareAmount);
        
        fprintf(output, "pop\n");
        fprintf(output, "push 0\n");

        fprintf(output, "compareg%u:\n", context->compareAmount);

        context->compareAmount++;
        break;
    case LEQ_OP:
        fprintf(output, "push 1\n");
        fprintf(output, "add\n");

        fprintf(output, "sub\n");

        fprintf(output, "jb compareg%u\n", context->compareAmount);
        
        fprintf(output, "pop");
        fprintf(output, "push 0");

        fprintf(output, "compareg%u:", context->compareAmount);

        context->compareAmount++;
        break;
    case G_OP:
        fprintf(output, "sub\n");

        fprintf(output, "ja compareg%u\n", context->compareAmount);
        
        fprintf(output, "pop");
        fprintf(output, "push 0");

        fprintf(output, "compareg%u:", context->compareAmount);

        context->compareAmount++;
        break;
    case GEQ_OP:
        fprintf(output, "push 1\n");
        fprintf(output, "add\n");

        fprintf(output, "sub\n");

        fprintf(output, "ja compareg%u\n", context->compareAmount);
        
        fprintf(output, "pop");
        fprintf(output, "push 0");

        fprintf(output, "compareg%u:", context->compareAmount);

        context->compareAmount++;
        break;
    default:
        break;
    }
}

int32_t GetFuncArgAmount(int8_t* name, CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curFunc = 0; curFunc < context->functionsAmount; curFunc++) {
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

    assert(0 && "UNKNOWN VARIABLE");
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
        fprintf(output, "pop {bx + %d}", rememberOffset);
    }

    return paramAmount;
}

void MakeFunc(int8_t* name, CodegenContext* context, int32_t argAmount) {
    assert(name    != nullptr);
    assert(context != nullptr);

    CheckFuncRepetitions(name, context);

    context->functions[context->functionsAmount] = {name, argAmount};
    context->functionsAmount++;
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

    for (uint32_t curFunc = 0; curFunc < context->functionsAmount; curFunc++) {
        if (!strcmp((const char*)context->functions[curFunc].name, (const char*)name)) {
            assert(0 && "TWO VARS WITH EQUAL NAMES");
        }
    }
}

void SkipStrConk(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if ((AST->type == TYPE_OP) &&
        (AST->data.operation == '$')) {
        if (AST->left != nullptr) {
            ASTBypass(AST->left, output, context);
        }

        if (AST->right != nullptr) {
            ASTBypass(AST->right, output, context);
        }
    }
}

void EnterFuncVisibilityZone(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    StackPush(context->offsetStack, context->offset);

    fprintf(output, "push %u\n", context->offset);
    fprintf(output, "push bx\n");
    fprintf(output, "add");
    fprintf(output, "pop bx");
}

void ExitFuncVisibilityZone(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    context->offset = (!context->offsetStack->size) ? 0 : StackPop(context->offsetStack);

    fprintf(output, "push %u\n", context->offset);
    fprintf(output, "pop bx");
}
