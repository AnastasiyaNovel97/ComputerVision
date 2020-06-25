#include "InterestingPoints.h"
#include "imgMatrix.h"
#include "RussianText.h"
#include "descriptor.h"
#include "descriptorFabric.h"
#include <QtCore>
#include <iostream>

void lab67(QString path,QString fileName1, QString ext1,QString fileName2, QString ext2,int operatorRadius,int pointsNum,int basketNum,int histSize,int descriptorSize)
{
    QDir dir;
    QString labPath = path+"Lab67 "+fileName1+"&"+fileName2+"\\";
    dir.mkdir(labPath);

    Print("\nЗагрузка исходного изображения№1: "+fileName1+ext1);
    ImgMatrix inputMatrix1(path+fileName1+ext1);
    QImage image1(path+fileName1+ext1);
    Print("\nЗагрузка исходного изображения№2: "+fileName2+ext2);
    ImgMatrix inputMatrix2(path+fileName2+ext2);
    QImage image2(path+fileName2+ext2);

    Print("\nВычисляем дескриптор для изображения №1.");
    DescriptorFabric myDescr1(inputMatrix1,operatorRadius, pointsNum, basketNum, histSize, descriptorSize);
    myDescr1.CalculateDescriptors();
    //QVector<Descriptor> descriptors1 = myDescr1.GetDescriptors();

    Print("\nВычисляем дескриптор для изображения №2.");
    DescriptorFabric myDescr2(inputMatrix2,operatorRadius, pointsNum, basketNum, histSize, descriptorSize);
    myDescr2.CalculateDescriptors();
    QVector<Descriptor> descriptors2 = myDescr2.GetDescriptors();

    Print("\nИщем похожие дескрипторы изображений.");
    QImage result = myDescr1.CompareDescriptor(inputMatrix2,descriptors2);//ч/б итоговое изображение
   // QImage result = myDescr1.CompareDescriptor(image1,image2,descriptors2);//цветное итоговое изображение
    result.save(labPath+"общие дескрипторы чб.jpg");
    Print("\n************************************************************************************************\n");
}


int main()
{
        lab67("D:\\ComputerVision\\Lab6-7\\","elf",".jpg","elf2",".jpg",3,2000,8,8,4);
        lab67("D:\\ComputerVision\\Lab6-7\\","elf",".jpg","elf3",".jpg",3,2000,8,8,4);
        lab67("D:\\ComputerVision\\Lab6-7\\","elf",".jpg","elf4",".jpg",3,2000,8,8,4);

        lab67("D:\\ComputerVision\\Lab6-7\\","frog",".jpg","frog2",".jpg",3,2000,8,8,4);
        lab67("D:\\ComputerVision\\Lab6-7\\","frog",".jpg","frog3",".jpg",3,2000,8,8,4);
        lab67("D:\\ComputerVision\\Lab6-7\\","frog",".jpg","frog4",".jpg",3,2000,8,8,4);

        lab67("D:\\ComputerVision\\Lab6-7\\","girl",".jpg","girl2",".jpg",3,2000,8,8,4);
        lab67("D:\\ComputerVision\\Lab6-7\\","girl",".jpg","girl3",".jpg",3,2000,8,8,4);
        lab67("D:\\ComputerVision\\Lab6-7\\","girl",".jpg","girl4",".jpg",3,2000,8,8,4);

        lab67("D:\\ComputerVision\\Lab6-7\\","bridge",".jpg","bridge2",".jpg",3,2000,8,8,4);

        lab67("D:\\ComputerVision\\Lab6-7\\","library",".jpg","library2",".jpg",3,2000,8,8,4);

        lab67("D:\\ComputerVision\\Lab6-7\\","Queen",".jpg","Queen2",".jpg",3,2000,8,8,4);

        lab67("D:\\ComputerVision\\Lab6-7\\","Queen",".jpg","Queen3",".jpg",3,2000,8,8,4);

        lab67("D:\\ComputerVision\\Lab6-7\\","Moscow",".jpg","Moscow2",".jpg",3,2000,8,8,4);



     Print("\nВыполнение успешно завершено.\n");
    return 0;
}
