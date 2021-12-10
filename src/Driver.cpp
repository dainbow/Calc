#include <string.h>
#include <stdio.h>
#include <windows.h>

const int MAX_CMD_LENGTH = 100;

char* GetFormat(char* input);
void DeleteFormat(char* inputFile);

int main(int argc, char* argv[]) {
    char* inputFile  = 0;
    char* outputFile = 0;

    char frontEndCmd[MAX_CMD_LENGTH]  = "";
    char* frontEnd = frontEndCmd;

    char middleEndCmd[MAX_CMD_LENGTH] = "";
    char* middleEnd = middleEndCmd;

    char frontEndMinusOne[MAX_CMD_LENGTH] = "";
    char* frontEndMinusOneEnd = frontEndMinusOne;

    char backEndCmd[MAX_CMD_LENGTH] = "";
    char* backEnd = backEndCmd;

    bool isMFlag = 0;

    for (int curArg = 1; curArg < argc; curArg++) {
        if (!strcmp(argv[curArg], "-i")) {
            inputFile = argv[curArg + 1];

            frontEnd += sprintf(frontEnd, 
                                ".\\Frontend\\%s.exe -i %s ", GetFormat(inputFile), inputFile);

            DeleteFormat(inputFile);
            middleEnd += sprintf(middleEnd, ".\\Middleend\\Middleend.exe ", inputFile);
            backEnd   += sprintf(backEnd, ".\\Backend\\Backend.exe ", inputFile);
        }

        if (!strcmp(argv[curArg], "-o")) {
            outputFile = argv[curArg + 1];

            DeleteFormat(outputFile);

            frontEnd  += sprintf(frontEnd, "-o %s", outputFile);
            middleEnd += sprintf(middleEnd, "-i %s -o %s", outputFile, outputFile);
            backEnd   += sprintf(backEnd, "-i %s -o %s", outputFile, outputFile);
        }

        if (!strcmp(argv[curArg], "-frontG")) {
            frontEnd += 
                sprintf(frontEnd, 
                        "-frontG ");
        }

        if (!strcmp(argv[curArg], "-asmS")) {
            backEnd += 
                sprintf(backEnd, 
                        "-asmS ");
        }

        if (!strcmp(argv[curArg], "-endG")) {
            backEnd += 
                sprintf(backEnd, 
                        "-endG ");
        }

        if (!strcmp(argv[curArg], "-dec")) {
            frontEndMinusOneEnd += 
                sprintf(frontEndMinusOneEnd, 
                    ".\\Frontend\\Noname-1.exe ");
            if (inputFile)
                frontEndMinusOneEnd += sprintf(frontEndMinusOneEnd, "-i %s ", outputFile);
            
            if (outputFile)
                frontEndMinusOneEnd += sprintf(frontEndMinusOneEnd, "-o %s ", outputFile);
        }

        if (!strcmp(argv[curArg], "-m")) {
            isMFlag = 1;
        }
    }

    if (!isMFlag) {
        system(frontEndCmd);
        system(frontEndMinusOne);
        system(middleEndCmd);
        system(backEndCmd);
    }
    else {
        if (*frontEndCmd)      printf("[Driver] %s\n", frontEndCmd);
        if (*frontEndMinusOne) printf("[Driver] %s\n", frontEndMinusOne);
        if (*backEndCmd)       printf("[Driver] %s\n", backEndCmd);
        if (*middleEndCmd)     printf("[Driver] %s\n", middleEndCmd);     
    }
}

void DeleteFormat(char* inputFile) {
    while (*(inputFile++)) {
        if (*inputFile == '.') {
            *inputFile = '\0';
        }
    }
}

char* GetFormat(char* input) {
    while (*(input++) != '\0') {
        if (*input == '.') {
            return input + 1;
        }
    }

    return 0;
}

