#include "Differ.h"

int32_t Convert1251ToUtf8 (const char* input, char* output) {
    assert (input    != nullptr);
    assert (output   != nullptr);

    int32_t inputLength = 0;
    if ((inputLength = MultiByteToWideChar(1251, 0, input, -1, 0, 0)) == 0) {
        return 0;
    }

    wchar_t* buffer = (wchar_t*)calloc(inputLength + 10, sizeof(buffer[0]));
    if (MultiByteToWideChar(1251, 0, input, -1, buffer, inputLength) == 0) {
        free(buffer);
        return 0;
    }

    int32_t outputLength = 0;
    if ((outputLength = WideCharToMultiByte(65001, 0, buffer, -1, 0, 0, 0, 0)) == 0) {
        free(buffer);
        return 0;
    }

    if (WideCharToMultiByte(65001, 0, buffer, -1, output, outputLength, 0, 0) == 0) {
        free(buffer);

        return 0;
    }
    free(buffer);
    return outputLength;
}

int32_t ProcessNodeData(StackElem rawData, NodeDataTypes* type) {
    assert(rawData != nullptr);
    assert(type    != nullptr);
    
    int32_t convertedData = 0;

    int8_t buffer[MAX_TRASH_SIZE]      = "";
    
    if (sscanf((const char*)rawData, " %d", &convertedData)) {
        *type = NodeDataTypes::TYPE_CONST;

        return convertedData;
    }
    else if (sscanf((const char*)rawData, " %1[sc]", buffer)) {
        convertedData = buffer[0];
        *type = NodeDataTypes::TYPE_UNO;

        return convertedData;
    }
    else if (sscanf((const char*)rawData, " %1[+-*/^l]", buffer)) {
        convertedData = buffer[0];
        *type = NodeDataTypes::TYPE_OP;

        return convertedData;
    }
    else if (sscanf((const char*)rawData, " %1[a-zA-Z]", buffer)) {
        convertedData = buffer[0];
        *type = NodeDataTypes::TYPE_VAR;

        return convertedData;
    }
    else if (sscanf((const char*)rawData, " %1[()]", buffer)) {
        convertedData = buffer[0];
        *type = NodeDataTypes::TYPE_UNKNOWN;

        return convertedData;
    }
    else {
        assert(FAIL && "UNKNOWN TYPE OF DATA IN EXPRESSION");
    }

    return -1;
}

#define N root
#define R root->right
#define L root->left

#define D(smth) Differentiate(smth)
#define C(smth) Copy(smth)
#define F(smth) MakeFactor(smth)

#define MUL(first, second) MakeNewNode((int32_t)(MUL_OP), 0, 0, NodeDataTypes::TYPE_OP, first, second)
#define DIV(first, second) MakeNewNode((int32_t)(DIV_OP), 0, 0, NodeDataTypes::TYPE_OP, first, second)
#define ADD(first, second) MakeNewNode((int32_t)(ADD_OP), 0, 0, NodeDataTypes::TYPE_OP, first, second)
#define SUB(first, second) MakeNewNode((int32_t)(SUB_OP), 0, 0, NodeDataTypes::TYPE_OP, first, second)
#define LOG(first, second) MakeNewNode((int32_t)(LOG_OP), 0, 0, NodeDataTypes::TYPE_OP, first, second)
#define POW(first, second) MakeNewNode((int32_t)(POW_OP), 0, 0, NodeDataTypes::TYPE_OP, first, second)

#define SIN(smth)          MakeNewNode((int32_t)SIN_OP, 0, 0, NodeDataTypes::TYPE_UNO, nullptr, smth)
#define COS(smth)          MakeNewNode((int32_t)COS_OP, 0, 0, NodeDataTypes::TYPE_UNO, nullptr, smth)

#define CONST_NODE(smth)   MakeNewNode(0, smth, 0, NodeDataTypes::TYPE_CONST, nullptr, nullptr)
#define VAR_NODE(smth)     MakeNewNode(0, 0, smth, NodeDataTypes::TYPE_VAR, nullptr, nullptr)     

