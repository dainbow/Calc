#pragma once

#include <fcntl.h>
#include <sys\types.h>
#include <io.h>

#include "Tree.h"

const char G_PATH[] = "graphs/";
const char G_OUTPUT_FORMAT[] = ".png";
const char GRAPH_PATH_FOR_UBUNTU[] = "/mnt/c/Users/suslo/Documents/GitHub/Akinator/";

const int32_t MAX_PATH_FILE_LENGTH = 100;
const int32_t MAX_COMMAND_NAME     = 100;

char* MakeTreeGraph(Tree* tree, const char name[]);
void GenerateOutputName(const char beginningName[], char endName[], const char path[], const char format[]);
