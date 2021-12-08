#pragma once

#include "Grammar.h"
#include "Codegen.h"

void DropTreeOnDisk(Tree* AST);
void BypassTreeToDiskDrop(Node* node, CompileResult* output);
Node* ReadTreeFromDisk(Text* treeText, uint64_t* curByte);