static int8_t E_CONST[] = "e";

Node* Differentiate (Node* root) {
    assert(root        != nullptr);

    Node* returningRoot = nullptr;

    switch (root->type) {
    case NodeDataTypes::TYPE_VAR:
        if (!strcmp((const char*)root->data.expression, "e"))
            returningRoot = VAR_NODE(root->data.expression);
        else 
            returningRoot = CONST_NODE(1);
        break;
    case NodeDataTypes::TYPE_CONST:
        returningRoot = CONST_NODE(0);
        break;
    case NodeDataTypes::TYPE_OP:
        switch (root->data.operation) {
            case (int32_t)(SUB_OP):
                returningRoot = SUB(D(L), D(R));
                break;
            case (int32_t)(ADD_OP): 
                returningRoot = ADD(D(L), D(R));
                break;

            case (int32_t)(MUL_OP):
                returningRoot = ADD(MUL(D(L), C(R)), MUL(D(R), C(L)));
                break;

            case (int32_t)(DIV_OP):
                returningRoot = DIV(SUB(MUL(D(L), C(R)), MUL(C(L), D(R))), MUL(C(R), C(R)));
                break;

            case (int32_t)LOG_OP: 
                returningRoot = DIV(D(R), MUL(LOG(VAR_NODE(E_CONST), C(L)), C(R)));
                break;

            case (int32_t)POW_OP: 
                if (CheckForVars(L) && CheckForVars(R))
                    returningRoot = MUL(C(N), D(MUL(LOG(VAR_NODE((int8_t*)E_CONST), C(L)), C(R))));

                else if (CheckForVars(L)) 
                    returningRoot = MUL(D(L), MUL(C(R), POW(C(L), SUB(C(R), CONST_NODE(1)))));

                else if (CheckForVars(R))
                    returningRoot = MUL(LOG(VAR_NODE((int8_t*)E_CONST), C(L)), MUL(D(R), POW(C(L), C(R))));

                else
                    returningRoot = CONST_NODE(0);
                break;
            
            default:
                assert(FAIL && "INVALID OPERATION");
        }
        break;
    case NodeDataTypes::TYPE_UNO:
        switch (root->data.operation) {
        case SIN_OP: 
            returningRoot = MUL(COS(C(R)), D(R));
            break;
        
        case COS_OP: 
            returningRoot = MUL(MUL(CONST_NODE(-1), SIN(C(R))), D(R));
            break;
        
        default:
            break;
        }
        break;
    case NodeDataTypes::TYPE_ARR:
    case NodeDataTypes::TYPE_FUNC:
    case NodeDataTypes::TYPE_STR:
    case NodeDataTypes::TYPE_KEYWORD:
        printf("TRYING TO DIFF NODE TYPE %d\n", root->data.operation);
        assert(FAIL && "SYNTAX ERROR, DIFFER CAN'T PROCESS THESE TYPES");
        break;
    case NodeDataTypes::TYPE_UNKNOWN:
    default:
        assert(FAIL && "UNKNOWN DATA TYPE");
    }

    return returningRoot;
}

Node* Copy (Node* root) {
    assert(root  != nullptr);

    if ((root->left  != nullptr) &&
        (root->right != nullptr)) {
        return MakeNewNode(root->data.operation, root->data.number, root->data.expression, root->type, Copy(root->left), Copy(root->right));
    }
    else if ((root->left == nullptr) &&
             (root->right != nullptr)) {
        return MakeNewNode(root->data.operation, root->data.number, root->data.expression, root->type, 0, Copy(root->right));
    }
    else {
        return MakeNewNode(root->data.operation, root->data.number, root->data.expression, root->type, nullptr, nullptr);
    }

    return nullptr;
}

