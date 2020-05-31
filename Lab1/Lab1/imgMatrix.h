#ifndef IMAGEMATRIX_H
#define IMAGEMATRIX_H
#include <QVector>
#include <QImage>

//Черно-белая матрица исходного изображения. Каждый пиксель кодируется значением от 0 до 1 double
class ImgMatrix
{
public:
//Конструкторы:
    ImgMatrix();
    ImgMatrix(int width, int height);//создать матрицу размером w*h, заполненную нулями
    ImgMatrix(QString path);//загрузить в матрицу картинку из path
    ImgMatrix(QVector<QVector<double>> newMatrix);//инициализировать матрицу значениями newMatrix
    ImgMatrix(QVector<QVector<unsigned char>> newMatrix);//инициализировать матрицу значениями newMatrix

    QVector<QVector<unsigned char>> Get255();//выдать матрицу со значениями от 0 до 255
    QVector<QVector<double>> GetDouble();//выдать матрицу со значениями от 0 до 1 double

    void SetFrom255(QVector<QVector<unsigned char>> inputMatrix);// заполнить матрицу значениями newMatrix

    double GetValue(int x, int y);//получить значение пикселя по его координатам
    void SetValue(int x, int y, double value);//установить значение пикселя
//Получить значение ширины и высоты картинки
    int Width();
    int Height();
//Загрузить картинку из файла и сохранить её
    void LoadImage(QString path);
    void SaveImage(QString path);


private:
//Сама матрица, в которой хранятся значения пикселей
    QVector<QVector<double>> imgMatr;
};

#endif // IMAGEMATRIX_H
