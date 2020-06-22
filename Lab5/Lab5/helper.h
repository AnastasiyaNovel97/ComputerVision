#ifndef HELPER_H
#define HELPER_H

#include <QVector>
#include <QtMath>



class Helper
{
public:
    static QVector<QVector<double>> GaussCore(double sigma, int radius)
    {
        QVector<QVector<double>> gaussCore;
        double coeff = 1 / (2 * M_PI * sigma * sigma);
        double delitel = 2 * sigma * sigma;
        double sum = 0;
        for (int u = -radius; u <= radius; u++)
        {
            QVector<double> gaussRow;
            for (int v = -radius; v <= radius; v++)
            {
                double gaussValue = coeff * exp(- (u * u + v * v) / delitel);
                gaussRow.append(gaussValue);
                sum += gaussValue;
            }
             gaussCore.append(gaussRow);
        }
//        //Нормализуем ядро Гаусса
//        foreach(QVector<double> gaussRow, gaussCore)
//            foreach(double gaussValue, gaussRow)
//                gaussValue /= sum;

        return gaussCore;
    }
};

#endif // HELPER_H
