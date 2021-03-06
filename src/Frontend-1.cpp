#include "./Frontend/Frontend-1.h"

int main(int argc, char* argv[]) {
    __uint128_t value = 99999999999999;
    printf("%I128d\n", value);
    
    ProcessFrontendminus1Arguments(argc, argv);

    char treeName[MAX_FILE_NAME_LENGTH] = "";
    if (inputReversedName == nullptr) 
        GenerateOutputName(ASM_NAME, treeName, ASM_PATH, ASM_OUTPUT_FORMAT);
    
    else 
        strcat(treeName, inputReversedName);
    strcat(treeName, TREE_OUTPUT_FORMAT);
    printf("Reading %s\n", treeName);

    Text treeText = {};

    ReadTextFromFile(&treeText, treeName);

    TreeCtor(ASTREADED);

    uint64_t curByte = 0;
    ASTREADED.root   = ReadTreeFromDisk(&treeText, &curByte);

    if (isShowReversedTree) MakeTreeGraph(&ASTREADED, G_STANDART_NAME);

    FrontendMinusOne(&ASTREADED);
}

void ProcessFrontendminus1Arguments(int argc, char* argv[]) {
    for (int32_t curArg = 1; curArg < argc; curArg++) {
        if (!strcmp(argv[curArg], "-minoneG")) {
            isShowReversedTree = 1;
        }

        if (!strcmp(argv[curArg], "-o")) {
            outputReversedName = argv[curArg + 1];
        }

        if (!strcmp(argv[curArg], "-i")) {
            inputReversedName  = argv[curArg + 1];
        }
    }
}

void FrontendMinusOne(Tree* AST) {
    assert(AST != nullptr);

    char endName[MAX_FILE_NAME_LENGTH] = "";
    
    if (outputReversedName != nullptr) {
        strcat(endName, outputReversedName);
    }
    else {
        GenerateOutputName(ASM_NAME, endName, ASM_PATH, ASM_OUTPUT_FORMAT);
    }

    strcat(endName, ASM_REVERSED_FORMAT);
    FILE* outputReversed = (outputReversedName) ? 
                            fopen(endName, "w") : (FILE*)calloc(1, sizeof(FILE*));

    uint32_t depth = 0;
    PrintAST(AST->root, outputReversed, depth);
    fprintf(outputReversed, "bigEnd\n");

    printf("Frontend^-1 successfully decompiled tree and wrote it to %s\n", endName);
    fclose(outputReversed);
}

void PrintAST(Node* node, FILE* output, uint32_t depth) {
    assert(node    != nullptr);
    assert(output != nullptr);

    switch (node->type) {
        case NodeDataTypes::TYPE_FUNC:
            PrintFunction(node, output, depth);
            break;
        case NodeDataTypes::TYPE_KEYWORD:
            PrintKeyword(node, output, depth);
            break;
        case NodeDataTypes::TYPE_ARR:
            PrintArray(node, output, depth);
            break;
        case NodeDataTypes::TYPE_UNO: 
        case NodeDataTypes::TYPE_OP:
            PrintOperationMinus1(node, output, depth);
            break;
        case NodeDataTypes::TYPE_STR:
            PrintString(node, output);
            break;
        case NodeDataTypes::TYPE_VAR:
            fprintf(output, "%s ", node->data.expression);
            break;
        case NodeDataTypes::TYPE_CONST:
            fprintf(output, "%lf ", node->data.number);
            break;
        case NodeDataTypes::TYPE_UNKNOWN:
            assert(FAIL && "TYPE IS UNKNOWN");
            break;
        default:
            assert(FAIL && "UNKNOWN TYPE");
            break;
    }
}

void PrintArray(Node* node, FILE* output, uint32_t depth) {
    assert(node   != nullptr);
    assert(output != nullptr);

    fprintf(output, "%s[", node->data.expression);

    PrintAST(node->left, output, depth);

    fprintf(output, "] ");
}

