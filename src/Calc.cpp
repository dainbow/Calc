/*  G::= F
    F::= V 'with' {V,}* begin {K | S}* 'lilEnd'
    K::= VasyaSniff | NextSniff | Homyak | Hire 
    S::= (V 'in' E) | (E 'to' V) | E EOL_OP
    E::= T{[+-]T}*
    T::= {-}DEG{[*\]DEG}*
    DEG::= P{^P}
    P::= 'LEFT_ROUND_OP' E 'RIGHT_ROUND_OP' | V 'LEFT_ROUND_OP' E {,E}* 'RIGHT_ROUND_OP' | V | N
    V::= TYPE_VAR
    N::= TYPE_CONST
*/

#include "Grammar.h"

int main() {
    Text expression = {};
    Text keywords   = {};
    Tokens tokens   = {};

    MakeText(&expression, CALC_FILE);
    MakeText(&keywords, KEYS_FILE);

    AnalyseText(&expression, &tokens, &keywords);
    for (uint32_t curToken = 0; tokens.array[curToken].type != 0; curToken++) {
        if (tokens.array[curToken].type == TYPE_VAR)
            printf("'%s'[VAR], \n", tokens.array[curToken].data.expression);
        else if (tokens.array[curToken].type == TYPE_CONST)
            printf("'%d'[CONST], \n", tokens.array[curToken].data.number);
        else if ((tokens.array[curToken].type == TYPE_OP) || (tokens.array[curToken].type == TYPE_UNO))
            printf("'%c{%d}'[OP], \n", tokens.array[curToken].data.operation, tokens.array[curToken].data.number);
        else if (tokens.array[curToken].type == TYPE_KEYWORD)
            printf("'%c{%d}'[KEY], \n", tokens.array[curToken].data.operation, tokens.array[curToken].data.number);
    }

    TreeCtor(AST);
    MakeAST(&AST, &tokens);
    
    MakeTreeGraph(&AST, G_STANDART_NAME);

    printf("OK\n");
    //TreeDtor(&AST);
}
