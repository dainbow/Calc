#include "Graph.h"
#include "Tree.h"

char* MakeTreeGraph(Tree* tree, const char name[]) {
    assert(tree != nullptr);

    static char endName[MAX_PATH_FILE_LENGTH] = "";

    GenerateOutputName(name, endName, G_PATH, G_OUTPUT_FORMAT);

    FILE* output = fopen(endName, "w");
    assert(output != nullptr);

    fprintf(output, "digraph G{ \n");
    fprintf(output, "\tnewrank=true;\n");

    PrintTreeNodes(tree, tree->root, output);
    fprintf(output, "}\n");
    fclose(output);

    char command[MAX_COMMAND_NAME] = "";
    char cd[MAX_COMMAND_NAME]      = "";
    char del[MAX_COMMAND_NAME]     = "";
    char start[MAX_COMMAND_NAME]   = "";

    sprintf(cd, "ubuntu run cd %s", GRAPH_PATH_FOR_UBUNTU);
    sprintf(command, "ubuntu run dot '%s' -Tpng -O", endName);
    sprintf(del, "ubuntu run rm '%s'", endName);
    sprintf(start, "start %s%s", endName, G_OUTPUT_FORMAT);
    
    system(cd);
    system(command);
    system(del);
    system(start);
    

    return endName;
}

void GenerateOutputName(const char beginningName[], char endName[], const char path[], const char format[]) {
    assert(beginningName != nullptr);
    assert(endName != nullptr);

    uint32_t fileCopyAmount = 0;
    int32_t fileD = 0;

    sprintf(endName, "%s%s(%u)%s", path, beginningName, fileCopyAmount, format);
    while ((fileD = open(endName, O_RDONLY)) != -1) {
        fileCopyAmount++;
        sprintf(endName, "%s%s(%u)%s", path, beginningName, fileCopyAmount, format);

        close(fileD);
    }
    close(fileD);
    sprintf(endName, "%s%s(%u)", path, beginningName, fileCopyAmount);
}
