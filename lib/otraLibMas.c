#include "otraLibMas.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int strFindEndIn(char *strUniv, char *patern, int maxLen, int offset) {
    if (strUniv && patern && 0 <= maxLen && 0 <= offset) {
        int patLen = 0, i = 0, j = 0;
        patLen = strlen(patern);
        for (j = offset; j < maxLen && j <= maxLen - patLen; j++) {
            int ok = 1;
            for (i = 0; i < patLen && ok; i++)
                ok = (strUniv[j + i] == patern[i]);
            if (ok) {
                offset = j + patLen;
                j = maxLen + 1;
            } else if (j == maxLen - 1 || j == maxLen - patLen)
                offset = maxLen;
        }
    }
    return offset;
}

char *myVarStrCat(char *s, const char *format, ...) {
    char *var = NULL;
    int l = -1;
    va_list varList;
    va_start(varList, format);
    var = (char *) calloc(strlen(format) + 500, sizeof (char));
    vsprintf(var, format, varList);

    va_end(varList);
    if (s && var) {
        char *tmp = NULL;
        l = strlen(var);
        tmp = (char*) calloc(l + 50, sizeof (char));
        l = 1 + sprintf(tmp, "%s", var);
        s = (char *) realloc(s, (sizeof (char)) * (l + 10 + strlen(s)));
        s = strcat(s, tmp);
        free(tmp);
    }
    free(var);
    return s;
}

double dabs(double x) {
    if (x < 0.0)
        return -x;
    return x;
}
