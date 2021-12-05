#pragma once

#include <windows.h>
#include <stdlib.h>

#include "Tree.h"
#include "Graph.h"
#include "Text.h"
#include "Stack.h"
#include "Latex.h"

#define MAKLOREN 0

struct Context {
    Node* node;
    Node** prevNode;
};

const int32_t FAIL                 = 0;
const int32_t MAX_TRASH_SIZE       = 100;
const int32_t MAX_NODE_DATA_LENGTH = 20;

const char TEX_NAME[]              = "latex";

const char MATH_PHRASES[]          = "MathPhrases.txt";
const char GREEK_SYMBOLS[]         = "GreekSymbols.txt";

void ScanBase(Text* input, Stack* stack);
bool ReadTreeFromFile(Tree* tree, const char* inputFile);

int32_t ProcessNodeData(StackElem rawData, NodeDataTypes* type);
Node* MakeTreeFromStack(Stack* nodesStack);

int32_t Convert1251ToUtf8 (const char* input, char* output);

Node* Differentiate (Node* root);
Node* Copy (Node* root);
Node* MakeFactor(int32_t factor);
Node* OptimisationAfterDiff(Node* node);

int32_t FoldConst(Node* node);
int32_t CutEqualNodes(Context context);
int32_t CutNullNodes(Context context);
int32_t CutMinusOneNodes(Node* node);

int32_t CheckForVars(Node* node);

FILE* StartTex(Tree* tree, char** outputName, DiffContext* diffContext);
void LogDiffProcessToTex(Node* curNode, Node* diffNode, FILE* output, DiffContext* diffContext);
void StopTex(FILE* output, char* outputName, Node* beginNode, Node* node, DiffContext* diffContext);

void SubstituteVars(Node* node, int8_t* varData, int32_t value);

void MakeMakloren(FILE* output, Node* node, int32_t accuracy, int8_t* variable, DiffContext* diffContext);
