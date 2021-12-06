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

#include "Calc.h"

int main(int /*argc*/, char* argv[]) {
    Text expression = {};
    Text keywords   = {};
    Tokens tokens   = {};

    MakeText(&expression, argv[1]);
    MakeText(&keywords, KEYS_FILE);

    AnalyseText(&expression, &tokens, &keywords);
    for (uint32_t curToken = 0; tokens.array[curToken].type != 0; curToken++) {
        if ((tokens.array[curToken].type == TYPE_VAR) || (tokens.array[curToken].type == TYPE_STR))
            printf("'%s'[VAR], \n", tokens.array[curToken].data.expression);
        else if (tokens.array[curToken].type == TYPE_CONST)
            printf("'%lf'[CONST], \n", tokens.array[curToken].data.number);
        else if ((tokens.array[curToken].type == TYPE_OP) || (tokens.array[curToken].type == TYPE_UNO))
            printf("'%c{%d}'[OP], \n", tokens.array[curToken].data.operation, tokens.array[curToken].data.operation);
        else if (tokens.array[curToken].type == TYPE_KEYWORD)
            printf("'%c{%d}'[KEY], \n", tokens.array[curToken].data.operation, tokens.array[curToken].data.operation);
    }

    TreeCtor(AST);
    MakeAST(&AST, &tokens);
    
    MakeTreeGraph(&AST, G_STANDART_NAME);
    GenerateCode(&AST);

    printf("OK\n");
    //TreeDtor(&AST);
}
