#ifndef DESCRIPTORWORKER_H
#define DESCRIPTORWORKER_H
#include "descriptor.h"
#include "imgMatrix.h"

//Класс для создания дескрипторов
class DescriptorFabric
{
private:
    ImgMatrix inputMatrix1;//исходное изображение
    int harrisRadius;
    int harrisPointsNum;
    int basketNum;
    int histogramGridSize;
    int descriptorSize;

    QVector<Descriptor> descriptors1;//набор дескрипторов
public:
    DescriptorFabric(ImgMatrix inputMatrix,int harrisRadius, int harrisPointsNum, int basketNum, int histogramGridSize, int descriptorSize);

//Определить угол поворота для каждой интересной точки
    InterestingPoints::PointSet SetOrientation(ImgMatrix gradientDirection, ImgMatrix gradientMagnitude,InterestingPoints::PointSet inputPoints);
//Найти дескрипторы для изображения
    void CalculateDescriptors();
//Найти дексрипторы для одного уровня одной октавы
    void CalculateDescriptorsPyramid(ImgMatrix gradientDirection,ImgMatrix gradientMagnitude,InterestingPoints::PointSet pyramidPoints, int localHistogramGridSize);
//Сравнить дескрипторы текущего изображения с предложенным
    QImage CompareDescriptor(ImgMatrix inputMatrix2, QVector<Descriptor> descriptors2);
    QImage CompareDescriptor(QImage image1, QImage image2, QVector<Descriptor> descriptors2);
//Объединить текущее изображение с предложенным в одно
    ImgMatrix Merge(ImgMatrix inputMatrix2);
//Расстояние между двумя дескрипторами точек
    double Distance(Descriptor d1,Descriptor d2);
    double DistancePoints(int x1, int y1, int x2, int y2);


    QVector<Descriptor> GetDescriptors(){return descriptors1;}//Получить дескрипторы изображения


};

#endif // DESCRIPTORWORKER_H
