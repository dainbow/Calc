/*  G::= F
    F::= V 'with' {V,}* begin {K | S}* 'lilEnd'
    K::= VasyaSniff | NextSniff | Homyak | Hire 
    S::= (V 'in' DIFF) | (DIFF 'to' V) | DIFF EOL_OP
    E::= STR | T{[+-]T}*
    T::= {-}DEG{[*\]DEG}*
    DEG::= P{^P}
    P::= 'LEFT_ROUND_OP' DIFF 'RIGHT_ROUND_OP' | V 'LEFT_ROUND_OP' DIFF {,DIFF}* 'RIGHT_ROUND_OP' | V | N

    STR::= TYPE_STRING
    V::= TYPE_VAR
    N::= TYPE_CONST
*/

#include "./Frontend/Frontend.h"

int main(int argc, char* argv[]) {
    Text expression = {};
    Text keywords   = {};
    Tokens tokens   = {};
    ProcessMiddleendArguments(argc, argv);

    MakeText(&expression, inputFrontFile);
    MakeText(&keywords, KEYS_FILE);

    AnalyseText(&expression, &tokens, &keywords);
    
    TreeCtor(AST);
    MakeAST(&AST, &tokens);

    DropTreeOnDisk(&AST, outputFrontFile);
    if (isShowFrontTree) MakeTreeGraph(&AST, G_STANDART_NAME);
    
    printf("OK\n");
}

void ProcessMiddleendArguments(int argc, char* argv[]) {
    for (int32_t curArg = 1; curArg < argc; curArg++) {
        if (!strcmp(argv[curArg], "-frontG")) {
            isShowFrontTree = 1;
        }

        if (!strcmp(argv[curArg], "-o")) {
            outputFrontFile = argv[curArg + 1];
        }

        if (!strcmp(argv[curArg], "-i")) {
            inputFrontFile  = argv[curArg + 1];
        }
    }
}
