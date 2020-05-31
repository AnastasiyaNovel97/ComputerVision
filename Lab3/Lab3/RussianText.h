#define RUSSIANTEXT_H

#include <QString>
#include <QTextCodec>
#include <iostream>

void Print(QString inputString)
{
#ifdef Q_OS_WIN32
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
#endif
    std::cout << inputString.toLocal8Bit().data();
    //std::cout << QString::fromUtf8().toLocal8Bit().data();
}
