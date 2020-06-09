#ifndef DESCRIPTORWORKER_H
#define DESCRIPTORWORKER_H
#include "descriptor.h"
#include "imgMatrix.h"

//Класс для создания дескрипторов
class DescriptorFabric
{
private:
    ImgMatrix inputMatrix1;//исходное изображение
    QVector<Descriptor> descriptors1;//набор дескрипторов
public:
    DescriptorFabric(ImgMatrix inputMatrix);

//Найти дескрипторы для изображения
    void CalculateDescriptors(int harrisRadius, int harrisPointsNum, int basketNum, int histogramGridSize, int descriptorSize);
//Сравнить дескрипторы текущего изображения с предложенным
    QImage CompareDescriptor(ImgMatrix inputMatrix2, QVector<Descriptor> descriptors2);
//Объединить текущее изображение с предложенным в одно
    ImgMatrix Merge(ImgMatrix inputMatrix2);
//Расстояние между двумя дескрипторами точек
    double Distance(Descriptor d1,Descriptor d2);

    QVector<Descriptor> GetDescriptors(){return descriptors1;}//Получить дескрипторы изображения

};

#endif // DESCRIPTORWORKER_H
