#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QFile>
#include <QIODevice>
#include <QMessageBox>
#include <qxmlstream.h>
#include <QDebug>
#include <QXmlStreamReader>
#include <QDomDocument>
#include <QTextStream>
#include <string>
#include <iostream>

class Util
{
public:
    Util();
    static QFile* openFile(const char* path);
    static QXmlStreamReader *openXmlFileStream(const char* path);
    static QDomDocument *openXmlFileDom(const char* path);
    static void xmlDomParser(QDomDocument* xml);
    static void saveFile(const QString path, const QString text);

    static void addToLog(const char* text);
    static void saveLogToFile();
    static std::string log;
};

#endif // UTIL_H
