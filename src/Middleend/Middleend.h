#pragma once

#include "../Tree/TreeDiskUtilities.h"
#include "../Backend/Backend.h"
#include "../Tree/Differ.h"
#include "../Tree/Tree.h"

void OptimizeAST(Node* node);
void RecursionDiff(Context context);
void ProcessMiddleEndArgs(int argc, char* argv[]);
