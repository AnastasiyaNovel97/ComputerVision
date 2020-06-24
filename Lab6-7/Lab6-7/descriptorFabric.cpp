#include "descriptorFabric.h"
#include "helper.h"
#include "pyramidfabric.h"
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


InterestingPoints::PointSet DescriptorFabric::SetOrientation(ImgMatrix gradientDirection, ImgMatrix gradientMagnitude,InterestingPoints::PointSet inputPoints)
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
    QVector<Descriptor> imgDescriptors;

    PyramidFabric pyrFabric(inputMatrix1);

    PyramidFabric::Pyramid origPyramid = pyrFabric.GetPyramid(3,5,0,1.6);
    PyramidFabric::Pyramid DoGPyramid;

    for(int i = 0; i < origPyramid.octaves.size(); i++)
    {
        PyramidFabric::Octave tempOctave;
        tempOctave.octaveLevel = i;


        for(int j = 0; j < origPyramid.octaves[i].levels.size()-1; j++)
        {
            PyramidFabric::Level tempLevel = origPyramid.octaves[i].levels[j];
            PyramidFabric::Level nextLevel = origPyramid.octaves[i].levels[j+1];


            for(int ii = 0; ii < tempLevel.matrix.Width(); ii++)
                for(int jj = 0; jj < tempLevel.matrix.Height(); jj++)
                    tempLevel.matrix.SetValue(ii,jj, abs(tempLevel.matrix.GetValue(ii,jj)-nextLevel.matrix.GetValue(ii,jj)));

            tempOctave.levels.append(tempLevel);
        }

        DoGPyramid.octaves.append(tempOctave);
    }

    QVector<QVector<InterestingPoints::PointSet>> pyramidPoints;

    for(int i=0; i < DoGPyramid.octaves.size(); i++)
    {
        QVector<InterestingPoints::PointSet> octavePoints;

        for(int j=1; j < DoGPyramid.octaves[i].levels.size()-1; j++)
        {
            PyramidFabric::Level DoGLevel = DoGPyramid.octaves[i].levels[j];

            InterestingPoints::PointSet currLevelPoints;
            InterestingPoints myIntPoints(origPyramid.octaves[i].levels[j-1].matrix);

            ImgMatrix currHarrisMatrix = myIntPoints.HarrisResponse(harrisRadius);
            currHarrisMatrix.NormalizeDouble();


            for(int ii = 0; ii < DoGLevel.matrix.Width(); ii++)
            {
                for(int jj = 0; jj < DoGLevel.matrix.Height(); jj++)
                {
                    double DoGVal = DoGLevel.matrix.GetValue(ii,jj);


                    if(DoGVal > 0.01)
                    {
                        bool isMin = true, isMax = true, working = true;


                        for(int lvl = -1; lvl <= 1 && working; lvl++)
                            for(int ix = -1; ix <= 1 && working; ix++)
                                for(int iy = -1; iy <= 1 && working; iy++)
                                    if(lvl != 0 || ix != 0 || iy != 0)
                                    {

                                        double tempVal = DoGPyramid.octaves[i].levels[j+lvl].matrix.GetValue(ii+ix,jj+iy);

                                        if(tempVal > DoGVal)
                                            isMax = false;

                                        if(tempVal < DoGVal)
                                            isMin = false;

                                        if(!isMin && !isMax)
                                            working = false;
                                    }

                        if(isMax || isMin)
                            if(currHarrisMatrix.GetValue(ii,jj) > 0.008)
                            {
                                InterestingPoints::InterestPoint currPoint(ii,jj,currHarrisMatrix.GetValue(ii,jj));
                                currLevelPoints.points.append(currPoint);
                            }
                    }
                }
            }
            octavePoints.append(currLevelPoints);
        }
        pyramidPoints.append(octavePoints);
    }

    int allPointsNum=0;

    for(int i=0; i < pyramidPoints.size(); i++)
    {
        int OctavePointsNum = 0;

        for(int j=0; j < pyramidPoints[i].size(); j++)
            OctavePointsNum += pyramidPoints[i][j].points.size();

        allPointsNum += OctavePointsNum;
    }

    for(int i=0; i < pyramidPoints.size(); i++)
    {
        for(int j=0; j < pyramidPoints[i].size(); j++)
        {
            int minimizePointsNum = harrisPointsNum * (double)pyramidPoints[i][j].points.size() / (double)allPointsNum;
            int maxRadius = qMin(inputMatrix1.Width(), inputMatrix1.Width()) / 2 / pow(2, i);

            MatrixConv myConv(origPyramid.octaves[i].levels[j+1].matrix);

            ImgMatrix derivateX = myConv.DerivateX();
            ImgMatrix derivateY = myConv.DerivateY();
            ImgMatrix sobleDirection = myConv.SobelDirection(derivateX, derivateY);
            ImgMatrix sobelMagnitude = myConv.Sobel(derivateX, derivateY);

            InterestingPoints myInterestPoints(origPyramid.octaves[i].levels[j+1].matrix);
            myInterestPoints.SetInterestingPoints(pyramidPoints[i][j]);
            myInterestPoints.MinimizePoints(minimizePointsNum,maxRadius);
            pyramidPoints[i][j] = myInterestPoints.GetInterestingPoints();

            int localHistSize = histogramGridSize * DoGPyramid.octaves[i].levels[j].localSigma / DoGPyramid.octaves[i].levels[0].localSigma;
            CalculateDescriptorsPyramid(sobleDirection,sobelMagnitude,pyramidPoints[i][j],localHistSize);

            QVector<Descriptor> levelDescriptors = descriptors1;

            int coordMult = pow(2., i);
            foreach(Descriptor descr, levelDescriptors){
                int xBig = static_cast<int>(descr.GetX() * coordMult);
                int yBig = static_cast<int>(descr.GetY() * coordMult);
                if(xBig >= inputMatrix1.Width())
                    xBig = inputMatrix1.Width()-1;
                if(yBig >= inputMatrix1.Height())
                    yBig = inputMatrix1.Height()-1;
                descr.SetX(xBig);
                descr.SetY(yBig);
                imgDescriptors.append(descr);
            }
        }
    }
    descriptors1 = imgDescriptors;
}


