#include "Latex.h"

char* MakeLatexTitle(Tree* tree, char const* name, DiffContext* diffContext) {
    assert(name != nullptr);

    static char endName[TEX_MAX_NAME_LENGTH] = "";
    GenerateOutputName(name, endName, TEX_PATH, TEX_OUTPUT_FORMAT);

    FILE* output = fopen(endName, "w");
    assert(output != nullptr);

    fprintf(output, "\\documentclass[12pt, a4paper]{article}\n"
                    "\\usepackage[utf8]{inputenc}\n"
                    "\\usepackage[russian]{babel}\n"
                    "\\usepackage[T2A]{fontenc}\n"
                    "\\usepackage{amssymb}\n"
                    "\\usepackage{amsfonts}\n"
                    "\\usepackage{amsmath}\n\n");

    fprintf(output, "\\author{Великий дифференциатор, доцент математических наук}\n"
                    "\\title{Работа всей моей жизни, великое дифференцирование}\n\n");
    
    fprintf(output, "\\begin{document}\n" 
                    "\\maketitle\n");

    fprintf(output, "Приступаю к дифференцированию этого наилегчайшего выражения.\n\n");
    fprintf(output, "$");
    PrintTexTree(1, tree->root, output, diffContext);
    fprintf(output, "$\n\n");
    
    fclose(output);

    return endName;
}

#define PARENTHESIS_IF(branch, action)                                                      \
    if ((node->type       == TYPE_OP)       &&                                              \
        (node-> branch ->type == TYPE_OP)   &&                                              \
        (node->data != LOG_OP)                 &&                                              \
        (node-> branch ->data != LOG_OP)       &&                                              \
        (node->data != DIV_OP)) {                                                              \
        if (CompareOperations((int8_t)node->data, (int8_t)node-> branch ->data) >= 0) {     \
            action;                                                                         \
        }                                                                                   \
    }   

void PrintTexTree(bool bigCheckFlag, Node* node, FILE* output, DiffContext* diffContext) {
    assert(node  != nullptr);
    assert(output != nullptr);

    static int32_t maxWeight = 0;
    maxWeight = (node->weight > maxWeight) ? (node->weight) : (maxWeight);

    if ((bigCheckFlag) &&
        (node->left   != nullptr) &&
        (node->right  != nullptr)) {
        if  (((node->left->weight > (maxWeight / 3 - MAX_NODE_WEIGHT_DIFF))  && 
             (node->left->weight  < (maxWeight / 2 + MAX_NODE_WEIGHT_DIFF))) ||
            ((node->right->weight > (maxWeight / 3 - MAX_NODE_WEIGHT_DIFF))  && 
             (node->right->weight < (maxWeight / 2 + MAX_NODE_WEIGHT_DIFF)))) {
            PrintGreekCharacter(output, diffContext->bigStack->size, diffContext->greekSymbols);
            StackPush(diffContext->bigStack, (StackElem)node);

            return;
        }
    }

    switch(node->type) {
        case TYPE_OP:
            switch(node->data) {
                case DIV_OP:
                    fprintf(output, "\\frac{");
                    break;
                case LOG_OP:
                   fprintf(output, "\\log_{");
                   break;
                default:
                    break; 
            }
            break;
        default:
            break;
    }

    if (node->left != nullptr) {
        PARENTHESIS_IF(left, fprintf(output, "("));
        PrintTexTree(bigCheckFlag, node->left, output, diffContext);
        PARENTHESIS_IF(left, fprintf(output, ")"));
    }

    switch (node->type) {
        case TYPE_OP:
            switch (node->data) {
                case DIV_OP:
                    fprintf(output, "}{");
                    break;
                case LOG_OP:
                    fprintf(output, "}(");
                    break;
                case POW_OP:
                    fprintf(output, "^{");
                    break;
                default:
                    fprintf(output, "%c", node->data);
                    break;
            }
            break;
        case TYPE_UNO:
            switch (node->data) {
                case SIN_OP:
                    fprintf(output, "\\sin(");
                    break;
                case COS_OP:
                    fprintf(output, "\\cos(");
                    break;
                default:
                    break;
            }
            break;
        case TYPE_CONST:
            fprintf(output, "%d", node->data);
            break;
        case TYPE_VAR:
            fprintf(output, "%c", node->data);
            break;
        default:
            break;
    }
    
    if (node->right != nullptr) {
        PARENTHESIS_IF(right, fprintf(output, "("));
        PrintTexTree(bigCheckFlag, node->right, output, diffContext);
        PARENTHESIS_IF(right, fprintf(output, ")"));
    }

    switch (node->type) {
        case TYPE_OP:
            switch (node->data) {
                case DIV_OP:
                    fprintf(output, "}");
                    break;    
                case LOG_OP:
                    fprintf(output, ")");
                    break;
                case POW_OP:
                    fprintf(output, "}");
                    break;
                default:
                    break;
            }
            break;
        case TYPE_UNO:
            fprintf(output, ")");
            break;
        default:
            break;
    }
}

int32_t CompareOperations(int8_t firstOper, int8_t secondOper) {
    int32_t firstValue  = GiveOperationPriority(firstOper);
    int32_t secondValue = GiveOperationPriority(secondOper);

    if (firstValue > secondValue)
        return 1;
    else if ((firstValue < secondValue) || 
            ((firstValue == secondValue) && 
            ((firstOper == ADD_OP) || (firstOper == MUL_OP) || (firstOper == LOG_OP))))
        return -1;
    else
        return 0;
}

int32_t GiveOperationPriority(int8_t operation) {
    switch (operation) {
    case ADD_OP:
    case SUB_OP:
        return 1;
    case MUL_OP:
    case DIV_OP:
        return 2;
    case POW_OP:
        return 3;
    default:
        assert(0 && "INVALID OPERATOR");
        break;
    }

    return 0;
}

void PrintGreekCharacter(FILE* output, uint32_t index, Text* greekSymbols) {
    assert(output       != nullptr);
    assert(greekSymbols != nullptr);

    if (index < greekSymbols->strAmount) {
        fprintf(output, "%s", greekSymbols->strings[index].value);
    }
    else {
        fprintf(output, "%s_{%u}", greekSymbols->strings[index % greekSymbols->strAmount].value, 
                                                         index / greekSymbols->strAmount);
    }
}

void PrintBigNodes(FILE* output, DiffContext* diffContext) {
    assert(output      != nullptr);
    assert(diffContext != nullptr);

    if (diffContext->bigStack->size) {
        fprintf(output, "Где ");
    }

    while (diffContext->bigStack->size) {
        fprintf(output, "$");

        Node* curNode = (Node*)StackPop(diffContext->bigStack);
        PrintGreekCharacter(output, diffContext->bigStack->size, diffContext->greekSymbols);
        fprintf(output, "=");

        PrintTexTree(0, curNode, output, diffContext);
        fprintf(output, "$;\n\n");
    }
}
