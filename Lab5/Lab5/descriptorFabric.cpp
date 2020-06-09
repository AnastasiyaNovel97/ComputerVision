#include "descriptorFabric.h"
#include <QPainter>

DescriptorFabric::DescriptorFabric(ImgMatrix inputMatrix)
{
    inputMatrix1 = inputMatrix;
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


void DescriptorFabric::CalculateDescriptors(int harrisRadius,int harrisPointsNum,int basketNum,int histogramGridSize,int descriptorSize)
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

    double basketSize = 360. / basketNum;
    int descriptorRadius = descriptorSize / 2 * histogramGridSize;
    int histogramRadius = histogramGridSize / 2;


    //находим ядро Гаусса
    double sigma = static_cast<double>(histogramGridSize) / 6;
    QVector<QVector<double>> gaussKernel;

    double coeff = 1 / (2 * M_PI * sigma * sigma);
    double delitel = 2 * sigma * sigma;

    for (int u = -histogramRadius; u <= histogramRadius; u++)
    {
        QVector<double> gaussRow;
        for (int v = -histogramRadius; v <= histogramRadius; v++)
        {
            gaussRow.append( coeff * exp(- (u * u + v * v) / delitel));
        }
         gaussKernel.append(gaussRow);
    }


    int pointCount=0;
    //для каждой интересной точки
    foreach(InterestingPoints::interestPoint keyPoint, harrisPoints.points)
    {
        pointCount++;
        int     x = keyPoint.x,
                y = keyPoint.y;
        Descriptor pointDescriptor(basketNum,histogramGridSize,descriptorSize,x, y);
        for(int ih=-descriptorRadius; ih<descriptorRadius; ih++)
        {
            for(int jh=-descriptorRadius; jh<descriptorRadius; jh++)
            {
                int     currX=x+jh, //текущий х в сетке
                        currY=y+ih; //текущий у в сетке

                double currDirection = gradientDirection.GetValue(currX,currY);
                currDirection = (currDirection < 0) ? currDirection + 360 : currDirection;
                currDirection = (currDirection >= 360) ? currDirection - 360 : currDirection;
                double currMagnitude = gradientMagnitude.GetValue(currX,currY);

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

                //определяем гистограмму, в которую записываем значение
                int histRowNum = (ih + descriptorRadius) / descriptorSize / histogramRadius;
                int histColNum = (jh + descriptorRadius)  / descriptorSize / histogramRadius;
                int currHist =  descriptorSize * histRowNum + histColNum;

                pointDescriptor.addToBasket(currHist, basket1, currMagnitude*b1Weight * gaussKernel[(ih+descriptorRadius)/(descriptorSize)][(jh+descriptorRadius)/(descriptorSize)]);
                pointDescriptor.addToBasket(currHist, basket2, currMagnitude*b2Weight * gaussKernel[(ih+descriptorRadius)/(descriptorSize)][(jh+descriptorRadius)/(descriptorSize)]);

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
    int w = inputMatrix1.Width();
   // int h = inputMatrix1.Height();

    QVector<QVector<double>> distMatrix;
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::min();
    double middleValue = 0;

    //Мин Макс и среднее
    for(int i = 0; i < descriptors1.size(); i++)
    {
        QVector<double> distRow;
        for(int j = 0; j < descriptors2.size(); j++)
        {
            double dist = Distance(descriptors1[i], descriptors2[j]);
            distRow.append(dist);

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

        //берем точку если у нее NNDR < 0.6 (борьба с многозначностью). Также отбрасываем "ложные срабатывания"
        if(firstMinValue / secondMinValue < 0.6 && firstMinValue2 / secondMinValue2 < 0.6 && firstMinValue < middleValue * 0.1){

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
