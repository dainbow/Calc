#ifndef _DIFFER_H_
#define _DIFFER_H_

#include <stdlib.h>

#include "../Tree/Tree.h"
#include "../Tree/Graph.h"
#include "../Utilities/Text.h"
#include "../Utilities/Stack.h"

#define MAKLOREN 0

struct Context {
    Node* node;
    Node** prevNode;
};

const int32_t MAX_TRASH_SIZE       = 100;
const int32_t MAX_NODE_DATA_LENGTH = 20;

const char TEX_NAME[]              = "latex";
const char MATH_PHRASES[]          = "MathPhrases.txt";
const char GREEK_SYMBOLS[]         = "GreekSymbols.txt";

void ScanBase(Text* input, Stack* stack);
bool ReadTreeFromFile(Tree* tree, const char* inputFile);

int32_t ProcessNodeData(StackElem rawData, NodeDataTypes* type);
Node* MakeTreeFromStack(Stack* nodesStack);

Node* Differentiate (Node* root);
Node* Copy (Node* root);
Node* MakeFactor(int32_t factor);
Node* OptimisationAfterDiff(Node* node);

int32_t FoldConst(Node* node);
int32_t CutEqualNodes(Context context);
int32_t CutNullNodes(Context context);
int32_t CutMinusOneNodes(Node* node);

int32_t CheckForVars(Node* node);

void SubstituteVars(Node* node, int8_t* varData, int32_t value);

bool CheckDoubleEquality(double first, double second);

#endif
