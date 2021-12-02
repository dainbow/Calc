/*  G::= S'$'
    E::= T{[+-]T} *
    T::= P{[*\]P} *
    P::= '('E')' | V '('E')' | V  | N
    S::=  (V 'in' E) | (E 'to' V) | E  
    V::= TYPE_VAR
    N::= TYPE_CONST
*/

#include "calc.h"

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

    AST.root = GetG(&tokens.array);
    Node* bottom = AST.root;

    while((tokens.array->type != TYPE_KEYWORD) || (tokens.array->data.operation != KEY_END)) {
        printf("WE'RE ON ");
        if (tokens.array->type == TYPE_VAR)
            printf("'%s'[VAR], \n", tokens.array->data.expression);
        else if (tokens.array->type == TYPE_CONST)
            printf("'%d'[CONST], \n", tokens.array->data.number);
        else if ((tokens.array->type == TYPE_OP) || (tokens.array->type == TYPE_UNO))
            printf("'%c{%d}'[OP], \n", tokens.array->data.operation, tokens.array->data.number);
        else if (tokens.array->type == TYPE_KEYWORD)
            printf("'%c{%d}'[KEY], \n", tokens.array->data.operation, tokens.array->data.number);

        bottom->right = GetG(&tokens.array);
        bottom = bottom->right;
    }
    MakeTreeGraph(&AST, G_STANDART_NAME);

    printf("OK\n");
    //TreeDtor(&AST);
}

Node* GetG(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = GetS(pointer);
    
    printf("type is %d; operation is %c\n", (**pointer).type, (**pointer).data.operation);
    
    if (((**pointer).type == TYPE_UNO) && ((**pointer).data.operation == '$')) {
        (*pointer)->left = retValue;
        retValue = *pointer;

        (*pointer)++;
    }
    else {
        assert("SYNTAX ERROR, EOL NOT FOUND");
    }

    return retValue;
}

Node* GetS(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue       = 0;

    if (((**pointer).type == TYPE_VAR) &&
        ((*(*pointer + 1)).type == TYPE_KEYWORD) && ((*(*pointer + 1)).data.operation == KEY_IN)) {
        retValue       = *pointer + 1;
        retValue->left = GetV(pointer);
        
        (*pointer)++;
        retValue->right = GetE(pointer);
    }
    else {
        Node* expression = GetE(pointer);

        if (((**pointer).type == TYPE_KEYWORD) && ((**pointer).data.operation == KEY_TO)) {
            retValue = *pointer;
            (*pointer)++;

            retValue->left  = expression;
            retValue->right = GetV(pointer);
        }
        else {
            retValue = expression;
        }
    }

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
    else if ((**pointer).type == TYPE_VAR) {
        retValue = GetV(pointer);

        if (((**pointer).type == TYPE_UNO) && ((**pointer).data.operation == '(')) {
            (*pointer)++;
            retValue->type = TYPE_FUNC;

            if (((**pointer).type != TYPE_UNO) || ((**pointer).data.operation != ')'))
                retValue->right = GetE(pointer);
            Require(((**pointer).type != TYPE_UNO) || ((**pointer).data.operation != ')'));
        }
    }
    else {
        retValue = GetN(pointer);
    }

    assert(retValue != nullptr);
    return retValue;
}

Node* GetV(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = *pointer;
    (*pointer)++;

    if ((*retValue).type != TYPE_VAR) {
        assert(FAIL && "INVALID RETURN FOR GET V FUNCTION");
    } 

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

    while (isspace(**pointer) || (**pointer == '\0')) {
        (*pointer)++;
    }
}

void AnalyseText(Text* text, Tokens* tokens, Text* keywords) {
    assert(text != nullptr);

    tokens->array    = (Node*)  calloc(text->bufSize, sizeof(tokens->array[0]));
    tokens->database = (int8_t*)calloc(text->bufSize, sizeof(tokens->database[0]));

    int8_t* beginning = text->buffer;

    int64_t tokensCounter   = 0;
    int64_t databaseCounter = 0;
    
    for (int8_t* curChar = text->buffer; (size_t)(curChar - beginning) < text->bufSize; curChar++) {
        SkipSpaces(&curChar);
        
        if ((*curChar >= 'A') && (*curChar <= 'z')) {
            int8_t* bufferRemember = tokens->database + databaseCounter;
            
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
                tokens->array[tokensCounter].type            = TYPE_KEYWORD;
                tokens->array[tokensCounter].data.operation  = keywordNum;

                databaseCounter = bufferRemember - tokens->database;
            }
            else {
                tokens->array[tokensCounter].data.expression = bufferRemember;
                tokens->array[tokensCounter].type            = TYPE_VAR;
            }
            
            tokensCounter++;

            if (keywordNum == KEY_END)
                break;
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

    printf("TOKEN'S AMOUNT IS %I64lld\n", tokensCounter + 1);
    //tokens->array    = (Node*)realloc(tokens->array, tokensCounter + 3);
    tokens->database = (int8_t*)realloc(tokens->database, databaseCounter + 3);
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
