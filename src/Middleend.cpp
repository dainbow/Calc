#include "./Middleend/Middleend.h"

char* inputFile  = 0;
char* outputFile = 0;

bool isShowOptTree = 0;

int main(int argc, char* argv[]) {
    ProcessMiddleEndArgs(argc, argv);

    char inputFileName[MAX_FILE_NAME_LENGTH] = "";
    if (!inputFile) {
        char generatedInputFileName[MAX_FILE_NAME_LENGTH] = "";
        GenerateOutputName(ASM_NAME, generatedInputFileName, ASM_PATH, ASM_OUTPUT_FORMAT);

        strcat(inputFileName, generatedInputFileName);
    } 
    else {
        strcat(inputFileName, inputFile);
    }
    strcat(inputFile, TREE_OUTPUT_FORMAT);
    printf("[Mid] Reading file %s\n", inputFile);

    Text treeText = {};
    ReadTextFromFile(&treeText, inputFile);

    TreeCtor(ASTREADED);

    uint64_t curByte = 0;

    printf("Bufsize if %llu\n", treeText.bufSize);
    ASTREADED.root   = ReadTreeFromDisk(&treeText, &curByte);

    if (isShowOptTree) {
        MakeTreeGraph(&ASTREADED, G_STANDART_NAME);
    }

    OptimizeAST(ASTREADED.root);

    DropTreeOnDisk(&ASTREADED, outputFile);
    TreeDtor(&ASTREADED);

    printf("[Mid] Successfully ended tree optimisation\n");
}

void OptimizeAST(Node* node) {
    assert(node != nullptr);

    Context context = {node, &node};
    RecursionDiff(context);
    
    TreeCtor(tempTree);
    tempTree.root = node;

    int32_t optimisationCounter = 0;
    do {
        optimisationCounter  = 0;

        optimisationCounter += FoldConst(node);
        optimisationCounter += CutEqualNodes({node, &node});
        optimisationCounter += CutNullNodes({node, &node});
        optimisationCounter += CutMinusOneNodes(node);
    } while (optimisationCounter);
}

void RecursionDiff(Context context) {
    assert(context.node     != nullptr);
    assert(context.prevNode != nullptr);

    if ((context.node->type             == NodeDataTypes::TYPE_KEYWORD) &&
        (context.node->data.operation   == KEY_DIFF)) {
        if ((**context.prevNode).left == context.node) {
            (**context.prevNode).left = Differentiate(context.node->left);
        }

        if ((**context.prevNode).right == context.node) {
            (**context.prevNode).right = Differentiate(context.node->left);
        }
    }

    if (context.node->left != nullptr)
        RecursionDiff({context.node->left,  &context.node});
    
    if (context.node->right != nullptr)
        RecursionDiff({context.node->right, &context.node});
}

void ProcessMiddleEndArgs(int argc, char* argv[]) {
    assert(argv != nullptr);

    for (int curArg = 1; curArg < argc; curArg++) {
        if (!strcmp(argv[curArg], "-i")) {
            inputFile = argv[curArg + 1];
        }

        if (!strcmp(argv[curArg], "-o")) {
            outputFile = argv[curArg + 1];
        }

        if (!strcmp(argv[curArg], "middleG")) {
            isShowOptTree = 1;
        }
    }
}
