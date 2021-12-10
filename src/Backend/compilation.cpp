#include "Compilation.h"

const char* ShiftAndCheckArgs(String* string) {   //Shifts beginning of string to beginning of argument
    assert(string != nullptr);
    
    char trashLetters[TRASH_BUFFER_SIZE] = "";
    const char* ptrToArgs = (const char*)string->value + string->lastSpaceBeforeArgs;
    
    if(sscanf(ptrToArgs, "%[ {}a-zA-Zx0-9:.+-\"\"!,$']", trashLetters) & 
      (strlen(trashLetters) != string->length - string->lastSpaceBeforeArgs)) {
        assert(FAIL && "UNKNOWN LETTERS IN ARGUMENT");
    }

    return ptrToArgs;
}

ProcStackElem FindLabelByName(int8_t lblName[], Labels* labels) {
    assert(lblName != nullptr);
    assert(labels  != nullptr);

    for (uint32_t curLbl = 0; (labels->array[curLbl].go != -1) && (curLbl < MAX_LABEL_AMOUNT); curLbl++) {
        if (strcmp((const char*)lblName, (const char*)labels->array[curLbl].name) == 0) {
            return (ProcStackElem)labels->array[curLbl].go;
        }
    }

    assert(FAIL && "LABEL NOT FOUND");
    return 0;
}

void FillLabelsWithPoison(Labels* labels) {
    assert(labels != nullptr);

    for (uint32_t curLbl = 0; curLbl < MAX_LABEL_AMOUNT; curLbl++) {
        labels->array[curLbl].go = -1;
    }
}

void EmitCommand(int32_t cmdNum, CompileResult* output) {
    assert(isfinite(cmdNum));
    assert(output != nullptr);

    if (!(cmdNum & PREPROCESSOR_TYPE_MASK)) {                                                                                                           
        *(output->bytesArray + output->bytesCount) = (int8_t)cmdNum;                                                                                      
          output->bytesCount += COMMAND_SIZE;                                                                                                            
    } 
}

void EmitArgs(int32_t cmdNum, CompileResult* output, Arguments* comArgs, Labels* labels) {
    assert(isfinite(cmdNum));
    assert(output        != nullptr);
    assert(comArgs       != nullptr);
    assert(labels        != nullptr); 
    
    uint32_t argumentFlags = (comArgs->argFlags.string   << STRING_SHIFT) | 
                             (comArgs->argFlags.label    << LABEL_SHIFT)  |
                             (comArgs->argFlags.constant << CONST_SHIFT)  |
                             (comArgs->argFlags.reg      << REG_SHIFT)    |
                             (comArgs->argFlags.mem      << MEM_SHIFT); 

    //!EmitByteOfArg
    if (cmdNum & ARGUMENT_TYPE_MASK) {                                                                                                       
        *(output->bytesArray + output->bytesCount) = (uint8_t)(argumentFlags << SHIFT_OF_FLAGS | comArgs->argReg);                        
          output->bytesCount += BYTE_OF_ARGS;                                                                                                    
    }                                                                                                                                           

    //!EmitConstant                                                                                                                                 
    if (comArgs->argFlags.constant) {                                                                                                  
        *(ProcStackElem*)(output->bytesArray + output->bytesCount) = comArgs->argConst;                                                                
                      output->bytesCount += CONST_ARGUMENT_SIZE;                                                                                               
    }                                                                                                                                           

    //!EmitLabel                                                                                                                         
    if (comArgs->argFlags.label) {       
        if (cmdNum & ARGUMENT_TYPE_MASK) {                                          
            *(ProcStackElem*)(output->bytesArray + output->bytesCount) = (labels->isAllDataRead) ? FindLabelByName((int8_t*)comArgs->labelName, labels) : 0;        
                        output->bytesCount += CONST_ARGUMENT_SIZE;
        }                                 
        else if (labels->isAllDataRead == 0) {
            labels->array[labels->curLbl].name = (int8_t*)comArgs->labelName;
            labels->array[labels->curLbl].go   = output->bytesCount;
            labels->curLbl++;
        }                                                                                                            
    }                                                                                                                                           

    //!EmitString                                                                                                                               
    if (comArgs->argFlags.string) { 
        *(output->bytesArray + output->bytesCount) = (int8_t)128;                                                                              
          output->bytesCount += STRING_DIVIDER_SIZE;  
                                                                                                        
        *(output->bytesArray + output->bytesCount) = STRING_DIVIDER;                                                                              
          output->bytesCount += STRING_DIVIDER_SIZE;                                                                                               

        output->bytesCount += sprintf((char*)(output->bytesArray + output->bytesCount), "%s", comArgs->stringName);                                                                                                                                                                                                                                                                      
                                                                                                                                                
        *(output->bytesArray + output->bytesCount) = STRING_DIVIDER;                                                                              
          output->bytesCount += STRING_DIVIDER_SIZE;                                                                                               
    }
}

bool PushArgsFilter(Flags argFlags) {
    if ((argFlags.label)  || 
        (argFlags.string) || 
       !(argFlags.mem     || argFlags.reg || argFlags.constant))
       return 0;
    return 1;
}

bool DbArgsFilter(Flags argFlags) {
    if ((!argFlags.string) || 
         (argFlags.label)  || 
         (argFlags.reg)    || 
         (argFlags.mem)    || 
         (argFlags.constant))
        return 0;
    return 1;
}

bool PopArgsFilter(Flags argFlags) {
    if ((  argFlags.label)  || 
        (  argFlags.string) || 
        (( argFlags.constant)   &&  (!argFlags.mem)   &&  (!argFlags.reg))  || 
        ((!argFlags.constant)   &&   (argFlags.mem)   &&  (!argFlags.reg)))
        return 0;
    return 1;
}

bool JumpArgsFilter(Flags argFlags) {
    if ((!argFlags.label)     || 
         (argFlags.string)    || 
         (argFlags.reg)       || 
         (argFlags.mem)       || 
         (argFlags.constant))
        return 0;
    return 1;
}

bool OutArgsFilter (Flags argFlags) {
    if ((argFlags.label)  || 
        (argFlags.string))
       return 0;
    return 1;
}

bool NoArgsFilter (Flags /*argFlags*/) {
    return 1;
}

void FillArguments(Arguments* args) {
    assert(args != nullptr);

    for (uint32_t curArg = 0; curArg < MAX_OUTPUTFILE_LENGTH; curArg++) {
        args[curArg].labelName  = (char*)calloc(MAX_LABEL_NAME,  sizeof(args[curArg].labelName[0]));
        args[curArg].stringName = (char*)calloc(MAX_STRING_NAME, sizeof(args[curArg].stringName[0]));
    }

}