void PrintString(Node* node, FILE* output) {
    assert(node   != nullptr);
    assert(output != nullptr);

    fprintf(output, "\"%s\" ", node->data.expression);
}

#define BRACKETS_IF(DISTONATION, BRACKETS_TYPE)                             \
    if ((node-> DISTONATION ->type == NodeDataTypes::TYPE_OP) &&                           \
        (GetOperationPriority(node-> DISTONATION-> data.operation) <        \
        GetOperationPriority(node->data.operation))) {                      \
        fprintf(output, BRACKETS_TYPE);                                     \
    }

void PrintOperationMinus1(Node* node, FILE* output, uint32_t depth) {
    assert(node   != nullptr);
    assert(output != nullptr);

    if (node->left != nullptr) {
        BRACKETS_IF(left, "(")

        PrintAST(node->left, output, depth);

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
            if  (!((node->left->type == NodeDataTypes::TYPE_KEYWORD)       &&
                ((node->left->data.operation == KEY_FOR)    ||
                 (node->left->data.operation == KEY_CRY)    ||
                 (node->left->data.operation == KEY_GOBBLE) ||
                 (node->left->data.operation == KEY_IF)     ||
                 (node->left->data.operation == KEY_SHOW))))
                fprintf(output, "$\n");
            break;
        default:
            assert(FAIL && "UNKNOWN OPERATOR");
            break;
    }

    if (node->right != nullptr) {
        BRACKETS_IF(right, "(")

        PrintAST(node->right, output, depth);

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

void PrintFunction(Node* node, FILE* output, uint32_t depth) {
    assert(node   != nullptr);
    assert(output != nullptr);

    if ((node->left  == nullptr) &&
        (node->right == nullptr)) {
        fprintf(output, "%s() ", node->data.expression);
    }
    else if (node->left != nullptr) {
        if ((node->left->type           == NodeDataTypes::TYPE_KEYWORD) &&
            (node->left->data.operation == KEY_WITH)) {
            fprintf(output, "%s with ", node->data.expression);
            
            PrintAST(node->left->left, output, depth);
            if (node->left->right != nullptr) {
                fprintf(output, ", ");
                PrintAST(node->left->right, output, depth);
            }

            fprintf(output, "begin \n");
            
            PrintAST(node->right, output, depth + 1);
            fprintf(output, "\n");
        }
        else {
            PrintIndent(depth, output);
            fprintf(output, "%s( ", node->data.expression);
            
            PrintAST(node->left,  output, depth);
            if (node->right != nullptr) {
                fprintf(output, ", ");
                PrintAST(node->right, output, depth);
            }

            fprintf(output, ")");
        }
    }
    else {
        fprintf(output, "%s ", node->data.expression);  
        fprintf(output, "begin \n");

        PrintAST(node->right, output, depth + 1);

        fprintf(output, "\n");
    }
}

void PrintKeyword(Node* node, FILE* output, uint32_t depth) {
    assert(node   != nullptr);
    assert(output != nullptr);

    switch(node->data.operation) {
        case KEY_IF:
            PrintIndent(depth, output);
            fprintf(output, "vasyaSniff ");

            PrintAST(node->left, output, depth);

            fprintf(output, "begin\n");

            PrintAST(node->right->left, output, depth + 1);

            PrintIndent(depth, output);
            fprintf(output, "lilEnd\n\n");
            
            if (node->right->right != nullptr) {
                PrintIndent(depth, output);
                fprintf(output, "someoneSniff begin\n");

                PrintAST(node->right->right, output, depth + 1);

                PrintIndent(depth, output);
                fprintf(output, "lilEnd\n\n");
            }
            break;
        case KEY_BEGIN:
            fprintf(output, "begin\n");

            if (node->left != nullptr)
                PrintAST(node->left, output, depth + 1);

            if (node->right != nullptr)
                PrintAST(node->right, output, depth + 1);
            break;
        case KEY_CRY:
            PrintIndent(depth, output);
            fprintf(output, "cry ");

            if (node->left != nullptr)
                PrintAST(node->left, output, depth);
            if (node->right != nullptr) { 
                fprintf(output, ", ");
                PrintAST(node->right, output, depth);
            }

            fprintf(output, "dot\n");
            break;
        case KEY_DIFF:
            fprintf(output, "diff ");
            
            if (node->left) {
                PrintAST(node->left, output, depth);
            }

            if (node->right) {
                PrintAST(node->right, output, depth);
            }
            break;
        case KEY_DOT:
            fprintf(output, "dot\n");
            break;
        case KEY_ELSE:
            assert(FAIL && "MAKE ELSE");
            break;
        case KEY_ELSEIF:
            assert(FAIL && "MAKE ELSEIF");
            break;
        case KEY_END:
            assert(FAIL && "MAKE BIGEND");
            break;
        case KEY_FOR:
            PrintIndent(depth, output);
            fprintf(output, "hire ");

            PrintAST(node->left, output, depth);
            PrintAST(node->right, output, depth);

            PrintIndent(depth, output);
            fprintf(output, "lilEnd\n\n");
            break;
        case KEY_FROM:
            PrintAST(node->left, output, depth);

            fprintf(output, "from ");
            
            PrintAST(node->right, output, depth);
            break;
        case KEY_SHOW:
            PrintIndent(depth, output);
            fprintf(output, "show ");

            if (node->left != nullptr)
                PrintAST(node->left, output, depth);
            if (node->right != nullptr) {
                fprintf(output, ", ");
                PrintAST(node->right, output, depth);
            }

            fprintf(output, "dot\n");
            break;
        case KEY_GOBBLE:
            PrintIndent(depth, output);
            fprintf(output, "gobble ");

            if (node->left != nullptr)
                PrintAST(node->left, output, depth);
            if (node->right != nullptr) {
                fprintf(output, ", ");
                PrintAST(node->right, output, depth);
            }

            fprintf(output, "dot\n");
            break;
        case KEY_WITH:
            if (node->left != nullptr)
                PrintAST(node->left, output, depth);

            fprintf(output, "with ");
            
            if (node->right != nullptr)
                PrintAST(node->right, output, depth);
            break;
        case KEY_LILEND:
            if (node->left != nullptr)
                PrintAST(node->left, output, depth);

            fprintf(output, "lilEnd\n\n");
            
            if (node->right != nullptr)
                PrintAST(node->right, output, depth);
            break;
        case KEY_RETURN:
            PrintIndent(depth, output);
            fprintf(output, "spit ");
            
            if (node->left != nullptr)
                PrintAST(node->left, output, depth);

            if (node->right != nullptr)
                PrintAST(node->right, output, depth);
            break;
        case KEY_TO:
            if (node->left != nullptr)
                PrintAST(node->left, output, depth);

            fprintf(output, "to ");
            
            if (node->right != nullptr)
                PrintAST(node->right, output, depth);
            break;
        case KEY_IN:
            PrintIndent(depth, output);
            if (node->left != nullptr)
                PrintAST(node->left, output, depth);

            fprintf(output, "in ");
            
            if (node->right != nullptr)
                PrintAST(node->right, output, depth);
            break;   
        case KEY_WHILE:
            PrintIndent(depth, output);
            fprintf(output, "homyak ");

            PrintAST(node->left, output, depth);

            fprintf(output, "begin\n");
            PrintAST(node->right, output, depth + 1);
            break;
        default:
            fprintf(stderr, "UNKNOWN KEY %d\n", node->data.operation);
            abort();
            break;
    }
}

void PrintIndent(uint32_t depth, FILE* output) {
    assert(output != nullptr);

    for (uint32_t curIndent = 0; curIndent < depth; curIndent++) {
        fprintf(output, "\t");
    }
}
