#pragma once

#include "TXLib.h"

#include "../Tree/TreeDiskUtilities.h"
#include "Grammar.h"

char* inputFrontFile  = 0;
char* outputFrontFile = 0;

bool  isShowFrontTree = 0;

void ProcessMiddleendArguments(int argc, char* argv[]);
