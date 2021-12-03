#include "Codegen.h"

void GenerateCode(Tree* AST) {
    assert(AST != nullptr);
    char* endName = 0;

    GenerateOutputName(ASM_NAME, endName, ASM_PATH, ASM_OUTPUT_FORMAT);

    FILE* output = fopen(endName, "w");

    CodegenContext context = {};
    ASTBypass(AST->root, output, &context);

    fclose(output);
}

void ASTBypass(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    ProcessFunction(AST, output, context);
}

void ProcessFunction(Node* AST, FILE* output, CodegenContext* context) {
    assert(AST     != nullptr);
    assert(output  != nullptr);
    assert(context != nullptr);

    if (AST->type == TYPE_FUNC) {
        if (!context->ifInFunction) {
            fprintf(output, "%s:", AST->data.expression);

            int32_t paramAmount = 0;
            if (AST->left != nullptr) {
                paramAmount += ProcessFuncArguments(AST->left, output, context);
            }

            MakeFunc(AST->data.expression, context, paramAmount);
            context->ifInFunction = 1;
            ASTBypass(AST->right, output, context);
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

        fprintf(output, "call %s\n", AST->data.expression);
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
        fprintf(output, "push %f\n");
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
        fprintf(output, "pow%d:\n", context->powerAmount);
        
        fprintf(output, "push ax\n");
        fprintf(output, "push 1\n");
        fprintf(output, "sub\n");
        fprintf(output, "pop ax");

        fprintf(output, "push cx\n");
        fprintf(output, "mul\n");

        fprintf(output, "push ax\n");
        fprintf(output, "push 1\n");
        fprintf(output, "ja pow%d\n", context->powerAmount);

        context->powerAmount++;
        break;
    case L_OP:
        fprintf(output, "sub\n");

        fprintf(output, "jb compareg%d\n", context->compareAmount);
        
        fprintf(output, "pop\n");
        fprintf(output, "push 0\n");

        fprintf(output, "compareg%d:\n", context->compareAmount);

        context->compareAmount++;
        break;
    case LEQ_OP:
        fprintf(output, "push 1\n");
        fprintf(output, "add\n");

        fprintf(output, "sub\n");

        fprintf(output, "jb compareg%d\n", context->compareAmount);
        
        fprintf(output, "pop");
        fprintf(output, "push 0");

        fprintf(output, "compareg%d:", context->compareAmount);

        context->compareAmount++;
        break;
    case G_OP:
        fprintf(output, "sub\n");

        fprintf(output, "ja compareg%d\n", context->compareAmount);
        
        fprintf(output, "pop");
        fprintf(output, "push 0");

        fprintf(output, "compareg%d:", context->compareAmount);

        context->compareAmount++;
        break;
    case GEQ_OP:
        fprintf(output, "push 1\n");
        fprintf(output, "add\n");

        fprintf(output, "sub\n");

        fprintf(output, "ja compareg%d\n", context->compareAmount);
        
        fprintf(output, "pop");
        fprintf(output, "push 0");

        fprintf(output, "compareg%d:", context->compareAmount);

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

void MakeLocalVar(int8_t* name, CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);
    CheckVarRepetitions(name, context);

    context->variables[context->offset] = {name, context->offset};
    context->offset++; 
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

void CheckVarRepetitions(int8_t* name, CodegenContext* context) {
    assert(name    != nullptr);
    assert(context != nullptr);

    for (uint32_t curVar = 0; curVar < context->offset; curVar++) {
        if (!strcmp((const char*)context->variables[curVar].name, (const char*)name)) {
            assert(0 && "TWO VARS WITH EQUAL NAMES");
        }
    }
}
