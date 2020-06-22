#ifndef KEYPOINTS_H
#define KEYPOINTS_H

#include <QVector>
#include "imgMatrix.h"
#include "MatrixConv.h"
#include <QDebug>

//Класс для поиска интересных точек
class InterestingPoints
{
public:
    InterestingPoints(ImgMatrix inputMatrix);
    struct InterestPoint
    {
        int x,y;
        double sLocal;
        int angle;
        InterestPoint(int x, int y, double sLocal)
        {
            this->x = x;
            this->y = y;
            this->sLocal = sLocal;
        }
    };
    //Набор интересных точек
    struct PointSet
    {
        QVector<InterestPoint> points;
        void Clear(){points = QVector<InterestPoint>();}
    };
    ImgMatrix Moravec(int windowRadius, int resultPointsNum);//Находит и отрисовывает интересные точки методом Моравека
    ImgMatrix Harris(int windowRadius, int resultPointsNum);//Находит и отрисовывает интересные точки методом Харриса
    void LocalMaximums(ImgMatrix responseMatrix, int windowRadius, bool isHarris);//Находит интересные точки на матрице отклика Харисса/Моравека(указывается булевым значением)
    void MinimizePoints(int resultPointsNum, int maxRadius);//Сокращает число точек до необходимого количества
    PointSet GetInterestingPoints(){return myInterestPoints;}//Возвращает набор интересных точек

private:
    ImgMatrix inputMatrix;//исходное изображение
    PointSet myInterestPoints;//интересные точки изображения

};

#endif // KEYPOINTS_H
