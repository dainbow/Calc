#include "TreeDiskUtilities.h"

#include "../Backend/Backend.h"

void DropTreeOnDisk(Tree* AST, char* outputF) {
    assert(AST != nullptr);
    char endName[MAX_FILE_NAME_LENGTH] = "";

    if (outputF)
        strcat(endName, outputF);
    else
        GenerateOutputName(ASM_NAME, endName, ASM_PATH, ASM_OUTPUT_FORMAT);

    strcat(endName, TREE_OUTPUT_FORMAT);
    FILE* output = fopen(endName, "w");

    BinaryArr outputB = {};
    outputB.bytesArray    = (int8_t*)calloc(MAX_OUTPUTFILE_LENGTH, sizeof(outputB.bytesArray[0]));

    BypassTreeToDiskDrop(AST->root, &outputB);

    printf("Tree droped in '%s', size if %llu\n", endName, fwrite(outputB.bytesArray, 
                                      sizeof(outputB.bytesArray[0]), outputB.bytesCount, output));

    fclose(output);
}

void BypassTreeToDiskDrop(Node* node, BinaryArr* output) {
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
        case NodeDataTypes::TYPE_ARR:
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
        case NodeDataTypes::TYPE_ARR:
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
    printf("Made node with type %d\n", retValue->type);

    if ((leftPtr != 0) &&
        (rightPtr != 0)) {
        retValue->left  = ReadTreeFromDisk(treeText, curByte);
        retValue->right = ReadTreeFromDisk(treeText, curByte);
    }
    else if (leftPtr != 0) {
        retValue->left  = ReadTreeFromDisk(treeText, curByte);
    }
    else if (rightPtr != 0) {
        retValue->right = ReadTreeFromDisk(treeText, curByte);
    }
    
    assert(retValue != nullptr);
    return retValue;
}
