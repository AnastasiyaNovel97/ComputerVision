#ifndef GAUSSPIRAMYDE_H
#define GAUSSPIRAMYDE_H

#include "imgMatrix.h"
#include "MatrixConv.h"

//Класс, создающий Гауссовы пирамиды
class PyramidFabric
{
private:
    ImgMatrix inputMatrix;//Исходное изображение, для которого строится пирамида
public:
    PyramidFabric(ImgMatrix inputMatrix);

//Уровень октавы
    struct Level{
        ImgMatrix matrix;
        double localSigma;//локальная сигма
        double effectiveSigma;//сигма, относительно исходного изображения
        Level(ImgMatrix inputmatrix,double currSigma, double actSigma):matrix(inputmatrix)
        {
            localSigma = currSigma;
            effectiveSigma = actSigma;
        }
    };

//Октава пирамиды
    struct Octave{
        QVector<Level> levels;
        int octaveLevel;
    };
//Пирамида собственной персоной :)
    struct Pyramid{
        QVector<Octave> octaves;
    };

    ImgMatrix Downsample(ImgMatrix workMatrix);//Уменьшение сторон изображения в 2 раза
    Pyramid GetPyramid(int octaveNum, int levelNum, double sigmaA, double sigma0);//Вычисление пирамиды
    ImgMatrix GetL(PyramidFabric::Pyramid inputPyramid, double sigma);//

};

#endif // GAUSSPIRAMYDE_H
