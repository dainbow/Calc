#pragma once

#include "../Tree/Tree.h"
#include "../Utilities/Text.h"

struct BinaryArr {
    int8_t*     bytesArray;
    uint64_t    bytesCount;
};

void DropTreeOnDisk(Tree* AST, char* outputF);
void BypassTreeToDiskDrop(Node* node, BinaryArr* output);
Node* ReadTreeFromDisk(Text* treeText, uint64_t* curByte);
