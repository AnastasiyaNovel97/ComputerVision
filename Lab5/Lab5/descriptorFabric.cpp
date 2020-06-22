#include "descriptorFabric.h"
#include "helper.h"
#include <QPainter>

DescriptorFabric::DescriptorFabric(ImgMatrix inputMatrix, int harrisRadius,int harrisPointsNum,
                                   int basketNum,int histogramGridSize,int descriptorSize)
{
    inputMatrix1 = inputMatrix;
    this->harrisRadius = harrisRadius;
    this->harrisPointsNum = harrisPointsNum;
    this->basketNum = basketNum;
    this->histogramGridSize = histogramGridSize;
    this->descriptorSize = descriptorSize;

}


double DescriptorFabric::Distance(Descriptor d1,Descriptor d2)
{
    double result = 0;
    int histogramsCount = d1.GetHistogramSize();
    int basketCount = d1.GetBasketNum();
    for(int i = 0; i < histogramsCount; i++){
        for(int j = 0; j < basketCount; j++){
            result += pow((d1.GetBasket(i,j) - d2.GetBasket(i,j)), 2);
        }
    }
    return sqrt(result);
}

double DescriptorFabric::DistancePoints(int x1, int y1, int x2, int y2)
{
   return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}


InterestingPoints::PointSet DescriptorFabric::OrientPoints(ImgMatrix gradientDirection, ImgMatrix gradientMagnitude,InterestingPoints::PointSet inputPoints)
{
    InterestingPoints::PointSet orientedPoints;
    int localBasketCount = 36;  //число корзин
    double localBasketSize = 360.0 / localBasketCount;  //охват корзины
    int descriptorRadius = descriptorSize / 2 * histogramGridSize;
    //int histogramRadius = histogramGridSize / 2;


    //находим ядро Гаусса
    QVector<QVector<double>> gaussCore = Helper::GaussCore(descriptorRadius / 6, descriptorRadius);

    for(int index = 0; index < inputPoints.points.size(); index++) {

        double localBaskets[localBasketCount];
        for (int i = 0; i < localBasketCount; i++)
            localBaskets[i] = 0;

        InterestingPoints::InterestPoint currPoint = inputPoints.points[index];

        for(int i = -descriptorRadius; i <= descriptorRadius; i++){
            for(int j = -descriptorRadius; j <= descriptorRadius; j++){

                //В пределах ?
                if(sqrt(i * i + j * j) < sqrt(2)*descriptorRadius){

                    //Направление Фи
                    double direction =  gradientDirection.GetValue(currPoint.x + i, currPoint.y + j);

                    //в какую корзину пишем
                    double basketBetw = direction / localBasketSize;

                    int basket1 = floor(basketBetw);
                    double b1Weight = 1;

                    int basket2 = ceil(basketBetw);
                    double b2Weight = 0;

                    if(basketBetw < basket1 + 0.5)
                    {
                        basket2 = basket1 - 1;
                        if(basket2 < 0) basket2 = localBasketCount - 1;

                        b1Weight = abs(basketBetw - floor(basketBetw) + 0.5);
                    }
                    else
                    {
                        basket2 = basket1 + 1;
                        if(basket2 > localBasketCount - 1) basket2 = 0;

                        b1Weight = abs(basketBetw - floor(basketBetw) - 0.5);

                    }
                    b2Weight = 1. - b1Weight;

                    double currMagnitude = gradientMagnitude.GetValue(currPoint.x + i,currPoint.y + j);

                    localBaskets[basket1] += currMagnitude * b1Weight * gaussCore[(i + descriptorRadius)][(j + descriptorRadius)];

                    localBaskets[basket2] += currMagnitude * b2Weight * gaussCore[(i + descriptorRadius)][(j + descriptorRadius)];
                }
            }
        }

        double firstMaxValue = -1;
        int firstMaxIndex = -1;
        double secondMaxValue = -1;
        int secondMaxIndex = -1;

        //ищем первую и вторую максимальную
        for(int i = 0; i < localBasketCount; i++){
            if(localBaskets[i] > firstMaxValue){
                secondMaxValue = firstMaxValue;
                secondMaxIndex = firstMaxIndex;

                firstMaxValue = localBaskets[i];
                firstMaxIndex = i;
            } else {
                if(localBaskets[i] > secondMaxValue){
                    secondMaxValue = localBaskets[i];
                    secondMaxIndex = i;
                }
            }
        }

        //добавляем первую
        InterestingPoints::InterestPoint firstPoint(inputPoints.points[index]);
        firstPoint.angle = (firstMaxIndex * localBasketSize);
        orientedPoints.points.push_back(firstPoint);

        //если вторая корзина >= 0.8 от макс значения, то добваляем ее тоже
        if(secondMaxValue >= (firstMaxValue * 0.8)){
            InterestingPoints::InterestPoint otherPoint(inputPoints.points[index]);
            otherPoint.angle = (secondMaxIndex * localBasketSize);
            orientedPoints.points.push_back(otherPoint);
        }

    }
    return orientedPoints;
}



