#include "InterestingPoints.h"
#include "imgMatrix.h"
#include "RussianText.h"
#include <QtCore>
#include <iostream>


void lab3(QString path, QString fileName, QString ext,int windowRadius, int resultPointsNum)
{
    QDir dir;
    QString labPath = path+"Lab3 "+fileName+"\\";
    dir.mkdir(labPath);

    Print("\nЗагрузка исходного изображения: "+fileName+ext);
    ImgMatrix inputMatrix(path+fileName+ext);

    InterestingPoints myKeyFeat(inputMatrix);
    Print("\nНаходим интересные точки методом Моравека.");
    ImgMatrix moravecImage = myKeyFeat.Moravec(windowRadius, resultPointsNum);
    Print("\nРисуем интересные точки методом Моравека.");
    moravecImage.SaveImage(labPath+"Интересные точки методом Моравека"+ext);

    Print("\nНаходим интересные точки методом Харриса.");
    ImgMatrix harrisImage = myKeyFeat.Harris(windowRadius, resultPointsNum);
    Print("\nРисуем интересные точки методом Харриса.");
    harrisImage.SaveImage(labPath+"Интересные точки методом Харриса"+ext);
    Print("\n************************************************************************************************\n");

}


int main()
{
   // lab3("D:\\ComputerVision\\Lab3\\","bicycle",".jpg",2,3500);
   // lab3("D:\\ComputerVision\\Lab3\\","bunny",".jpg",1, 7500);
    //lab3("D:\\ComputerVision\\Lab3\\","elf",".jpg",3, 2000);
   // lab3("D:\\ComputerVision\\Lab3\\","anime",".jpg",3, 6700);
    //lab3("D:\\ComputerVision\\Lab3\\","anime2",".jpg",3, 4876);
    lab3("D:\\ComputerVision\\Lab3\\","Lena",".jpg",1, 1000);
     Print("\nВыполнение успешно завершено.\n");
    return 0;
}
