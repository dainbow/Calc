#include <assert.h>
#include <io.h>
#include <string.h>
#include <windows.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "Text.h"
#include "Utilities.h"

bool ReadTextFromFile(Text *text, const char* inputFile) {
    assert(text != nullptr);
    assert(inputFile != nullptr);

    int input = open(inputFile, O_RDONLY | O_BINARY, 0);
    if (input == -1)
        return 0;

    text->bufSize = CountFileSize(input);
    text->buffer = (int8_t*)calloc(text->bufSize + 10, sizeof(text->buffer[0]));
    assert(text->buffer != nullptr);

    read(input, text->buffer, (uint32_t)text->bufSize);

    close(input);
    return 1;
}

void CountStrAmount(struct Text *text) {
    assert(text != nullptr);

    uint32_t strCount = 0;
    
    for (uint32_t curChr = 0; curChr < (text->bufSize); curChr++) {
        if (text->buffer[curChr] == '\n') {
            strCount++;
        }
    }
    text->strAmount = strCount;
}

void FillStrings(struct Text *text) {
    assert(text != nullptr);
	
    text->strings = (String*)calloc(text->strAmount + 10, sizeof(text->strings[0]));
    assert(text->strings != nullptr);

    text->strings[0].value = &text->buffer[0];
    for (size_t curStrBuf = 1, curStrIdx = 1; curStrBuf < text->bufSize; curStrBuf++) {
        if (text->buffer[curStrBuf - 1] == '\n') {
            text->strings[curStrIdx].firstSpaceIdx = 0;
            text->strings[curStrIdx].lastSpaceBeforeArgs = 0;

            text->strings[curStrIdx].value = &text->buffer[curStrBuf];
            text->strings[curStrIdx - 1].length = text->strings[curStrIdx].value - text->strings[curStrIdx - 1].value - 1;

            curStrIdx++;
        }
    }
    text->strings[text->strAmount - 1].length = &text->buffer[text->bufSize] - text->strings[text->strAmount - 1].value - 1;
}

void ProcessStrings(Text* text) {
    uint32_t lastStrNotSpace = 0;

    for (size_t curString = 0; curString < text->strAmount; curString++) {
        for (uint32_t curChar = 0; text->strings[curString].value[curChar] != '\0'; curChar++) {
            switch (text->strings[curString].value[curChar])
            {
            case ';':
                text->strings[curString].length = curChar + 1;
            case '\r':
            case '\n':
                text->strings[curString].value[curChar + 1] = '\0';
                text->strings[curString].value[curChar] = '\0';
                break;
            case ' ':
                break;    
            default:
                lastStrNotSpace = curChar;
                break;
            }
        }

        bool isInString = 0;
        for (uint32_t curChar = 0; text->strings[curString].value[curChar] != '\0'; curChar++) {
            if ((text->strings[curString].value[0] == ' ') || (text->strings[curString].value[0] == '\t')) {
                text->strings[curString].value[0] = '\0';
                text->strings[curString].value++;
                text->strings[curString].length--;
                lastStrNotSpace--;

                curChar = 0;
            }
            else if (text->strings[curString].value[curChar] == '"') {
                text->strings[curString].lenOfArgs += 1;
                isInString = !isInString;
            }
            else if (text->strings[curString].value[curChar] == ' ') {
                if (isInString) {
                    text->strings[curString].lenOfArgs += 1;
                }
                if (text->strings[curString].firstSpaceIdx == 0)
                    text->strings[curString].firstSpaceIdx = curChar;
                if ((curChar >= text->strings[curString].firstSpaceIdx)   & 
                    (text->strings[curString].value[curChar + 1] != ' ') &
                    (text->strings[curString].lastSpaceBeforeArgs == 0))
                    text->strings[curString].lastSpaceBeforeArgs = curChar;
                
                if (curChar > lastStrNotSpace)
                    text->strings[curString].value[curChar] = '\0';
            }
            else if ((curChar != 0) && (text->strings[curString].value[curChar] == '}')) {
                text->strings[curString].value[curChar] = '\0';
                text->strings[curString].length--;
            }
            else if (text->strings[curString].firstSpaceIdx != 0) {
                text->strings[curString].lenOfArgs += 1;
            }
            text->strings[curString].length = lastStrNotSpace + 1;
        }
    }
}

int MakeStrings(struct Text *text) {
	CountStrAmount(text);
	FillStrings(text);
    

	return 1;
}

void MakeText(Text* text, const char* inputFile) {
    assert(text != nullptr);

    ReadTextFromFile(text, inputFile);

    CountStrAmount(text);
	FillStrings(text);
    ProcessStrings(text);
}

void DestroyText(struct Text *text) {
    assert(text != nullptr);

    free(text->buffer);
    text->buffer = nullptr;

    free(text->strings);
    text->strings = nullptr;
}
