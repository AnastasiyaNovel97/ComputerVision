#include "imgMatrix.h"

ImgMatrix::ImgMatrix(QString path)
{
    LoadImage(path);
}

ImgMatrix::ImgMatrix(int width, int height)
{
    QVector<QVector<double>> matrix;
    for(int i=0; i < height; i++)
    {
        QVector<double> vector = QVector<double>(width);
        matrix.append(vector);
    }
    imgMatr = matrix;
}
ImgMatrix::ImgMatrix()
{

}
ImgMatrix::ImgMatrix(QVector<QVector<double>> newMatrix)
{
    imgMatr = newMatrix;
}

ImgMatrix::ImgMatrix(QVector<QVector<unsigned char>> newMatrix)
{
    SetFrom255(newMatrix);
}

QVector<QVector<unsigned char>> ImgMatrix::Get255()
{
    QVector<QVector<unsigned char>> resMatrix;
    int width = imgMatr[0].size();
    int height = imgMatr.size();

    double min=1,max=0;

    for(int i=0; i<imgMatr.size(); i++)
    {
        for(int j=0; j<imgMatr[0].size(); j++)
        {
            if(imgMatr[i][j]>max)
                max=imgMatr[i][j];
            if(imgMatr[i][j]<min)
                min=imgMatr[i][j];
        }
    }

    //нормализация выходных данных
    for (int i = 0; i < height; i++) {
        QVector<unsigned char> resLine;
        for (int j = 0; j < width; j++){
            resLine.append((double)(imgMatr[i][j]-min)*255/(max-min));
        }
        resMatrix.append(resLine);
    }
    return resMatrix;
}


QVector<QVector<double>> ImgMatrix::GetDouble()
{
    return imgMatr;
}

void ImgMatrix::NormalizeDouble()
{
    int width = imgMatr[0].size();
    int height = imgMatr.size();

    double min=1,max=0;

    for(int i=0; i<imgMatr.size(); i++)
    {
        for(int j=0; j<imgMatr[0].size(); j++)
        {
            if(imgMatr[i][j]>max)
                max=imgMatr[i][j];
            if(imgMatr[i][j]<min)
                min=imgMatr[i][j];
        }
    }

    //нормализация выходных данных
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            imgMatr[i][j] = (imgMatr[i][j]-min)/(max-min);
        }
    }
}

void ImgMatrix::SetFrom255(QVector<QVector<unsigned char>> inputMatrix)
{
    QVector<QVector<double>> resMatrix;
    double width = inputMatrix[0].size();
    double height = inputMatrix.size();
    for (int i = 0; i < height; i++) {
        QVector<double> resLine;
        for (int j = 0; j < width; j++) {
            resLine.append((double)inputMatrix[i][j]/255);
        }
        resMatrix.append(resLine);
    }
    imgMatr = resMatrix;
}


void ImgMatrix::LoadImage(QString path)
{
    QImage myImg1;
    QVector<QVector<unsigned char>> imgMatrix;
    if(myImg1.load(path))
    {
        QImage myImg = myImg1.convertToFormat(QImage::Format_RGB32);
        {
            for(int i = 0; i < myImg.height(); i++)
            {
                QVector<unsigned char> vectorX;

                for(int j = 0; j < myImg.width(); j++)
                {
                    //Перевод в ч-б
                    float brightness;
                    QColor pColor = myImg.pixelColor(j,i);
                    brightness = pColor.red()*0.299 + pColor.green()*0.587 + pColor.blue()*0.114;

                    vectorX.append(brightness);
                }
                imgMatrix.append(vectorX);
            }
        }
    }
    SetFrom255(imgMatrix);
 }

void ImgMatrix::SaveImage(QString path)
{
    QVector<QVector<unsigned char>> imgMatrix = Get255();
    QImage outImg = getImage();
    outImg.save(path);
}


QImage ImgMatrix::getImage()
{
    QVector<QVector<unsigned char>> imgMatrix = Get255();
    QImage outImg(imgMatrix.at(0).size(),imgMatrix.size(),QImage::Format_RGB32);


    for(int i=0; i<outImg.height(); i++)
    {
        for(int j=0; j<outImg.width(); j++)
        {
            unsigned char grayColor = imgMatrix[i][j];
            outImg.setPixel(j,i,qRgb(grayColor,grayColor,grayColor));
        }
    }
    return outImg;
}


double ImgMatrix::GetValue(int x, int y)
{
    //Если координаты лежат за границами массива - возвращаем значение граничного пикселя
    int i, j;
    i = x <= 0 ? 0 : (x >= imgMatr[0].size() - 1 ? imgMatr[0].size() - 1 : x);
    j = y <= 0 ? 0 : (y >= imgMatr.size() - 1 ? imgMatr.size() - 1 : y);
    return imgMatr[j][i];
}

void ImgMatrix::SetValue(int x, int y, double value)
{
    if(y>=0 && x>=0 && y<imgMatr.size() && x<imgMatr[0].size())
        imgMatr[y][x] = value;
}

int ImgMatrix::Width()
{
    return imgMatr[0].size();
}

int ImgMatrix::Height()
{
    return imgMatr.size();
}


