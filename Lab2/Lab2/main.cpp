#include "imgMatrix.h"
#include "pyramidfabric.h"
#include "RussianText.h"
#include <QtCore>
#include <iostream>

void lab2(QString path, QString fileName, QString ext, int octavesNum, int levelsNum, double sigmaA, double sigma0, double sigmaL)
{
    QDir dir;
    QString pyramidPath = path+"Lab2 "+fileName+"\\";
    dir.mkdir(pyramidPath);
    Print("\nЗагрузка исходного изображения: "+fileName+ext);
    ImgMatrix inputmatrix(path+fileName+ext);
    Print("\nВычисляем пирамиду.");
    PyramidFabric myPyrFabric(inputmatrix);
    PyramidFabric::Pyramid myPyramid = myPyrFabric.GetPyramid(octavesNum, levelsNum, sigmaA, sigma0);

    Print("\nСохраняем пирамиду.");
    int octaveCount=0, levelCount=0;
    foreach(PyramidFabric::Octave currOctave, myPyramid.octaves)
    {
        levelCount=0;
        foreach(PyramidFabric::Level currLevel, currOctave.levels)
        {
            currLevel.matrix.SaveImage(pyramidPath+"октава "+QString::number(octaveCount)+", уровень "+QString::number(levelCount++)+ext);
        }
        octaveCount++;
    }

    Print("\nВычисляем изображение L(x,y,sigma).");
    ImgMatrix lMatrix = myPyrFabric.GetL(myPyramid, sigmaL);

    Print("\nСохранияем изображение L(x,y,sigma).");
    lMatrix.SaveImage(pyramidPath+"изображение L(x,y,sigma)"+ext);
    Print("\nПреобразования изображения завершены."
          "\nПолученные изображения сохранены в соответсвующую папку.");
    Print("\n************************************************************************************************\n");
}

int main()
{
//    lab2("D:\\ComputerVision\\Lab2\\","bicycle",".jpg",6,5,0,1,7.13);
//    lab2("D:\\ComputerVision\\Lab2\\","bicycle_copy",".jpg",6,7,0,1,7.13);

//    lab2("D:\\ComputerVision\\Lab2\\","bunny",".jpg",2,3,0,1,5.23);
//    lab2("D:\\ComputerVision\\Lab2\\","bunny_copy",".jpg",2,5,0,1,5.23);

//    lab2("D:\\ComputerVision\\Lab2\\","elf",".jpg",4,2,0,1,3.45);
//    lab2("D:\\ComputerVision\\Lab2\\","elf_copy",".jpg",4,7,0,1,3.45);

//    lab2("D:\\ComputerVision\\Lab2\\","anime",".jpg",5,2,0,1,6.36);
//    lab2("D:\\ComputerVision\\Lab2\\","anime_copy",".jpg",5,5,0,1,6.36);

//    lab2("D:\\ComputerVision\\Lab2\\","anime2",".jpg",3,3,0,1,3.36);
//    lab2("D:\\ComputerVision\\Lab2\\","anime2_copy",".jpg",3,5,0,1,3.36);

    lab2("D:\\ComputerVision\\Lab2\\","drive1",".jpg",3,3,0,1,3.36);
    lab2("D:\\ComputerVision\\Lab2\\","drive2",".jpg",3,5,0,1,3.36);


    Print("\nВыполнение успешно завершено.\n");
    return 0;
}
