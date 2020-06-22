#include "InterestingPoints.h"
#include "imgMatrix.h"
#include "RussianText.h"
#include "descriptor.h"
#include "descriptorFabric.h"
#include <QtCore>
#include <iostream>

void lab5(QString path,QString fileName1, QString ext1,QString fileName2, QString ext2,int operatorRadius,int pointsNum,int basketNum,int histSize,int descriptorSize)
{
    QDir dir;
    QString labPath = path+"Lab5 "+fileName1+"&"+fileName2+"\\";
    dir.mkdir(labPath);

    Print("\nЗагрузка исходного изображения№1: "+fileName1+ext1);
    ImgMatrix inputMatrix1(path+fileName1+ext1);
    Print("\nЗагрузка исходного изображения№2: "+fileName2+ext2);
    ImgMatrix inputMatrix2(path+fileName2+ext2);

    Print("\nВычисляем дескриптор для изображения №1.");
    DescriptorFabric myDescr1(inputMatrix1,operatorRadius, pointsNum, basketNum, histSize, descriptorSize);
    myDescr1.CalculateDescriptors();
    QVector<Descriptor> descriptors1 = myDescr1.GetDescriptors();

    Print("\nВычисляем дескриптор для изображения №2.");
    DescriptorFabric myDescr2(inputMatrix2,operatorRadius, pointsNum, basketNum, histSize, descriptorSize);
    myDescr2.CalculateDescriptors();
    QVector<Descriptor> descriptors2 = myDescr2.GetDescriptors();

    QImage result = myDescr1.CompareDescriptor(inputMatrix2,descriptors2);
    result.save(labPath+"общие дескрипторы.jpg");
    Print("\n************************************************************************************************\n");
}


int main()
{
    lab5("D:\\ComputerVision\\Lab5\\","frog",".jpg","frog27",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","frog",".jpg","frog45",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","frog",".jpg","frog90",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","frog",".jpg","frog180",".jpg",3,400,8,16,4);

    lab5("D:\\ComputerVision\\Lab5\\","elf",".jpg","elf12",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","elf",".jpg","elf90",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","elf",".jpg","elf180",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","elf",".jpg","elf320",".jpg",3,400,8,16,4);

    lab5("D:\\ComputerVision\\Lab5\\","Lizard",".png","Lizard2",".png",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","Lizard",".png","Lizard3",".png",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","Lizard",".png","Lizard4",".png",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","Lizard",".png","Lizard5",".png",3,400,8,16,4);

    lab5("D:\\ComputerVision\\Lab5\\","school",".jpg","school2",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","school",".jpg","school3",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","school",".jpg","school4",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","school",".jpg","school5",".jpg",3,400,8,16,4);

    lab5("D:\\ComputerVision\\Lab5\\","Lena",".jpg","Lena2",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","Lena",".jpg","Lena3",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","Lena",".jpg","Lena4",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","Lena",".jpg","Lena5",".jpg",3,400,8,16,4);

    lab5("D:\\ComputerVision\\Lab5\\","city",".jpg","city2",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","city",".jpg","city3",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","city",".jpg","city4",".jpg",3,400,8,16,4);
    lab5("D:\\ComputerVision\\Lab5\\","city",".jpg","city5",".jpg",3,400,8,16,4);


     Print("\nВыполнение успешно завершено.\n");
    return 0;
}