Node* MakeFactor(int32_t factor) {
    if ((factor == 1) || (factor == 0)) {
        return CONST_NODE(1);
    }
    else {
        return MUL(CONST_NODE(factor), F(factor - 1));
    }
}

int32_t FoldConst(Node* node) {
    assert(node != nullptr);
    int32_t returnValue = 0;

    if ((node->type == NodeDataTypes::TYPE_OP) || (node->type == NodeDataTypes::TYPE_UNO)) {
        if  (((node->type) == NodeDataTypes::TYPE_OP)        &&
            ((node->data.operation) != LOG_OP)             &&
            (node->left->type  == NodeDataTypes::TYPE_CONST) &&
            (node->right->type == NodeDataTypes::TYPE_CONST)) {
        switch (node->data.operation) {
            case (int32_t)(ADD_OP):
                node->data.number = node->left->data.number + node->right->data.number;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case (int32_t)(SUB_OP):
                node->data.number = node->left->data.number - node->right->data.number;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case (int32_t)(MUL_OP):
                node->data.number = node->left->data.number * node->right->data.number;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case (int32_t)(DIV_OP):
                node->data.number = node->left->data.number / node->right->data.number;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case (int32_t)(POW_OP):
                if (node->right->data.number > 0) {
                    node->data.number = (int32_t)pow(node->left->data.number, node->right->data.number);
                    node->type = NodeDataTypes::TYPE_CONST;
                    break;
                }
                else {
                    goto elseSection;
                }
            case L_OP:
                node->data.number = node->left->data.number < node->right->data.number;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case LEQ_OP:
                node->data.number = node->left->data.number <= node->right->data.number;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case G_OP:
                node->data.number = node->left->data.number > node->right->data.number;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case GEQ_OP:
                node->data.number = node->left->data.number >= node->right->data.number;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case DEQ_OP:
                node->data.number = 
                (node->left->data.number <= node->right->data.number + __DBL_EPSILON__) &&
                (node->left->data.number >= node->right->data.number - __DBL_EPSILON__);
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case NEQ_OP:
                node->data.number = 
                (node->left->data.number > node->right->data.number + __DBL_EPSILON__) ||
                (node->left->data.number < node->right->data.number - __DBL_EPSILON__);
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            default: {
                printf("Jumping\n");
                goto elseSection;
                break;
                }
            }
        }
        else if (((node->data.operation) == LOG_OP) &&
                (node->left->type == node->right->type) && CheckDoubleEquality(node->left->data.number, node->right->data.number) &&
                ((node->left->type == NodeDataTypes::TYPE_CONST) || (node->left->type == NodeDataTypes::TYPE_VAR))) {
            node->data.number = 1;
            node->type = NodeDataTypes::TYPE_CONST;
        }
        else if (((node->type) == NodeDataTypes::TYPE_OP) &&
                 (node->left->type  == NodeDataTypes::TYPE_VAR)    &&
                 (node->right->type == NodeDataTypes::TYPE_CONST)  &&
                 CheckDoubleEquality(node->right->data.number, 0)) {
            switch (node->data.operation) {
                case POW_OP:
                    node->data.number = 1;
                    node->type = NodeDataTypes::TYPE_CONST;
                    break;
                default:
                    goto elseSection;
            }
        }
        else if ((node->type       == NodeDataTypes::TYPE_UNO) &&
                CheckDoubleEquality(node->right->data.number, 0)) {
            switch (node->data.operation) {
            case SIN_OP:
                node->data.number = 0;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            case COS_OP:
                node->data.number = 1;
                node->type = NodeDataTypes::TYPE_CONST;
                break;
            default:
                goto elseSection;
                break;
            }
        }
        else 
            goto elseSection;        

        if (node->left  != nullptr) NodeDtor(node->left);
        if (node->right != nullptr) NodeDtor(node->right);

        node->left  = nullptr;
        node->right = nullptr; 
        return 1;
    }
    else {
        elseSection:
        if ((node->left  != nullptr) &&
            (node->left->left != nullptr) && (node->left->right != nullptr))
            returnValue += FoldConst(node->left);
        if ((node->right != nullptr) &&
            (node->right->left != nullptr) && (node->right->right != nullptr)) 
            returnValue += FoldConst(node->right);
    }

    return returnValue;
}

