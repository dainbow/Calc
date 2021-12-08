/*  G::= F
    F::= V 'with' {V,}* begin {K | S}* 'lilEnd'
    K::= VasyaSniff | NextSniff | Homyak | Hire 
    S::= (V 'in' DIFF) | (DIFF 'to' V) | DIFF EOL_OP
    E::= STR | T{[+-]T}*
    T::= {-}DEG{[*\]DEG}*
    DEG::= P{^P}
    P::= 'LEFT_ROUND_OP' DIFF 'RIGHT_ROUND_OP' | V 'LEFT_ROUND_OP' DIFF {,DIFF}* 'RIGHT_ROUND_OP' | V | N

    STR::= TYPE_STRING
    V::= TYPE_VAR
    N::= TYPE_CONST
*/

//! Массивы, глобалки, декомпилятор

#include "Calc.h"

int main(int /*argc*/, char* argv[]) {
    Text expression = {};
    Text keywords   = {};
    Tokens tokens   = {};

    MakeText(&expression, argv[1]);
    MakeText(&keywords, KEYS_FILE);

    AnalyseText(&expression, &tokens, &keywords);
    
    TreeCtor(AST);
    MakeAST(&AST, &tokens);

    DropTreeOnDisk(&AST);
    FrontendMinusOne(&AST);
    MakeTreeGraph(&AST, G_STANDART_NAME);

    char treeName[MAX_FILE_NAME_LENGTH] = "";
    GenerateOutputName(ASM_NAME, treeName, ASM_PATH, ASM_OUTPUT_FORMAT);

    strcat(treeName, TREE_OUTPUT_FORMAT);
    Text treeText = {};

    printf("Scanning tree %s\n", treeName);
    ReadTextFromFile(&treeText, treeName);

    TreeCtor(ASTREADED);

    uint64_t curByte = 0;
    ASTREADED.root   = ReadTreeFromDisk(&treeText, &curByte);
    MakeTreeGraph(&ASTREADED, G_STANDART_NAME);

    GenerateCode(&AST);

    

    printf("OK\n");
}

void DropTreeOnDisk(Tree* AST) {
    assert(AST != nullptr);
    char endName[MAX_FILE_NAME_LENGTH] = "";

    GenerateOutputName(ASM_NAME, endName, ASM_PATH, ASM_OUTPUT_FORMAT);

    strcat(endName, TREE_OUTPUT_FORMAT);
    FILE* output = fopen(endName, "w");

    CompileResult outputB = {};
    outputB.bytesArray    = (int8_t*)calloc(MAX_OUTPUTFILE_LENGTH, sizeof(outputB.bytesArray[0]));

    BypassTreeToDiskDrop(AST->root, &outputB);

    printf("Tree droped in '%s', %llu\n", endName, fwrite(outputB.bytesArray, 
                                      sizeof(outputB.bytesArray[0]), outputB.bytesCount, output));

    fclose(output);
}

void BypassTreeToDiskDrop(Node* node, CompileResult* output) {
    assert(node != nullptr);

    *(NodeDataTypes*)(output->bytesArray + output->bytesCount) = node->type;
    output->bytesCount += sizeof(node->type);

    switch(node->type) {
        case NodeDataTypes::TYPE_CONST:
            *(double*)(output->bytesArray + output->bytesCount) = node->data.number;
            output->bytesCount += sizeof(node->data.number);

            break;
        case NodeDataTypes::TYPE_KEYWORD:
        case NodeDataTypes::TYPE_OP:
        case NodeDataTypes::TYPE_UNO:
            *(int8_t*)(output->bytesArray + output->bytesCount) = node->data.operation;
            output->bytesCount += sizeof(node->data.operation);

            break;
        case NodeDataTypes::TYPE_FUNC:
        case NodeDataTypes::TYPE_STR:
        case NodeDataTypes::TYPE_VAR:
            output->bytesCount += 
            sprintf((char*)output->bytesArray + output->bytesCount, "%s", node->data.expression) + 1;

            break;
        case NodeDataTypes::TYPE_UNKNOWN:
        default:
            assert(FAIL && "UNKNOWN DATA TYPE");
            break;
    }

    *(uint64_t*)(output->bytesArray + output->bytesCount) = (uint64_t)node->left;
    output->bytesCount += sizeof(node->left);

    *(uint64_t*)(output->bytesArray + output->bytesCount) = (uint64_t)node->right;
    output->bytesCount += sizeof(node->right);

    if (node->left != nullptr) {
        BypassTreeToDiskDrop(node->left, output);
    }

    if (node->right != nullptr) {
        BypassTreeToDiskDrop(node->right, output);
    }
}

Node* ReadTreeFromDisk(Text* treeText, uint64_t* curByte) {
    assert(curByte  != nullptr);
    assert(treeText != nullptr);

    Node* retValue = 0;

    printf("curByte is %llu\n", *curByte);
    NodeDataTypes curType = *(NodeDataTypes*)(treeText->buffer + *curByte);
    (*curByte)++;

    Data curData = {};

    switch (curType) {
        case NodeDataTypes::TYPE_CONST:
            curData.number = *(double*)(treeText->buffer + *curByte);
            (*curByte) += sizeof(curData.number);
            break;
        case NodeDataTypes::TYPE_KEYWORD:
        case NodeDataTypes::TYPE_OP:
        case NodeDataTypes::TYPE_UNO:
            curData.operation = *(int8_t*)(treeText->buffer + *curByte);
            (*curByte) += sizeof(curData.operation);
            break;
        case NodeDataTypes::TYPE_FUNC:
        case NodeDataTypes::TYPE_STR:
        case NodeDataTypes::TYPE_VAR:    
            curData.expression = treeText->buffer + *curByte;
            
            while (treeText->buffer[(*curByte)++] != '\0');
            break;
        case NodeDataTypes::TYPE_UNKNOWN:
        default:
            assert(FAIL && "UNKNOWN DATA TYPE");
            break;
    }

    uint64_t leftPtr = *(uint64_t*)(treeText->buffer + *curByte);
    *curByte += sizeof(Node*);

    uint64_t rightPtr = *(uint64_t*)(treeText->buffer + *curByte);
    *curByte += sizeof(Node*);

    retValue = MakeNewNode(curData.operation, curData.number, curData.expression, curType, 0, 0);
    printf("Made node %p with type %d. OP is %d[%c], num is %lf\n", retValue, curType, curData.operation, curData.operation, curData.number);
    
    if ((leftPtr != 0) &&
        (rightPtr != 0)) {
        retValue->left  = ReadTreeFromDisk(treeText, curByte);
        retValue->right = ReadTreeFromDisk(treeText, curByte);
        printf("Both aren't null so them are %p and %p\n", retValue->left, retValue->right);
    }
    else if (leftPtr != 0) {
        retValue->left  = ReadTreeFromDisk(treeText, curByte);
        printf("Leftptr isn't null so it's left is %p\n", retValue->left);
    }
    else if (rightPtr != 0) {
        retValue->right = ReadTreeFromDisk(treeText, curByte);
        printf("Rightptr isn't null so it's right is %p\n", retValue->right);
    }
    

    return retValue;
}
