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
    double width = imgMatr[0].size();
    double height = imgMatr.size();

    double min=0,max=1;

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
        for (int j = 0; j < width; j++) {
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
                    QColor pixelColor = myImg.pixelColor(j,i);
                    brightness = pixelColor.red()*0.299 + pixelColor.green()*0.587 + pixelColor.blue()*0.114;

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
    QImage outImg(imgMatrix.at(0).size(),imgMatrix.size(),QImage::Format_RGB32);


    for(int i=0; i<outImg.height(); i++)
    {
        for(int j=0; j<outImg.width(); j++)
        {
            unsigned char grayColor = imgMatrix[i][j];
            outImg.setPixel(j,i,qRgb(grayColor,grayColor,grayColor));
        }
    }
    outImg.save(path);
}


double ImgMatrix::GetValue(int x, int y)
{
    //Если координаты лежат за границами массива - возвращаем значение граничного пикселя
    int i = y < 0 ? i = 0 : (y > imgMatr.size()-1 ? i = imgMatr.size()-1 : i = y);
    int j = x < 0 ? j = 0 : (x > imgMatr[i].size()-1 ? j = imgMatr[i].size()-1 : j = x);
    return imgMatr.at(i).at(j);
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