void DescriptorFabric::CalculateDescriptorsPyramid(ImgMatrix sobelDirection,ImgMatrix gradientMagnitude,InterestingPoints::PointSet pyramidPoints, int localHistogramGridSize)
{
    QVector<Descriptor> resultDescriptors;
    InterestingPoints::PointSet harrisPoints = pyramidPoints;
    harrisPoints = SetOrientation(sobelDirection,gradientMagnitude,harrisPoints);
    double basketSize = 360. / basketNum;
    int descriptorRadius = descriptorSize / 2 * localHistogramGridSize;
    foreach(InterestingPoints::InterestPoint keyPoint, harrisPoints.points)
    {
        int x = keyPoint.x;
        int y = keyPoint.y;
        int angle = keyPoint.angle;
        Descriptor pointDescriptor(basketNum,localHistogramGridSize,descriptorSize,x, y);
        for(int ih=-descriptorRadius; ih<descriptorRadius; ih++)
        {
            for(int jh=-descriptorRadius; jh<descriptorRadius; jh++)
            {
                if(sqrt(ih * ih + jh * jh) < sqrt(2)*descriptorRadius)
                {
                    double rotatedX = ih * cos(angle * M_PI / 180.0) +
                            jh * sin(angle * M_PI / 180.0);
                    double rotatedY = jh * cos(angle * M_PI / 180.0) -
                            ih * sin(angle * M_PI / 180.0);
                    rotatedX = (rotatedX > descriptorRadius) ? descriptorRadius : ((rotatedX < -descriptorRadius) ? -descriptorRadius : rotatedX);
                    rotatedY = (rotatedY > descriptorRadius) ? descriptorRadius : ((rotatedY < -descriptorRadius) ? -descriptorRadius : rotatedY);

                    double currMagnitude = gradientMagnitude.GetValue(x+ih,y+jh);
                    double currDirection = sobelDirection.GetValue(x+ih,y+jh) - angle;
                    currDirection = (currDirection < 0) ? currDirection + 360 : currDirection;
                    currDirection = (currDirection >= 360) ? currDirection - 360 : currDirection;

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

                    double histCol = (rotatedX+descriptorRadius) / (histogramGridSize) - 0.1;
                    double histRow = (rotatedY+descriptorRadius) / (histogramGridSize) - 0.1;
                    QVector<int> histNums(4);
                    QVector<double> histW(4);
                    double sumW = 0;
                    for(int ii = 0; ii < 2; ii++)
                    {
                        for(int jj = 0; jj < 2; jj++)
                        {
                            int tempHistCol = floor(histCol + 0.5 * pow(-1,ii));
                            tempHistCol = tempHistCol >= descriptorSize ? descriptorSize-1 : (tempHistCol < 0 ? 0 : tempHistCol);

                            int tempHistRow = floor(histRow + 0.5 * pow(-1,jj));
                            tempHistRow = tempHistRow >= descriptorSize ? descriptorSize-1 : (tempHistRow < 0 ? 0 : tempHistRow);
                            histNums[ii*2+jj] = descriptorSize * tempHistRow + tempHistCol;
                            histW[ii*2+jj] = sqrt(pow(histCol-tempHistCol,2) + pow(histRow-tempHistRow,2));
                            sumW += histW[ii*2+jj];
                        }
                    }

                    for(int ii = 0; ii < histNums.size(); ii++)
                    {
                        pointDescriptor.addToBasket(histNums[ii], basket1, currMagnitude * b1Weight * histW[ii] / sumW);
                        pointDescriptor.addToBasket(histNums[ii], basket2, currMagnitude * b2Weight * histW[ii] / sumW);
                    }

                }
            }
        }
        pointDescriptor.Normalize();
        resultDescriptors.append(pointDescriptor);
    }
    descriptors1 = resultDescriptors;
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


QImage DescriptorFabric::CompareDescriptor(QImage image1, QImage image2, QVector<Descriptor> descriptors2)
{
    //ImgMatrix resultGSMatrix = Merge(inputMatrix2);
    //QImage resultImage = resultGSMatrix.getImage();
    QImage resultImage(image1.width()+image2.width(),qMax(image1.height(), image2.height()),QImage::Format_RGB32);
    QPainter painter (&resultImage);
    painter.drawImage(0,0,image1);
    painter.drawImage(image1.width(),0,image2);
    painter.setPen(QColor(255, 255, 255, 255));
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
            int r = rand() % 200+56;
            int g = rand() % 200+56;
            int b = rand() % 200+56;
            painter.setPen(QColor(r,g,b, 255));

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


QImage DescriptorFabric::CompareDescriptor(ImgMatrix inputMatrix2, QVector<Descriptor> descriptors2)
{
    ImgMatrix resultGSMatrix = Merge(inputMatrix2);
    QImage resultImage = resultGSMatrix.getImage();
    QPainter painter (&resultImage);
    painter.setPen(QColor(255, 255, 255, 200));
    QList<int> foundList;
    int w = inputMatrix1.Width();
  //  int h = inputMatrix1.Height();

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
            int r = rand() % 176+80;
            int g = rand() % 176+80;
            int b = rand() % 176+80;
            painter.setPen(QColor(r,g,b, 255));

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
