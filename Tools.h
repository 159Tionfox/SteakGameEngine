#ifndef TOOLS_H
#define TOOLS_H

#include "Library.h"

enum LogLevel {
    ERR,
    WARN,
    INFO,
    SUC
};

void Log(QPlainTextEdit* plainTextEdit, LogLevel loglevel, QString text);

char* LoadFileContext(const char* url);

#endif // TOOLS_H
