#include "InterestingPoints.h"
#include "imgMatrix.h"
#include "RussianText.h"
#include "descriptor.h"
#include "descriptorFabric.h"
#include <QtCore>
#include <iostream>

void lab4(QString path,QString fileName1, QString ext1,QString fileName2, QString ext2,int operatorRadius,int pointsNum,int basketNum,int histSize,int descriptorSize)
{
    QDir dir;
    QString labPath = path+"Lab4 "+fileName1+"&"+fileName2+"\\";
    dir.mkdir(labPath);

    Print("\nЗагрузка исходного изображения№1: "+fileName1+ext1);
    ImgMatrix inputMatrix1(path+fileName1+ext1);
    Print("\nЗагрузка исходного изображения№2: "+fileName2+ext2);
    ImgMatrix inputMatrix2(path+fileName2+ext2);

    Print("\nВычисляем дескриптор для изображения №1.");
    DescriptorFabric myDescr1(inputMatrix1);
    myDescr1.CalculateDescriptors(operatorRadius, pointsNum, basketNum, histSize, descriptorSize);
    QVector<Descriptor> descriptors1 = myDescr1.GetDescriptors();

    Print("\nВычисляем дескриптор для изображения №2.");
    DescriptorFabric myDescr2(inputMatrix2);
    myDescr2.CalculateDescriptors(operatorRadius, pointsNum, basketNum, histSize, descriptorSize);
    QVector<Descriptor> descriptors2 = myDescr2.GetDescriptors();

    QImage result = myDescr1.CompareDescriptor(inputMatrix2,descriptors2);
    result.save(labPath+"общие дескрипторы.jpg");
    Print("\n************************************************************************************************\n");
}


int main()
{
    lab4("D:\\ComputerVision\\Lab4\\","bicycle",".jpg","bicycle2",".jpg",2,400,8,16,4);
    lab4("D:\\ComputerVision\\Lab4\\","bunny",".jpg","bunny2",".jpg",2,400,8,16,4);
    lab4("D:\\ComputerVision\\Lab4\\","elf",".jpg","elf2",".jpg",2,400,8,16,4);
    lab4("D:\\ComputerVision\\Lab4\\","anime",".jpg","anime2",".jpg",2,400,8,16,4);
    lab4("D:\\ComputerVision\\Lab4\\","girl",".jpg","girl2",".jpg",2,400,8,16,4);


     Print("\nВыполнение успешно завершено.\n");
    return 0;
}
