#include "Grammar.h"

void MakeAST(Tree* AST, Tokens* tokens) {
    assert(AST != nullptr);

    AST->root = GetG(&tokens->array);
    Node* bottom = AST->root;

    while((tokens->array->type != NodeDataTypes::TYPE_KEYWORD) || (tokens->array->data.operation != KEY_END)) {
        bottom->right = GetG(&tokens->array);
        bottom = bottom->right;
    }
}

Node* GetG(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = GetExternal(pointer);
    
    return retValue;
}

Node* GetExternal(Node** pointer) {
    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = 0;

    if (((*pointer + 1)->type == NodeDataTypes::TYPE_KEYWORD) && 
        (((*pointer + 1)->data.operation == KEY_WITH) ||
         ((*pointer + 1)->data.operation == KEY_BEGIN)))
        retValue = GetF(pointer);
    else {
        retValue = GetS(pointer);
    }   

    return retValue;
}

Node* GetF(Node** pointer) {
    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = 0;

    if ((**pointer).type == NodeDataTypes::TYPE_VAR) {
        retValue = GetV(pointer);

        if (((**pointer).type == NodeDataTypes::TYPE_KEYWORD) && ((**pointer).data.operation == KEY_WITH)) {
            retValue->left = *pointer;
            (*pointer)++;

            bool firstTimeFlag = 1;

            retValue->type = NodeDataTypes::TYPE_FUNC;
            Node* bottomPtr = retValue->left;

            while (((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_BEGIN)) {
                if (!firstTimeFlag) {
                    if (((**pointer).type == NodeDataTypes::TYPE_OP) && ((**pointer).data.operation == COMMA_OP)) {
                        bottomPtr->right = *pointer;
                        bottomPtr = *pointer;

                        (*pointer)++;
                    }
                }

                bottomPtr->left = GetV(pointer);
                firstTimeFlag = 0;
            }
        }
        Require(((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_BEGIN));

        retValue->right = GetK(pointer);
        Node* bottom = retValue->right;

        while(((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_LILEND)) {
            bottom->right = GetK(pointer);
            bottom = bottom->right;
        }
    }

    retValue->type = NodeDataTypes::TYPE_FUNC;

    if (((**pointer).type == NodeDataTypes::TYPE_KEYWORD) && ((**pointer).data.operation == KEY_LILEND)) {
        (*pointer)->left = retValue;
        retValue = *pointer;

        (*pointer)++;
    }
    else {
        assert("SYNTAX ERROR, EOF NOT FOUND");
    }

    return retValue;
}

#define IO_CONSTRUCTION(function)                                                                   \
    (*pointer)++;                                                                                   \
    Node* remember = function(pointer);                                                             \
                                                                                                    \
    if (((**pointer).type == NodeDataTypes::TYPE_OP) && ((**pointer).data.operation == COMMA_OP)) {                \
        Node* bottom = retValue;                                                                    \
        bottom->left = remember;                                                                    \
                                                                                                    \
        while (((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_DOT)) {     \
            bottom->right = *pointer;                                                               \
            bottom = bottom->right;                                                                 \
                                                                                                    \
            (*pointer)++;                                                                           \
            bottom->left = function(pointer);                                                       \
        }                                                                                           \
                                                                                                    \
        Require(((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_DOT));     \
    }                                                                                               \
    else {                                                                                          \
        Require(((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_DOT));     \
                                                                                                    \
        retValue->left = remember;                                                                  \
    }                                                                                               \
                                                                                                    \
    Node* conkNode = MakeNewNode(EOL_OP, 0, 0, NodeDataTypes::TYPE_UNO, retValue, 0);                              \
    return conkNode;                                                                                \
    break

Node* GetD(Node** pointer) {
    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    if (((**pointer).type == NodeDataTypes::TYPE_KEYWORD) && ((**pointer).data.operation == KEY_DIFF)) {
        Node* diff = *pointer;
        (*pointer)++;

        diff->left = GetE(pointer);

        return diff;
    }
    else {
        return GetE(pointer);
    }
}

Node* GetK(Node** pointer) {
    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    if ((**pointer).type == NodeDataTypes::TYPE_KEYWORD) {
    Node* retValue = *pointer;

    switch ((**pointer).data.operation) {
        case KEY_DIFF:
            retValue = GetD(pointer);
            break;
        case KEY_GOBBLE: {
            IO_CONSTRUCTION(GetV);
        }
        case KEY_CRY: {
            IO_CONSTRUCTION(GetD);
        }
        case KEY_SHOW: {
            IO_CONSTRUCTION(GetD);
        }
        case KEY_RETURN:
            (*pointer)++;
            retValue->left = GetS(pointer);
            return retValue;

            break;
        case KEY_WHILE:
        case KEY_IF:
            (*pointer)++;
            retValue->left  = GetD(pointer);
            break;
        case KEY_FOR: {
            (*pointer)++;
            Node* whoNode = GetV(pointer);
            
            if (((**pointer).data.operation == KEY_FROM) && ((**pointer).type == NodeDataTypes::TYPE_KEYWORD)) {
                retValue->left = (*pointer)++;
                retValue->left->left = whoNode;
                
                Node* fromNode = GetD(pointer);
                if (((**pointer).data.operation == KEY_TO) && ((**pointer).type == NodeDataTypes::TYPE_KEYWORD)) {
                    retValue->left->right = (*pointer)++;
                    retValue->left->right->left = fromNode;

                    Node* toNode = GetD(pointer);
                    
                    if (((**pointer).data.operation == KEY_WITH) && ((**pointer).type == NodeDataTypes::TYPE_KEYWORD)) {
                        retValue->left->right->right = (*pointer)++;

                        retValue->left->right->right->left  = toNode;
                        retValue->left->right->right->right = GetD(pointer);
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
        retValue->right->left = GetK(pointer);
        Node* bottom = retValue->right->left;

        while(((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_LILEND)) {
            bottom->right = GetK(pointer);
            bottom = bottom->right;
        }
        (*pointer)++;
    }
    else {
        assert(FAIL && "BEGIN AFTER KEYWORD EXPRESSION NOT FOUND");
    }

    if ((retValue->data.operation   == KEY_IF) &&
        ((**pointer).type           == NodeDataTypes::TYPE_KEYWORD)   && 
        ((**pointer).data.operation == KEY_ELSEIF)) {

        (**pointer).data.operation  = KEY_IF;
        retValue->right->right      = GetK(pointer);
    }
    else if (((retValue->data.operation   == KEY_IF)    || 
              (retValue->data.operation   == KEY_WHILE)) &&
            ((**pointer).type == NodeDataTypes::TYPE_KEYWORD)          && 
            ((**pointer).data.operation == KEY_ELSE)) {

        (*pointer)++;
        (*pointer)++;
    
        retValue->right->right = GetK(pointer);
        Node* bottom = retValue->right->right;

        while(((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_LILEND)) {
            bottom->right = GetK(pointer);
            bottom = bottom->right;
        }
        (*pointer)++;
    }
    Node* connector = MakeNewNode(EOL_OP, 0, 0, NodeDataTypes::TYPE_UNO, retValue, 0);
    retValue = connector;

    return retValue;
    }

    else {
        return GetS(pointer);
    }
}

Node* GetS(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue       = 0;
    Node* ptrToSkipped   = SkipThings(pointer);
    
    if ((ptrToSkipped->type == NodeDataTypes::TYPE_KEYWORD) &&
        (ptrToSkipped->data.operation == KEY_IN)) {
        retValue       = ptrToSkipped;

        retValue->left = GetV(pointer);
        (*pointer)++;

        if (retValue->left->type == NodeDataTypes::TYPE_VAR) {
            retValue->right = GetD(pointer);
        }
        else {
            printf("Getting in args for var %s\n", retValue->left->data.expression);
            Node* exprNode = GetD(pointer);
            Node* bottom   = retValue;

            if (((**pointer).type == NodeDataTypes::TYPE_KEYWORD) && ((**pointer).data.operation == KEY_DOT)) {
                while(((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_DOT)) {
                    bottom->right = *pointer;
                    (*pointer)++;

                    bottom = bottom->right;

                    bottom->left = exprNode;
                                        
                    exprNode = GetD(pointer);
                }
                bottom->right = exprNode;

                Require(((**pointer).type != NodeDataTypes::TYPE_KEYWORD) || ((**pointer).data.operation != KEY_DOT));
            }
            else {
                bottom->right = exprNode;
            }
        }
    }
    else if (((*(*pointer + 1)).type == NodeDataTypes::TYPE_KEYWORD) && ((*(*pointer + 1)).data.operation == KEY_TO)) {
        Node* expression = GetD(pointer);

        if (((**pointer).type == NodeDataTypes::TYPE_KEYWORD) && ((**pointer).data.operation == KEY_TO)) {
            retValue = *pointer;
            (*pointer)++;

            retValue->left  = expression;
            retValue->right = GetV(pointer);
        }
        else {
            retValue = expression;
        }
    }
    else {
        retValue = GetD(pointer);
    }
    
    if (((**pointer).type == NodeDataTypes::TYPE_UNO) && ((**pointer).data.operation == EOL_OP)) {
        (*pointer)->left = retValue;
        retValue = *pointer;

        (*pointer)++;
    }
    else {
        fprintf(stderr, "Looking at node with type %d and op is %d\n", (**pointer).type, (**pointer).data.operation);
        assert(FAIL && "SYNTAX ERROR, EOL NOT FOUND");
    }

    return retValue;
}

Node* SkipThings(Node** pointer) {
    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    int32_t ptrOffset = 0;
    
    if ((**pointer).type == NodeDataTypes::TYPE_VAR) {
        ptrOffset++;
    }

    if (((*pointer + ptrOffset)->type == NodeDataTypes::TYPE_UNO) &&
        ((*pointer + ptrOffset)->data.operation == LEFT_SQR_OP)) {
        while(((*pointer + ptrOffset)->type != NodeDataTypes::TYPE_UNO) ||
              ((*pointer + ptrOffset)->data.operation != RIGHT_SQR_OP)) {
            ptrOffset++;
        }
        ptrOffset++;
    }

    return *pointer + ptrOffset;
}

Node* GetE(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = 0;

    if ((**pointer).type != NodeDataTypes::TYPE_STR) {
        retValue = GetT(pointer);
        
        while((((**pointer).data.operation == ADD_OP) || 
            ((**pointer).data.operation == SUB_OP) || 
            ((**pointer).data.operation == LEQ_OP) ||
            ((**pointer).data.operation == L_OP)   ||
            ((**pointer).data.operation == GEQ_OP) ||
            ((**pointer).data.operation == G_OP)   ||
            ((**pointer).data.operation == DEQ_OP)) &&
            ((**pointer).type == NodeDataTypes::TYPE_OP)) {
            Node*  operationNode = *pointer;
            (*pointer)++;

            Node* bufValue = GetT(pointer);

            operationNode->left  = retValue;
            operationNode->right = bufValue;

            retValue = operationNode;
        }
    }
    else {
        retValue = GetStr(pointer);
    }

    return retValue;
}

Node* GetT(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = GetDeg(pointer);

    while((((**pointer).data.operation == MUL_OP) || ((**pointer).data.operation == DIV_OP)) &&
           ((**pointer).type == NodeDataTypes::TYPE_OP)) {
        Node* operationNode = *pointer;
        (*pointer)++;

        Node* bufValue = GetDeg(pointer);

        operationNode->left  = retValue;
        operationNode->right = bufValue;

        retValue = operationNode;
    }

    return retValue; 
}

Node* GetDeg(Node** pointer) {
    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = GetP(pointer);

    if (((**pointer).type == NodeDataTypes::TYPE_OP) && ((**pointer).data.operation == POW_OP)) {
        Node* remember = retValue;

        retValue = *pointer;
        (*pointer)++;

        retValue->left  = remember;
        retValue->right = GetP(pointer);
    }

    return retValue;
}

Node* GetP(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = nullptr;
    Node* bottom = nullptr;

    while ((**pointer).type == NodeDataTypes::TYPE_OP) {
        if ((**pointer).data.operation == SUB_OP) {
            if (bottom == nullptr) {
                bottom        = MakeNewNode(MUL_OP, 0, 0, NodeDataTypes::TYPE_OP, 0, 0);
            }
            else {
                bottom->right = MakeNewNode(MUL_OP, 0, 0, NodeDataTypes::TYPE_OP, 0, 0);
                bottom = bottom->right;
            }

            bottom->left = MakeNewNode(0, -1, 0, NodeDataTypes::TYPE_CONST, 0, 0);
        }

        (*pointer)++;
    }

    if (((**pointer).type == NodeDataTypes::TYPE_UNO) && ((**pointer).data.operation == LEFT_ROUND_OP)) {
        (*pointer)++;

        retValue = GetD(pointer);

        Require(((**pointer).type != NodeDataTypes::TYPE_UNO) || ((**pointer).data.operation != RIGHT_ROUND_OP));
    }
    else if ((**pointer).type == NodeDataTypes::TYPE_VAR) {
        retValue = GetV(pointer);

        if (((**pointer).type == NodeDataTypes::TYPE_UNO) && ((**pointer).data.operation == LEFT_ROUND_OP)) {
            (*pointer)++;
            bool firstTimeFlag = 1;

            retValue->type = NodeDataTypes::TYPE_FUNC;
            Node* bottomPtr = retValue;

            while (((**pointer).type != NodeDataTypes::TYPE_UNO) || ((**pointer).data.operation != RIGHT_ROUND_OP)) {
                if (!firstTimeFlag) {
                    if (((**pointer).type == NodeDataTypes::TYPE_OP) && ((**pointer).data.operation == COMMA_OP)) {
                        bottomPtr->right = *pointer;
                        bottomPtr = *pointer;

                        (*pointer)++;
                    }
                }

                bottomPtr->left = GetD(pointer);
                firstTimeFlag = 0;
            }

            Require(((**pointer).type != NodeDataTypes::TYPE_UNO) || ((**pointer).data.operation != RIGHT_ROUND_OP));
        }
    }
    else {
        retValue = GetN(pointer);
    }

    if (bottom == nullptr)
        return retValue;
    else {
        bottom->right = retValue;
        return bottom;
    }  
}

Node* GetStr(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = *pointer;
    (*pointer)++;

    if (retValue->type != NodeDataTypes::TYPE_STR) {
        assert(FAIL && "INVALID RETURN FOR GET STR FUNCTION");
    }

    return retValue;
}

Node* GetV(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = *pointer;
    (*pointer)++;

    if ((*retValue).type != NodeDataTypes::TYPE_VAR) {
        assert(FAIL && "INVALID RETURN FOR GET V FUNCTION");
    } 

    if(((**pointer).type == NodeDataTypes::TYPE_UNO) &&
        ((**pointer).data.operation == LEFT_SQR_OP)) {
        (*pointer)++;

        retValue->left = GetD(pointer);

        Require(((**pointer).type != NodeDataTypes::TYPE_UNO) || ((**pointer).data.operation != RIGHT_SQR_OP));
        retValue->type = NodeDataTypes::TYPE_ARR;
    }

    return retValue;
}

Node* GetN(Node** pointer) {
    assert(pointer  != nullptr);
    assert(*pointer != nullptr);

    Node* retValue = *pointer;
    (*pointer)++;

    if ((*retValue).type != NodeDataTypes::TYPE_CONST) {
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
        
        if (((*curChar >= 'A') && (*curChar <= 'Z')) || ((*curChar >= 'a') && (*curChar <= 'z'))) {
            int8_t* bufferRemember = tokens->database + databaseCounter;
            while (((*curChar >= 'A') && (*curChar <= 'Z')) || 
                   ((*curChar >= 'a') && (*curChar <= 'z')) ||
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
                tokens->array[tokensCounter].type            = NodeDataTypes::TYPE_KEYWORD;
                tokens->array[tokensCounter].data.operation  = keywordNum;

                databaseCounter = bufferRemember - tokens->database;
            }
            else {
                tokens->array[tokensCounter].data.expression = bufferRemember;
                tokens->array[tokensCounter].type            = NodeDataTypes::TYPE_VAR;
            }
            
            tokensCounter++;

            if (keywordNum == KEY_END)
                break;
        }
        else if ((*curChar >= '0') && (*curChar <= '9')) {
            double value = 0;
            uint32_t digitsAfterDot = 0;
            
            while (((*curChar >= '0') && 
                    (*curChar <= '9')) || 
                    (*curChar == '.')) {
                if (*curChar == '.') {
                    if (digitsAfterDot == 0) {
                        digitsAfterDot++;
                    }
                    else {
                        assert(FAIL && "DOUBLE DOT AT NUMBER");
                    }

                    curChar++;
                }

                if (digitsAfterDot) {
                    value += (*curChar - '0') / pow(10, digitsAfterDot);

                    digitsAfterDot++;
                }
                else {
                    value = 10*value + (*curChar - '0');
                }

                curChar++;
            }
            curChar--;

            tokens->array[tokensCounter].type               = NodeDataTypes::TYPE_CONST;
            tokens->array[tokensCounter].data.number        = value;

            tokensCounter++;
        }
        else if (*curChar == '\"') {
            int8_t* bufferRemember = tokens->database + databaseCounter;
            curChar++;

            while (*curChar != '\"') {
                tokens->database[databaseCounter] = *curChar;

                databaseCounter++;
                curChar++;
            }

            tokens->database[databaseCounter] = '\0';
            databaseCounter++;

            tokens->array[tokensCounter].data.expression = bufferRemember;
            tokens->array[tokensCounter].type            = NodeDataTypes::TYPE_STR;

            tokensCounter++;
        }
        else {
            switch (*curChar) {
                case LEFT_ROUND_OP:
                case RIGHT_ROUND_OP:
                case LEFT_SQR_OP:
                case RIGHT_SQR_OP:
                case LEFT_CUR_OP:
                case RIGHT_CUR_OP:
                case EOL_OP:
                    tokens->array[tokensCounter].type           = NodeDataTypes::TYPE_UNO;
                    tokens->array[tokensCounter].data.operation = *curChar;
                    break;
                case COMMA_OP:
                case ADD_OP:
                case SUB_OP:
                case MUL_OP:
                case DIV_OP:
                case POW_OP:
                    tokens->array[tokensCounter].type           = NodeDataTypes::TYPE_OP;
                    tokens->array[tokensCounter].data.operation = *curChar;
                    break;
                case NON_OP:
                    if (*(curChar + 1) == EQ_OP) {
                        tokens->array[tokensCounter].data.operation = NEQ_OP;
                        curChar++;
                    }
                    else {
                        assert(FAIL && "SINGLE NON OPERATOR NOT ALLOWED");
                    }
                    tokens->array[tokensCounter].type = NodeDataTypes::TYPE_OP;
                    break;
                case L_OP:
                    if (*(curChar + 1) == EQ_OP) {
                        tokens->array[tokensCounter].data.operation = LEQ_OP;
                        curChar++;
                    }
                    else {
                        tokens->array[tokensCounter].data.operation = *curChar;
                    }
                    tokens->array[tokensCounter].type = NodeDataTypes::TYPE_OP;
                    break;
                case G_OP:
                    if (*(curChar + 1) == EQ_OP) {
                        tokens->array[tokensCounter].data.operation = GEQ_OP;
                        curChar++;
                    }
                    else {
                        tokens->array[tokensCounter].data.operation = *curChar;
                    }
                    tokens->array[tokensCounter].type = NodeDataTypes::TYPE_OP;
                    break;
                case EQ_OP:
                    if (*(curChar + 1) == EQ_OP) {
                        tokens->array[tokensCounter].data.operation = DEQ_OP;
                        curChar++;
                    }
                    else {
                        assert(FAIL && "SINGLE EQ NOT AN OPERATOR");
                    }
                    tokens->array[tokensCounter].type = NodeDataTypes::TYPE_OP;
                    break;
                default:
                    printf("%c[%d]\n", *curChar,*curChar);
                    assert(FAIL && "Invalid operator");
                    break;
            }

            tokensCounter++;
        }
    }

    //tokens->array    = (Node*)realloc(tokens->array, tokensCounter + 3);
    tokens->database = (int8_t*)realloc(tokens->database, databaseCounter + 3);
}

int8_t IsKeyword(int8_t* buffer, Text* keywords) {
    assert(buffer   != nullptr);
    assert(keywords != nullptr);

    for (uint32_t curStr = 0; curStr < keywords->strAmount; curStr++) {
        if (!strcmp((const char*)buffer, (const char*)keywords->strings[curStr].value)) {
            return (int8_t)(curStr + 1);
        }
    }

    return 0;
}
