#include <string.h>
#include <stdio.h>
#include <windows.h>

const int MAX_CMD_LENGTH = 100;

char* GetFormat(char* input);

int main(int argc, char* argv[]) {
    char* inputFile = 0;

    char frontEndCmd[MAX_CMD_LENGTH]  = "";
    char middleEndCmd[MAX_CMD_LENGTH] = ".\\Middleend\\Middleend.exe ";
    char frontEndMinusOne[MAX_CMD_LENGTH] = "";

    for (int curArg = 1; curArg < argc; curArg++) {
        if (!strcmp(argv[curArg], "-o")) {
            inputFile = argv[curArg + 1];

            sprintf(frontEndCmd, ".\\Frontend\\%s.exe -o %s ", GetFormat(inputFile), inputFile);
        }

        if (!strcmp(argv[curArg], "-frontG")) {
            sprintf(frontEndCmd, "-frontG ");
        }

        if (!strcmp(argv[curArg], "-asmS")) {
            sprintf(middleEndCmd, "-amsS ");
        }

        if (!strcmp(argv[curArg], "-middleG")) {
            sprintf(middleEndCmd, "-middleG ");
        }

        if (!strcmp(argv[curArg], "-dec")) {
            sprintf(frontEndMinusOne, ".\\Frontend\\Noname-1.exe");
        }
    }

    system(frontEndCmd);
    system(frontEndMinusOne);
    system(middleEndCmd);
}

char* GetFormat(char* input) {
    while (*(input++) != '\0') {
        if (*input == '.') {
            return input + 1;
        }
    }

    return 0;
}