void DescriptorFabric::CalculateDescriptors()
{
    QVector<Descriptor> imageDescriptor;
    InterestingPoints pointFabr(inputMatrix1);
    pointFabr.Harris(harrisRadius, harrisPointsNum);
    InterestingPoints::PointSet harrisPoints = pointFabr.GetInterestingPoints();

    MatrixConv myConv(inputMatrix1);
    ImgMatrix dx = myConv.DerivateX();
    ImgMatrix dy = myConv.DerivateY();
    ImgMatrix gradientDirection = myConv.SobelDirection();
    ImgMatrix gradientMagnitude = myConv.Sobel();
    harrisPoints = OrientPoints(gradientDirection,gradientMagnitude,harrisPoints);

    double basketSize = 360. / basketNum;
    int descriptorRadius = descriptorSize / 2 * histogramGridSize;

    //для каждой интересной точки
    foreach(InterestingPoints::InterestPoint keyPoint, harrisPoints.points)
    {
        int x = keyPoint.x;
        int y = keyPoint.y;
        int angle = keyPoint.angle;
        Descriptor pointDescriptor(basketNum,histogramGridSize,descriptorSize,x, y);
        for(int ih=-descriptorRadius; ih<descriptorRadius; ih++)
        {
            for(int jh=-descriptorRadius; jh<descriptorRadius; jh++)
            {
                if(sqrt(ih * ih + jh * jh) < sqrt(2)*descriptorRadius)
                {
                    int rotatedX = ih * cos(angle * M_PI / 180.0) +
                            jh * sin(angle * M_PI / 180.0);
                    int rotatedY = jh * cos(angle * M_PI / 180.0) -
                            ih * sin(angle * M_PI / 180.0);
                    rotatedX = (rotatedX > descriptorRadius) ? descriptorRadius : ((rotatedX < -descriptorRadius) ? -descriptorRadius : rotatedX);
                    rotatedY = (rotatedY > descriptorRadius) ? descriptorRadius : ((rotatedY < -descriptorRadius) ? -descriptorRadius : rotatedY);

                    double currMagnitude = gradientMagnitude.GetValue(x+ih,y+jh);
                    double currDirection = gradientDirection.GetValue(x+ih,y+jh) - angle;
                    currDirection = (currDirection < 0) ? currDirection + 360 : currDirection;
                    currDirection = (currDirection >= 360) ? currDirection - 360 : currDirection;
                    int histRowNum = ((rotatedX + descriptorRadius) / histogramGridSize - 0.1);
                    int histColNum = ((rotatedY + descriptorRadius) / histogramGridSize - 0.1);
                    int currHist =  descriptorSize * histRowNum + histColNum;

                    double basketBetw = currDirection / basketSize;
                    int basket1 = floor(basketBetw);
                    double b1Weight = 1;
                    int basket2 = ceil(basketBetw);
                    double b2Weight = 0;

                    //если на границе с 0 или 360 градусов
                    if(basketBetw < basket1 + 0.5)
                    {
                        basket2 = basket1 - 1;
                        if(basket2 < 0) basket2 = basketNum - 1;

                        b1Weight = abs(basketBetw - floor(basketBetw) + 0.5);
                    }
                    else
                    {
                        basket2 = basket1 + 1;
                        if(basket2 > basketNum - 1) basket2 = 0;

                        b1Weight = abs(basketBetw - floor(basketBetw) - 0.5);

                    }
                    b2Weight = 1. - b1Weight;

                    pointDescriptor.addToBasket(currHist, basket1, currMagnitude*b1Weight);
                    pointDescriptor.addToBasket(currHist, basket2, currMagnitude*b2Weight);

                }
            }
        }
        pointDescriptor.Normalize();
        imageDescriptor.append(pointDescriptor);
    }
    descriptors1 = imageDescriptor;
}


