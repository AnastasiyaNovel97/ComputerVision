#include "InterestingPoints.h"

InterestingPoints::InterestingPoints(ImgMatrix inputMatrix)
{
    this->inputMatrix = inputMatrix;
}


ImgMatrix InterestingPoints::Moravec(int windowRadius, int resultPointsNum)
{
    int w = inputMatrix.Width();
    int h = inputMatrix.Height();

    ImgMatrix moravecMatrix(w,h);
    MatrixConv myConv(inputMatrix);
    ImgMatrix smoothMatrix = myConv.Gauss(1.3);

    for(int i=windowRadius+1; i < h-windowRadius-1; i++)
    {
        for(int j=windowRadius+1; j < w-windowRadius-1; j++)
        {
            double localCoeff = std::numeric_limits <double>::max();
            for(int iy = -1; iy <= 1; iy++)
            {
                for(int ix = -1; ix <= 1; ix++)
                {
                    if(ix!=0 && iy!=0)
                    {
                        double sum = 0;
                        for (int u = -windowRadius; u <= windowRadius; u++) {
                            for (int v = -windowRadius; v <= windowRadius; v++) {
                                double tmp =  smoothMatrix.GetValue(j,i) - smoothMatrix.GetValue(j+ix+u,i+iy+v);
                                sum += tmp * tmp;
                            }
                        }
                        localCoeff = std::min(sum,localCoeff);
                    }
                }
                moravecMatrix.SetValue(j,i,localCoeff);
            }
        }
    }

    LocalMaximums(moravecMatrix, windowRadius, false);

    int maxRadius = std::min(w/2,h/2);
    MinimizePoints(resultPointsNum, maxRadius);

    smoothMatrix = inputMatrix;
    foreach (interestPoint point, myInterestPoints.points) {
        for(int i=-1; i<=1; i++)
        {
            for(int j=-1; j<=1; j++)
            {
                if(i==0 || j==0)
                {
                    try
                    {
                        smoothMatrix.SetValue(point.x+j,point.y+i,1);
                    } catch(_exception e){}
                }
            }
        }
    }
    return smoothMatrix;
}


void InterestingPoints::LocalMaximums(ImgMatrix responseMatrix, int windowRadius, bool isHarris)
{
    myInterestPoints.Clear();
    int w = responseMatrix.Width();
    int h = responseMatrix.Height();
    responseMatrix.NormalizeDouble();
    double  min = std::numeric_limits <double>::max(),
            max = std::numeric_limits <double>::min();
    for(int i = 0; i < h; i++)
    {
         for(int j = 0; j < w; j++)
         {
             double temp = responseMatrix.GetValue(j,i);
            if (max < temp) max = temp;
            if (min > temp) min = temp;
        }
    }

    double thresholdCoeff = 0.005;
    if(isHarris) thresholdCoeff = 0.004;
    double threshold = min + (max - min) * thresholdCoeff;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            bool is_correct = true;
            double sLocal = responseMatrix.GetValue(j,i);
            for (int px = -windowRadius; px <= windowRadius && is_correct; px++) {
                for (int py = -windowRadius; py <= windowRadius && is_correct; py++) {
                    if (px != 0 || py != 0) {
                        is_correct = sLocal > responseMatrix.GetValue(j+px,i+py);
                    }
                }
            }
            if (is_correct && sLocal > threshold) {
                myInterestPoints.points.append(interestPoint(j,i,sLocal));
            }
        }
    }
}

void InterestingPoints::MinimizePoints(int resultPointsNum, int maxRadius)
{
    int r = 3;

    while (myInterestPoints.points.size() > resultPointsNum && r < maxRadius)
    {
        for(int i=0; i < myInterestPoints.points.size(); i++)
        {
            for(int j=0; j< myInterestPoints.points.size(); j++)
            {
                double xd = myInterestPoints.points[i].x - myInterestPoints.points[j].x;
                double yd = myInterestPoints.points[i].y - myInterestPoints.points[j].y;
                double dist = sqrt(xd * xd + yd * yd);
                if(dist <= r)
                {
                    if(myInterestPoints.points[i].sLocal < myInterestPoints.points[j].sLocal)
                    {
                        myInterestPoints.points.remove(i);
                        i--;
                        j = myInterestPoints.points.size();
                    }
                }
            }
        }
        r++;
    }

}


ImgMatrix InterestingPoints::Harris(int windowRadius, int resultPointsNum)
{
    ImgMatrix smoothMatrix = inputMatrix;
    MatrixConv myConv(smoothMatrix);
    smoothMatrix = myConv.Gauss(1.3);

    ImgMatrix derivateX = myConv.DerivateX();
    ImgMatrix derivateY = myConv.DerivateY();

    int w = smoothMatrix.Width();
    int h = smoothMatrix.Height();

    QVector<QVector<double>> a,b,c;

    double sigma = static_cast<double>(windowRadius*2) / 6;
    QVector<QVector<double>>gaussKernel;

    double coeff = 1 / (2 * M_PI * sigma * sigma);
    double delitel = 2 * sigma * sigma;

    for (int u = -windowRadius; u <= windowRadius; u++)
    {
        QVector<double> gaussRow;
        for (int v = -windowRadius; v <= windowRadius; v++)
        {
            gaussRow.append(coeff * exp(- (u * u + v * v) / delitel));
        }
        gaussKernel.append(gaussRow);
    }

    for (int i = 0; i < h; i++)
    {
        QVector<double> aRow,bRow,cRow;
         for (int j = 0; j < w; j++)
         {
            double sumA = 0, sumB = 0, sumC = 0;

            for (int u = -windowRadius; u <= windowRadius; u++)
            {
                for (int v = -windowRadius; v <= windowRadius; v++)
                {
                    double i_x = derivateX.GetValue(j + v, i + u);
                    double i_y = derivateY.GetValue(j + v, i + u);
                    sumA += i_x * i_x * gaussKernel[windowRadius+u][windowRadius+v];
                    sumB += i_x * i_y * gaussKernel[windowRadius+u][windowRadius+v];
                    sumC += i_y * i_y * gaussKernel[windowRadius+u][windowRadius+v];
                }
            }
            aRow.append(sumA);
            bRow.append(sumB);
            cRow.append(sumC);
        }
         a.append(aRow);
         b.append(bRow);
         c.append(cRow);
    }


    ImgMatrix harrisMatrix(w,h);

    for (int i =0; i < h; i++) {
        for (int j =0; j < w; j++) {
            double sc = a[i][j] + c[i][j];
            double d = a[i][j] * c[i][j] - b[i][j] * b[i][j];
            double det = sc * sc - 4 * d;
            double L1 = (sc + sqrt(det)) / 2;
            double L2 = (sc - sqrt(det)) / 2;
            double cHarris = std::min (L1, L2);
            harrisMatrix.SetValue(j, i, cHarris);
        }
    }

    LocalMaximums(harrisMatrix, windowRadius, true);

    MinimizePoints(resultPointsNum, std::min(w/2,h/2));

    smoothMatrix = inputMatrix;
    foreach (interestPoint point, myInterestPoints.points)
        for(int i=-1; i<=1; i++)
            for(int j=-1; j<=1; j++)
                if(i==0 || j==0)
                    smoothMatrix.SetValue(point.x+j,point.y+i,1);

    return smoothMatrix;
}


