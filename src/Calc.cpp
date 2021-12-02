/*  G::= F
    F::= V {with} {V,}* begin {K | S}* 'lilEnd'
    K::= VasyaSniff | NextSniff | Homyak | Hire 
    S::= (V 'in' E) | (E 'to' V) | E '$'
    E::= T{[+-]T} *
    T::= P{[*\]P} *
    P::= '('E')' | V '('E {,E}*')' | V  | N
    V::= TYPE_VAR
    N::= TYPE_CONST
*/

#include "Calc.h"

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

    Node* retValue = GetF(pointer);
    
    printf("type is %d; operation is %c\n", (**pointer).type, (**pointer).data.operation);
    
    if (((**pointer).type == TYPE_KEYWORD) && ((**pointer).data.operation == KEY_LILEND)) {
        (*pointer)->left = retValue;
        retValue = *pointer;

        (*pointer)++;
    }
    else {
        assert("SYNTAX ERROR, EOF NOT FOUND");
    }

    return retValue;
}

Node* GetF(Node** pointer) {
    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = 0;

    if ((**pointer).type == TYPE_VAR) {
        retValue = GetV(pointer);

        if (((**pointer).type == TYPE_KEYWORD) && ((**pointer).data.operation == KEY_WITH)) {
            retValue->left = *pointer;
            (*pointer)++;

            bool firstTimeFlag = 1;

            retValue->type = TYPE_FUNC;
            Node* bottomPtr = retValue->left;

            while (((**pointer).type != TYPE_KEYWORD) || ((**pointer).data.operation != KEY_BEGIN)) {
                if (!firstTimeFlag) {
                    if (((**pointer).type == TYPE_OP) && ((**pointer).data.operation == ',')) {
                        bottomPtr->right = *pointer;
                        bottomPtr = *pointer;

                        (*pointer)++;
                    }
                }

                bottomPtr->left = GetV(pointer);
                firstTimeFlag = 0;
            }
        }
        Require(((**pointer).type != TYPE_KEYWORD) || ((**pointer).data.operation != KEY_BEGIN));

        retValue->right = ((**pointer).type == TYPE_KEYWORD) ? GetK(pointer) : GetS(pointer);
        Node* bottom = retValue->right;

        while(((**pointer).type != TYPE_KEYWORD) || ((**pointer).data.operation != KEY_LILEND)) {
            bottom->right = ((**pointer).type == TYPE_KEYWORD) ? GetK(pointer) : GetS(pointer);
            bottom = bottom->right;
        }
    }

    return retValue;
}

Node* GetK(Node** pointer) {
    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = *pointer;

    switch ((**pointer).data.operation) {
        case KEY_WHILE:
        case KEY_IF:
            (*pointer)++;
            retValue->left  = GetE(pointer);
            break;
        case KEY_FOR: {
            (*pointer)++;
            Node* whoNode = GetV(pointer);
            
            if (((**pointer).data.operation == KEY_FROM) && ((**pointer).type == TYPE_KEYWORD)) {
                retValue->left = (*pointer)++;
                retValue->left->left = whoNode;
                
                Node* fromNode = GetE(pointer);
                if (((**pointer).data.operation == KEY_TO) && ((**pointer).type == TYPE_KEYWORD)) {
                    retValue->left->right = (*pointer)++;
                    retValue->left->right->left = fromNode;

                    Node* toNode = GetE(pointer);
                    
                    if (((**pointer).data.operation == KEY_WITH) && ((**pointer).type == TYPE_KEYWORD)) {
                        retValue->left->right->right = (*pointer)++;

                        retValue->left->right->right->left  = toNode;
                        retValue->left->right->right->right = GetE(pointer);
                    }
                    else {
                        retValue->left->right->right = toNode;
                    }
                }
                else {
                    assert(FAIL && "CAN'T FIND \"TO\" AFTER EXPRESSION");
                }
            }
            else {
                assert(FAIL && "CAN'T FIND \"FROM\" AFTER HIRE");
            }
            break;
        }
        default:
            assert(FAIL && "INVALID KEYWORD FOR BEING FIRST IN LINE");
            break;
    }

    if ((**pointer).data.operation == KEY_BEGIN) {
        retValue->right = (*pointer)++; 
        retValue->right->left = GetS(pointer);
        Node* bottom = retValue->right->left;

        while(((**pointer).type != TYPE_KEYWORD) || ((**pointer).data.operation != KEY_LILEND)) {
            bottom->right = GetS(pointer);
            bottom = bottom->right;
        }
        (*pointer)++;
    }
    else {
        assert(FAIL && "BEGIN AFTER KEYWORD EXPRESSION NOT FOUND");
    }

    if ((retValue->data.operation   == KEY_IF) &&
        ((**pointer).type           == TYPE_KEYWORD)   && 
        ((**pointer).data.operation == KEY_ELSEIF)) {

        (**pointer).data.operation  = KEY_IF;
        retValue->right->right      = GetK(pointer);
    }
    else if (((retValue->data.operation   == KEY_IF)    || 
              (retValue->data.operation   == KEY_WHILE)) &&
            ((**pointer).type == TYPE_KEYWORD)          && 
            ((**pointer).data.operation == KEY_ELSE)) {

        (*pointer)++;
        (*pointer)++;
    
        retValue->right->right = GetS(pointer);
        Node* bottom = retValue->right->right;

        while(((**pointer).type != TYPE_KEYWORD) || ((**pointer).data.operation != KEY_LILEND)) {
            bottom->right = GetS(pointer);
            bottom = bottom->right;
        }
        (*pointer)++;
    }
    Node* connector = MakeNewNode('$', 0, TYPE_UNO, retValue, 0);
    retValue = connector;

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
    
    if (((**pointer).type == TYPE_UNO) && ((**pointer).data.operation == '$')) {
        (*pointer)->left = retValue;
        retValue = *pointer;

        (*pointer)++;
    }
    else {
        assert(FAIL && "SYNTAX ERROR, EOL NOT FOUND");
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
            bool firstTimeFlag = 1;

            retValue->type = TYPE_FUNC;
            Node* bottomPtr = retValue;

            while (((**pointer).type != TYPE_UNO) || ((**pointer).data.operation != ')')) {
                if (!firstTimeFlag) {
                    if (((**pointer).type == TYPE_OP) && ((**pointer).data.operation == ',')) {
                        bottomPtr->right = *pointer;
                        bottomPtr = *pointer;

                        (*pointer)++;
                    }
                }

                bottomPtr->left = GetE(pointer);
                firstTimeFlag = 0;
            }

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
        fprintf(stderr, "I CAN'T EAT %c[%d] with type %d\n", (**pointer).data.operation, (**pointer).data.number, (**pointer).type);
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
            case ',':
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
