#ifndef CONVOLUTION_H
#define CONVOLUTION_H
#include "imgMatrix.h"

#include <QVector>
#include <QtMath>

//Класс для операций с ч-б матрицами изображений
class MatrixConv
{
private:
    ImgMatrix inputMatrix;//экземпляр ч-б матрицы

public:
    MatrixConv(ImgMatrix inputMatrix);//конструктор класса
    void SetMatrix(ImgMatrix inputMatrix);//заменить текущую матрицу
    ImgMatrix Convolution(QVector<QVector<double>> core);//свёртка изображения с ядром core
    QVector<QVector<double>> Multiplication(QVector<QVector<double>> matrix1, //умножение двух матриц друг на друга
                                           QVector<QVector<double>> matrix2);

    ImgMatrix DerivateX();//получить массивы с частными производными по X
    ImgMatrix DerivateY();//получить массивы с частными производными по Y
    ImgMatrix Sobel(ImgMatrix derivateX,ImgMatrix derivateY);
    ImgMatrix SobelDirection(ImgMatrix derivateX,ImgMatrix derivateY);
    ImgMatrix Gauss(double sigma);

};

#endif // CONVOLUTION_H
