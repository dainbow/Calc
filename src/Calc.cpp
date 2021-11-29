/*  G::= E'$'
    E::= T{[+-]T} *
    T::= P{[*\]P} *
    P::= '('E')' | N
    N::= [0]-9 +
*/

#include "calc.h"

int main() {
    Text expression = {};
    Text keywords   = {};

    Tokens tokens = {};

    MakeText(&expression, CALC_FILE);
    MakeText(&keywords, KEYS_FILE);

    AnalyseText(&expression, &tokens, &keywords);
    for (uint32_t curToken = 0; tokens.array[curToken].type != 0; curToken++) {
        if (tokens.array[curToken].type == TYPE_VAR)
            printf("'%s'[VAR], ", tokens.array[curToken].data.expression);
        else if (tokens.array[curToken].type == TYPE_CONST)
            printf("'%d'[CONST], ", tokens.array[curToken].data.number);
        else if ((tokens.array[curToken].type == TYPE_OP) || (tokens.array[curToken].type == TYPE_UNO))
            printf("'%c{%d}'[OP], ", tokens.array[curToken].data.operation, tokens.array[curToken].data.number);
    }

    TreeCtor(AST);

    AST.root = GetG(&tokens.array);
    MakeTreeGraph(&AST, G_STANDART_NAME);

    printf("OK\n");
    //TreeDtor(&AST);
}

Node* GetG(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = GetE(pointer);
    Require(((**pointer).type != TYPE_UNO) || ((**pointer).data.operation != '$'));

    return retValue;
}

Node* GetE(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = GetT(pointer);
    
    while((((**pointer).data.operation == '+') || ((**pointer).data.operation == '-')) &&
           ((**pointer).type == TYPE_OP)) {
        Node*  operationNode = *pointer;
        (*pointer)++;

        Node* bufValue = GetT(pointer);

        operationNode->left  = retValue;
        operationNode->right = bufValue;

        retValue = operationNode;
    }

    return retValue;
}

Node* GetT(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = GetP(pointer);

    while((((**pointer).data.operation == '*') || ((**pointer).data.operation == '/')) &&
           ((**pointer).type = TYPE_OP)) {
        Node* operationNode = *pointer;
        (*pointer)++;

        Node* bufValue = GetP(pointer);

        operationNode->left  = retValue;
        operationNode->right = bufValue;

        retValue = operationNode;
    }

    return retValue;
}

Node* GetP(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);
    Node* retValue = 0;
    
    if (((**pointer).type == TYPE_UNO) && ((**pointer).data.operation == '(')) {
        (*pointer)++;

        retValue = GetE(pointer);

        printf("GetP looking at %c[%d] with type %d\n", (**pointer).data.operation, (**pointer).data.number, (**pointer).type);
        Require(((**pointer).type != TYPE_UNO) || ((**pointer).data.operation != ')'));
    }
    else {
        retValue = GetN(pointer);
    }

    assert(retValue != nullptr);
    return retValue;
}

Node* GetN(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = *pointer;
    (*pointer)++;

    if ((*retValue).type != TYPE_CONST) {
        assert(FAIL && "INVALID RETURN FOR GET N FUNCTION");
    } 

    return retValue;
}

void SkipSpaces(int8_t** pointer) {
    assert(pointer != nullptr);

    if (isspace(**pointer)) {
        (*pointer)++;
    }
}

void AnalyseText(Text* text, Tokens* tokens, Text* keywords) {
    assert(text != nullptr);

    tokens->array    = (Node*)calloc(text->bufSize, sizeof(tokens->array[0]));
    tokens->database = (int8_t*)calloc(text->bufSize, sizeof(tokens->database[0]));

    int8_t* beginning = text->buffer;

    int64_t tokensCounter   = 0;
    int64_t databaseCounter = 0;
    
    for (int8_t* curChar = text->buffer; (curChar - beginning < text->bufSize) && *curChar != '\0'; curChar++) {
        SkipSpaces(&curChar);
        
        if ((*curChar >= 'A') && (*curChar <= 'z')) {
            int8_t* bufferRemember = tokens->database;
            
            while (((*curChar >= 'A') && (*curChar <= 'z')) ||
                   ((*curChar >= '0') && (*curChar <= '9'))) {
                tokens->database[databaseCounter] = *curChar;

                databaseCounter++;
                curChar++;
            }
            curChar--;
            tokens->database[databaseCounter] = '\0';
            databaseCounter++;
        
            int8_t keywordNum = 0;
            if ((keywordNum = IsKeyword(bufferRemember, keywords))) {
                tokens->array[tokensCounter].type = TYPE_OP;
                tokens->array[tokensCounter].data.operation = keywordNum;

                databaseCounter = bufferRemember - tokens->database;
            }
            else {
                tokens->array[tokensCounter] = {bufferRemember, TYPE_VAR};
            }

            tokensCounter++;
        }
        else if ((*curChar >= '0') && (*curChar <= '9')) {
            int32_t value = 0;
            
            while ((*curChar >= '0') && (*curChar <= '9')) {
                value = 10*value + (*curChar - '0');
                curChar++;
            }
            curChar--;

            tokens->array[tokensCounter].type               = TYPE_CONST;
            tokens->array[tokensCounter].data.number        = value;

            tokensCounter++;
        }
        else {
            switch (*curChar) {
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case '$':
                tokens->array[tokensCounter].type           = TYPE_UNO;
                tokens->array[tokensCounter].data.operation = *curChar;
                break;
            case '+':
            case '-':
            case '*':
            case '/':
            case '^':
                tokens->array[tokensCounter].type           = TYPE_OP;
                tokens->array[tokensCounter].data.operation = *curChar;
                break;
            default:
                printf("%c[%d]\n", *curChar,*curChar);
                assert(FAIL && "Invalid operator");
                break;
            }

            tokensCounter++;
        }
    }

    printf("TOKEN'S AMOUNT IS %d\n", tokensCounter + 1);
    //tokens->array    = (Node*)realloc(tokens->array, tokensCounter + 1);
    //tokens->database = (int8_t*)realloc(tokens->database, databaseCounter + 1);
}

int8_t IsKeyword(int8_t* buffer, Text* keywords) {
    assert(buffer   != nullptr);
    assert(keywords != nullptr);

    for (uint32_t curStr = 0; curStr < keywords->strAmount; curStr++) {
        if (!MyStrCmp(buffer, keywords->strings[curStr].value)) {
            return (int8_t)(curStr + 1);
        }
    }

    return 0;
}
