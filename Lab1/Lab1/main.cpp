#include <QtCore>
#include "imgMatrix.h"
#include "MatrixConv.h"
#include <QString>
#include <iostream>
#include <QDir>
#include <QCoreApplication>
#include <QtDebug>
#include <QTextCodec>
#include <russiantext.h>

void lab1(QString path, QString fileName, QString ext, double sigma)
{
    Print("\nЗагрузка исходного изображения: "+fileName+ext);
    ImgMatrix origMatr(path+fileName);
    MatrixConv conv(origMatr);
    Print("\nЧёрно-белое изображение получено и сохранено.");
    origMatr.SaveImage(path+"\\Modified\\"+fileName+"(исходное изобр. ч-б)"+ext);

    Print("\nВычисляем производные по X...");
    ImgMatrix derivateX = conv.DerivateX();
    Print("\nЧастные производные по X для изображения получены и сохранены.");
    derivateX.SaveImage(path+"\\Modified\\"+fileName+"(частная производная по X)"+ext);

    Print("\nВычисляем производные по Y...");
    ImgMatrix derivateY = conv.DerivateY();
    Print("\nЧастные производные по Y для изображения получены и сохранены.");
    derivateY.SaveImage(path+"\\Modified\\"+fileName+"(частная производная по Y)"+ext);

    Print("\nВычисляем нормализованный градиент(оператор Собеля)...");
    ImgMatrix sobel = conv.Sobel();
    Print("\nНормализованный градиент для матрицы изображения вычислен, полученное изображение сохранено.");
    sobel.SaveImage(path+"\\Modified\\"+fileName+"(нормализованный градиент(Собель))"+ext);

    Print("\nВычисляем фильтр Гаусса...");
    ImgMatrix gauss = conv.Gauss(sigma);
    Print("\nФильтр Гаусса для матрицы изображения вычислен, полученное изображение сохранено.");
    gauss.SaveImage(path+"\\Modified\\"+fileName+"(фильтр Гаусса)"+ext);

    Print("\n************************************************************************************************\n");

}

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    lab1("D:\\ComputerVision\\Lab1\\","bicycle",".jpg",3);
    lab1("D:\\ComputerVision\\Lab1\\","bunny",".jpg",3);
    lab1("D:\\ComputerVision\\Lab1\\","elf",".jpg",3);
    lab1("D:\\ComputerVision\\Lab1\\","anime",".jpg",3);
    lab1("D:\\ComputerVision\\Lab1\\","anime2",".jpg",3);
      lab1("D:\\ComputerVision\\Lab1\\","city",".jpg",3);
    Print("\nПреобразования изображений закончены, изменения сохранены в папке Modified.");

    return a.exec();
}



