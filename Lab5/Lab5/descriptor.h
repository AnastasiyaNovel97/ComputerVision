#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H
#include <QVector>
#include "InterestingPoints.h"

//Класс дескриптора интересной точки
class Descriptor
{
private:
    int x,y; // координаты интересной точки
    int basketNum; //число корзин
    int descriptorSize; //количество гистограмм в дескрипторе
    int histogramSize; //размер гистограммы

    QVector<QVector<double>> pointDescriptor;//Дескриптор интересной точки

public:
    Descriptor(int basketNum, int histogramSize, int descriptorSize, int x, int y);
    double GetBasket(int histogramNum, int basketNum);//получить значение корзины
    QVector<double> GetHistogram(int histogramNum);//получить гистограмму

    int GetBasketNum(){return basketNum;}//узнать число корзин
    int GetHistogramSize(){return histogramSize;}//узнать размер гистограммы
    int GetDescriptorSize(){return descriptorSize;}//узнать размер дескриптора
    //получить координаты интересной точки:
    int GetX();
    int GetY();

    void Normalize();//нормализовать дескриптор

    void SetHistogram(int histogramNum, QVector<double> histogram);//заменить гистограмму новой
    void SetBasket(int histogramNum, int basketNum, double value);//заменить корзину новой
    void addToBasket(int histogramNum, int basketNum, double value);//прибавить значение к существующей корзине


};

#endif // DESCRIPTOR_H
