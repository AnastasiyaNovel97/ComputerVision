#include "descriptor.h"

Descriptor::Descriptor(int basketNum, int histogramSize, int descriptorSize, int x, int y)
{
    this->basketNum = basketNum;
    this->histogramSize = histogramSize;
    this->descriptorSize = descriptorSize;
    this->x = x;
    this->y = y;


    QVector<QVector<double>> pointDescr;
    QVector<double> histogram;

    for(int i=0; i < basketNum; i++)
    {
        histogram.append(0.);
    }

    for(int j=0; j < descriptorSize*descriptorSize; j++)
    {
        pointDescr.append(histogram);
    }
    pointDescriptor = pointDescr;
}


double Descriptor::GetBasket(int histogramNum, int basketNum)
{
    return pointDescriptor[histogramNum][basketNum];
}


QVector<double> Descriptor::GetHistogram(int histogramNum)
{
    return pointDescriptor[histogramNum];
}


void Descriptor::SetHistogram(int histogramNum, QVector<double> histogram)
{
    pointDescriptor[histogramNum] = histogram;
}


void Descriptor::SetBasket(int histogramNum, int basketNum, double value)
{
    pointDescriptor[histogramNum][basketNum] = value;
}


void Descriptor::addToBasket(int histogramNum, int basketNum, double value)
{
    pointDescriptor[histogramNum][basketNum] = pointDescriptor[histogramNum][basketNum] + value;
}


int Descriptor::GetX()
{
    return x;
}


int Descriptor::GetY()
{
    return y;
}

void Descriptor::Normalize()
{
    for(int k=0; k<2; k++)
    {
        double length=0;

        for(int i =0; i<pointDescriptor.size(); i++)
            for(int j=0; j<pointDescriptor[i].size(); j++)
                length+= pointDescriptor[i][j]*pointDescriptor[i][j];

        length = sqrt(length);

        for(int i =0; i<pointDescriptor.size(); i++)
            for(int j=0; j<pointDescriptor[i].size(); j++)
            {
                pointDescriptor[i][j] /= length;

                if(pointDescriptor[i][j] > 0.2 && k==0)
                    pointDescriptor[i][j]=0.2;
            }
    }
}