#define CUT_EQUAL_NODES(direction1, direction2, value)                          \
    if ((context.node-> direction1 ->type == NodeDataTypes::TYPE_CONST) &&                     \
        (CheckDoubleEquality(context.node-> direction1 ->data.number, value))) {    \
        if (*context.prevNode == context.node) {                                \
            Node* saveNode = *context.prevNode;                                 \
            *context.prevNode = context.node-> direction2;                      \
                                                                                \
            free(saveNode-> direction1);                                        \
            free(saveNode);                                                     \
        }                                                                       \
        else {                                                                  \
            if ((*context.prevNode)->left == context.node) {                    \
                (*context.prevNode)->left = context.node->direction2;           \
            }                                                                   \
            else {                                                              \
                (*context.prevNode)->right = context.node->direction2;          \
            }                                                                   \
            free(context.node-> direction1);                                    \
            free(context.node);                                                 \
        }                                                                       \
                                                                                \
        return 1;                                                               \
    }  

int32_t CutEqualNodes(Context context) {
    int32_t returnValue = 0;

    if (context.node->type == NodeDataTypes::TYPE_OP) {
        if (context.node->data.operation == (int32_t)ADD_OP) {
            CUT_EQUAL_NODES(left, right, 0)
            CUT_EQUAL_NODES(right, left, 0)
        }
        else if (context.node->data.operation == (int32_t)MUL_OP) {
            CUT_EQUAL_NODES(left, right, 1)
            CUT_EQUAL_NODES(right, left, 1)
        }
        else if (context.node->data.operation == (int32_t)SUB_OP) {
            CUT_EQUAL_NODES(right, left, 0)
        }
        else if (context.node->data.operation == (int32_t)POW_OP) {
            CUT_EQUAL_NODES(right, left, 1)
        }
        else if (context.node->data.operation == (int32_t)DIV_OP) {
            CUT_EQUAL_NODES(right, left, 1)
        }
        else if (context.node->data.operation == (int32_t)LOG_OP) {
            CUT_EQUAL_NODES(left, right, 1)
        }
    }

    if ((context.node->left != nullptr) &&
        (context.node->left->left)      &&
        (context.node->left->right)) {
        Context next = {context.node->left, &context.node};    
                                                                    
        returnValue += CutEqualNodes(next); 
    }

    if ((context.node->right != nullptr) &&
        (context.node->right->left)  &&
        (context.node->right->right)) {
        Context next = {context.node->right, &context.node};    
                                                                    
        returnValue += CutEqualNodes(next); 
    }
    
    return returnValue;
}

#undef CUT_EQUAL_NODES

#define CUT_NULL_NODES(direction1, direction2, value)                           \
    if ((context.node-> direction1 ->type == NodeDataTypes::TYPE_CONST) &&                     \
        CheckDoubleEquality(context.node-> direction1 ->data.number,value)) {   \
        if (*context.prevNode == context.node) {                                \
            Node* saveNode = *context.prevNode;                                 \
            *context.prevNode = context.node-> direction1;                      \
                                                                                \
            NodeDtor(saveNode-> direction2);                                    \
            free(saveNode);                                                     \
        }                                                                       \
        else {                                                                  \
            if ((*context.prevNode)->left == context.node) {                    \
                (*context.prevNode)->left = context.node->direction1;           \
            }                                                                   \
            else {                                                              \
                (*context.prevNode)->right = context.node->direction1;          \
            }                                                                   \
            NodeDtor(context.node-> direction2);                                \
            free(context.node);                                                 \
        }                                                                       \
                                                                                \
        return 1;                                                               \
    }

