#include "Tools.h"

void Log(QPlainTextEdit* plainTextEdit, LogLevel loglevel, QString text)
{
    QTextCharFormat fmt;
    QString str;
    //QDateTime timer = QDateTime::currentDateTime();
    switch (loglevel)
    {
    case ERR:
        fmt.setForeground(QBrush(QColor(252, 0, 0)));
        str = QDateTime::currentDateTime().toString("[hh:mm:ss | ERROR] ") + text;
        break;
    case WARN:
        fmt.setForeground(QBrush(QColor("yellow")));
        str = QDateTime::currentDateTime().toString("[hh:mm:ss | WARNING] ") + text;
        break;
    case INFO:
        fmt.setForeground(QBrush(QColor("white")));
        str = QDateTime::currentDateTime().toString("[hh:mm:ss | INFO] ") + text;
        break;
    case SUC:
        fmt.setForeground(QBrush(QColor(0, 252, 0)));
        str = QDateTime::currentDateTime().toString("[hh:mm:ss | SUCCESS] ") + text;
        break;
    }
    plainTextEdit->mergeCurrentCharFormat(fmt);
    plainTextEdit->appendPlainText(str);
}

char* LoadFileContext(const char* url)
{
    char* file = NULL;
    FILE* fp = fopen(url, "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        int len = ftell(fp);
        if (len > 0) {
            rewind(fp);
            file = new char[len + 1];
            fread(file, 1, len, fp);
            file[len] = 0;
        }
        fclose(fp);
    }
    return file;
}

