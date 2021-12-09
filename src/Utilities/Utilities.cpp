#include <sys\stat.h>
#include <windows.h>

#include "Utilities.h"

int MyFPuts(const uint8_t *str, FILE *stream) {
    assert(str != nullptr);
    assert(stream != nullptr);

    for (int i = 0; str[i] != '\0' && str[i] != '\n'; i++) {
        fputc(str[i], stream);
    }
    fputc('\n', stream);

    return 1;
}

bool MyLblCmp(const int8_t* str1, const int8_t* str2) {
    size_t strIdx = 0;
    
    while ((str1[strIdx] != '\0') && (str2[strIdx] != '\0')) {
        if (str1[strIdx] != str2[strIdx]) {
            return 1;
        }

        strIdx++;
    }

    if ((str1[strIdx] == ':') || (str2[strIdx] == ':'))
        return 0;
    else
        return 1;
}

bool MyStrCmp(const int8_t* str1, const int8_t* str2) {
    size_t strIdx = 0;
    
    while ((str1[strIdx] != '\0') && (str2[strIdx] != '\0')) {
        if (str1[strIdx] != str2[strIdx]) {
            return 1;
        }

        strIdx++;
    }

    return 0;
}

size_t CountFileSize (int fd) {
	assert(fd != -1);
	
	struct stat fileStat = {};
    fstat(fd, &fileStat);
	
	return fileStat.st_size;
}

void ScanIn(float* scannedValue) {
    while (scanf("%g", scannedValue) != 1) {
        printf("Please enter correct value\n");
        fflush(stdin);
    }
}

size_t strLenWithoutSpaces(char* countingString) {
    size_t counter = 0;
    for (size_t curChar = 0; countingString[curChar] != '\0'; curChar++) {
        if (countingString[curChar] != ' ') counter++;
    }

    return counter;
}

uint32_t MyFGets(int8_t buffer[], int32_t bufSize, FILE* stream) {
    assert(buffer != nullptr);
    printf("In MyFGets\n");

    int8_t  curChar     = 0;
    int32_t charsAmount = 0;
    
    while(((curChar = (int8_t)fgetc(stream)) != '\n') &&
          (charsAmount < bufSize - 1)) {
        buffer[charsAmount++] = curChar;
    }
    buffer[charsAmount++] = '|';

    return charsAmount;
}

uint32_t MyFGetsForOneItem(int8_t buffer[], int32_t bufSize, FILE* stream) {
    assert(buffer != nullptr);

    int8_t  curChar     = 0;
    int32_t charsAmount = 0;
    
    while(((curChar = (int8_t)fgetc(stream)) != '\n') &&
          (charsAmount < bufSize - 1)) {
        buffer[charsAmount++] = curChar;
    }
    buffer[charsAmount++] = '\0';

    return charsAmount;
}