ImgMatrix DescriptorFabric::Merge(ImgMatrix inputMatrix2)
{
    ImgMatrix resultGSMatrix(inputMatrix1.Width()+inputMatrix2.Width(),
                                   std::max(inputMatrix1.Height(), inputMatrix2.Height()));

    for(int i=0; i<inputMatrix1.Height(); i++)
    {
        for(int j=0; j< inputMatrix1.Width(); j++)
        {
            resultGSMatrix.SetValue(j,i,inputMatrix1.GetValue(j,i));
        }
    }
    for(int i=0; i<inputMatrix2.Height(); i++)
    {
        for(int j=0; j< inputMatrix2.Width(); j++)
        {
            resultGSMatrix.SetValue(j+inputMatrix1.Width(),i,inputMatrix2.GetValue(j,i));
        }
    }
    return resultGSMatrix;
}

QImage DescriptorFabric::CompareDescriptor(ImgMatrix inputMatrix2, QVector<Descriptor> descriptors2)
{
    ImgMatrix resultGSMatrix = Merge(inputMatrix2);
    QImage resultImage = resultGSMatrix.getImage();
    QPainter painter (&resultImage);
    painter.setPen(QColor(255, 255, 255, 200));
    QList<int> foundList;
    int w = inputMatrix1.Width();
    //int h = inputMatrix1.Height();

    QVector<QVector<double>> distMatrix;
    QVector<double> modVector;
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::min();
    double middleValue = 0;
    double modValue = 0;
    //Мин Макс и среднее
    for(int i = 0; i < descriptors1.size(); i++)
    {
        QVector<double> distRow;
        for(int j = 0; j < descriptors2.size(); j++)
        {
            double dist = Distance(descriptors1[i], descriptors2[j]);
            distRow.append(dist);
            modVector.append(dist);
            middleValue += dist;
            if(dist < minValue){
                minValue = dist;
            }
            if(dist > maxValue){
                maxValue = dist;
            }
        }
        distMatrix.append(distRow);
    }
    middleValue /= descriptors1.size() * descriptors2.size();
    std::sort(modVector.begin(), modVector.end());
    modValue = modVector[3*qMin(descriptors1.size(),descriptors2.size())];
    //Поиск соответствий
    for(int i = 0; i < descriptors1.size(); i++){
        double firstMinValue = std::numeric_limits<double>::max();
        int firstMinIndex = 0;
        double secondMinValue = std::numeric_limits<double>::max();
        //int secondMinIndex = 0;

        //ищем соотв. точку второй картинки
        for(int j = 0; j < descriptors2.size(); j++){
            double dist = distMatrix[i][j];
            if(dist < firstMinValue){
                secondMinValue = firstMinValue;
                //secondMinIndex = firstMinIndex;

                firstMinValue = dist;
                firstMinIndex = j;
            } else {
                if(dist < secondMinValue){
                    secondMinValue = dist;
                    //secondMinIndex = j;
                }
            }
        }

        //а теперь то же самое, но смотрим в обратную сторону. Чтобы для точки со второй картинки не было двух кандидатов из первой
        double firstMinValue2 = std::numeric_limits<double>::max();
        double secondMinValue2 = std::numeric_limits<double>::max();
        for(int j = 0; j < descriptors1.size(); j++){
            double dist = distMatrix[j][firstMinIndex];
            if(dist < firstMinValue2){
                secondMinValue2 = firstMinValue2;

                firstMinValue2 = dist;
            } else {
                if(dist < secondMinValue2){
                    secondMinValue2 = dist;
                }
            }
        }

        //берем точку если у нее NNDR < 0.8 (борьба с многозначностью). Также отбрасываем "ложные срабатывания"
        if(foundList.indexOf(firstMinIndex) == -1
                && firstMinValue <= modValue
                && firstMinValue / secondMinValue < 0.8
                && firstMinValue2 / secondMinValue2 < 0.8
                )
        {
            foundList.append(firstMinIndex);
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            painter.setPen(QColor(r,g,b, 170));

            for(int ii=-1; ii<=1; ii++)
            {
                for(int jj=-1; jj<=1; jj++)
                {
                    if(ii==0 || jj==0)
                    {
                        painter.drawPoint(descriptors1[i].GetX()+ii, descriptors1[i].GetY()+jj);
                        painter.drawPoint(descriptors2[firstMinIndex].GetX() + w +jj, descriptors2[firstMinIndex].GetY()+jj);

                    }
                }
            }


            painter.drawLine(descriptors1[i].GetX(), descriptors1[i].GetY(), descriptors2[firstMinIndex].GetX() + w, descriptors2[firstMinIndex].GetY());
        }
    }

//сохраняем
    return resultImage;

}