int32_t CutNullNodes(Context context) {
    int32_t returnValue = 0;

    if (context.node->type == NodeDataTypes::TYPE_OP) {
        if (context.node->data.operation == (int32_t)MUL_OP) {
            CUT_NULL_NODES(left, right, 0)
            CUT_NULL_NODES(right, left, 0)
        }
        else if (context.node->data.operation == (int32_t)POW_OP) {
            CUT_NULL_NODES(left, right, 0)
        }
        else if (context.node->data.operation == (int32_t)DIV_OP) {
            CUT_NULL_NODES(left, right, 0)
            if (CheckDoubleEquality(context.node->right->data.number, 0)) {
                assert(FAIL && "ZERO DIVISION ERROR");
            }
        }
        else if (context.node->data.operation == (int32_t)LOG_OP) {
            CUT_NULL_NODES(right, left, 1)
        }
    }

    if ((context.node->left)        &&
        (context.node->left->left)  &&
        (context.node->left->right)) {
        Context next = {context.node->left, &context.node};    
                                                                    
        returnValue += CutNullNodes(next); 
    }

    if ((context.node->right)       &&
        (context.node->right->left)  &&
        (context.node->right->right)) {
        Context next = {context.node->right, &context.node};    
                                                                    
        returnValue += CutNullNodes(next); 
    }
    
    return returnValue;
}

#undef CUT_NULL_NODES
#define CUT_MINUS_ONE_NODES(operator, value, swapping)                              \
    if ((node->data.operation == (int32_t)operator) &&                              \
        CheckDoubleEquality(node->left->data.number, value)) {                      \
        node->data.operation = (int32_t)swapping;                                   \
        NodeDtor(node->left);                                                       \
                                                                                    \
        node->left          = Copy(node->right);                                    \
                                                                                    \
        node->right->data.number   = -1;                                            \
        node->right->left   = nullptr;                                              \
        node->right->right  = nullptr;                                              \
        node->right->type   = NodeDataTypes::TYPE_CONST;                                           \
    }                                           


int32_t CutMinusOneNodes(Node* node) {
    int32_t returnValue = 0;

    if ((node->type == NodeDataTypes::TYPE_OP) &&
        (node->left->type  == NodeDataTypes::TYPE_CONST)) {
        CUT_MINUS_ONE_NODES(DIV_OP, 1, POW_OP)
        CUT_MINUS_ONE_NODES(SUB_OP, 0, MUL_OP)
    }

    if ((node->left != nullptr) &&
        (node->left->left)  &&
        (node->left->right))
        returnValue += CutMinusOneNodes(node->left);

    if ((node->right != nullptr) &&
        (node->right->left)  &&
        (node->right->right))
        returnValue += CutMinusOneNodes(node->right);
    
    return returnValue;
}

#undef CUT_MINUS_ONE_NODES

int32_t CheckForVars(Node* node) {
    assert(node != nullptr);
    int32_t returnValue = 0;

    if ((node->type == NodeDataTypes::TYPE_VAR) &&
        (strcmp((const char*)node->data.expression, (const char*)E_CONST))) {
        returnValue++;
    }

    if (node->left != nullptr) {
        returnValue += CheckForVars(node->left);
    }
    if (node->right != nullptr) {
        returnValue += CheckForVars(node->right);
    }

    return returnValue;
}

Node* OptimisationAfterDiff(Node* node) {
    assert(node != nullptr);

    int32_t optimisationCounter = 0;
    do {
        optimisationCounter  = 0;

        optimisationCounter += FoldConst(node);
        optimisationCounter += CutEqualNodes({node, &node});
        optimisationCounter += CutNullNodes({node, &node});
        optimisationCounter += CutMinusOneNodes(node);
    } while (optimisationCounter);

    return node;
}

bool CheckDoubleEquality(double first, double second) {
    assert(first  != NAN);
    assert(second != NAN);

    if ((first + __DBL_EPSILON__ >= second) &&
        (first - __DBL_EPSILON__ <= second)) {
        return 1;
    }

    return 0;
}
