#include "MatrixConv.h"

MatrixConv::MatrixConv(ImgMatrix inputMatrix)
{
    this->inputMatrix = inputMatrix;
}

void MatrixConv::SetMatrix(ImgMatrix inputMatrix)
{
    this->inputMatrix = inputMatrix;
}

ImgMatrix MatrixConv::Convolution(QVector<QVector<double>> core)
{
    int xRadius = core[0].size() / 2;
    int yRadius = core.size() / 2;

    int width = inputMatrix.Width();
    int height = inputMatrix.Height();

    ImgMatrix outputMatrix(width,height);

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            double sum = 0; //результат свертки для одной точки

            for (int u = -yRadius; u <= yRadius; u++)//для каждого ряда в ядре
                for (int v = -xRadius; v <= xRadius; v++)  //для каждого значения в ряду
                    sum += inputMatrix.GetValue(j - v, i - u) * core[u+yRadius][v+xRadius];
            outputMatrix.SetValue(j,i,sum);
        }

    return outputMatrix;
}


QVector<QVector<double>> MatrixConv::Multiplication(QVector<QVector<double>> matrix1, QVector<QVector<double>> matrix2)
{

    QVector<QVector<double>> resMatrix;
    for(int i = 0; i<matrix1.size(); i++)
    {
        QVector<double> matrixLine
                ;
        for(int j=0; j<matrix2[0].size(); j++)
        {
            double Sum=0;
            for(int k =0; k<matrix1.size(); k++)
            {
                Sum += matrix1[i][k]*matrix2[k][j];
            }
        }
    }
    return resMatrix;
}



ImgMatrix MatrixConv::DerivateX() //получить массивы с частными производными
{
    QVector<QVector<double>> core;
    core.append(QVector<double>({1,0,-1}));
    core.append(QVector<double>({2,0,-2}));
    core.append(QVector<double>({1,0,-1}));

    return Convolution(core);
}



ImgMatrix MatrixConv::DerivateY()
{
    QVector<QVector<double>> core;
    core.append(QVector<double>({1,2,1}));
    core.append(QVector<double>({0,0,0}));
    core.append(QVector<double>({-1,-2,-1}));
    return Convolution(core);
}



ImgMatrix MatrixConv::Sobel()
{
    ImgMatrix derivateX = DerivateX();
    ImgMatrix derivateY = DerivateY();

    int     width = inputMatrix.Width(),
            height = inputMatrix.Height();

    ImgMatrix sobelMatrix(width,height);

    double derXij, derYij, sobelij;

    for (int i = 0; i < height; i++)
    {//все строки

for (int j = 0; j < width; j++)
        {
            derXij = derivateX.GetValue(j,i);
            derYij = derivateY.GetValue(j,i);
            sobelij = sqrt(derXij*derXij + derYij*derYij);
            sobelMatrix.SetValue(j,i,sobelij);
        }
    }
    return sobelMatrix;
}


ImgMatrix MatrixConv::Gauss(double sigma)
{
    QVector<QVector<double> > core; //ядро свертки
    QVector<QVector<double> > core1;

    int sigmaInt = static_cast<int>(sigma) * 3;
    if (sigmaInt  % 2 == 0)
        sigmaInt++;
    double coeff = 2 * sigma * sigma;

    double sum = 0;
    QVector<double> str;
    for (int j = -sigmaInt; j <= sigmaInt; j++)
    {
        double gaussValue = exp( -(j * j) / coeff) / (M_PI * coeff);
        str.append(gaussValue);
        sum += gaussValue;
    }
    core.append(str);
    //Нормализуем ядро Гаусса
    foreach(QVector<double> row, core)
        foreach(double value, row)
            value /= sum;

    ImgMatrix resMatr = Convolution(core);

    sum = 0;
    for (int j = -sigmaInt; j <= sigmaInt; j++)
    {
        QVector<double> str1;
        double gaussValue = exp( -(j * j) / coeff) / (M_PI * coeff);
        str1.append(gaussValue);
        sum += gaussValue;
        core1.append(str);
    }
    //Нормализуем ядро Гаусса
    foreach(QVector<double> row, core1)
        foreach(double value, row)
            value /= sum;

    return Convolution(core1); //непосредственно вычисляем
}

ImgMatrix MatrixConv::SobelDirection()
{
    ImgMatrix derivateX = DerivateX();
    ImgMatrix derivateY = DerivateY();
    int width = derivateX.Width(), height = derivateX.Height();
    ImgMatrix resMatrix(width, height);

    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            resMatrix.SetValue(j,i, atan2(derivateY.GetValue(j,i), derivateX.GetValue(j,i)) * 180 / M_PI + 180);
        }

    }
    return resMatrix;
}

