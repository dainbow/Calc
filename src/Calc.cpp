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

//! Массивы, глобалки, декомпилятор

#include "Calc.h"

int main(int /*argc*/, char* argv[]) {
    Text expression = {};
    Text keywords   = {};
    Tokens tokens   = {};

    MakeText(&expression, argv[1]);
    MakeText(&keywords, KEYS_FILE);

    AnalyseText(&expression, &tokens, &keywords);
    
    TreeCtor(AST);
    MakeAST(&AST, &tokens);
    FrontendMinusOne(&AST);

    MakeTreeGraph(&AST, G_STANDART_NAME);
    GenerateCode(&AST);

    printf("OK\n");
}
