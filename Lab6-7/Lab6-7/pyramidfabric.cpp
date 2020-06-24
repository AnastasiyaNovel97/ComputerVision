#include "pyramidfabric.h"
#include <QDebug>

PyramidFabric::PyramidFabric(ImgMatrix inputMatrix)
{
    this->inputMatrix = inputMatrix;
}

ImgMatrix PyramidFabric::Downsample(ImgMatrix workMatrix)
{
    int     outW = workMatrix.Width()/2,
            outH = workMatrix.Height()/2;
    ImgMatrix resMatrix(outW,outH);

    for(int i=0; i < outH; i++)
    {
        for(int j=0; j < outW; j++)
        {
            resMatrix.SetValue(j,i,workMatrix.GetValue(j*2,i*2));
        }
    }
    return resMatrix;
}


PyramidFabric::Pyramid PyramidFabric::GetPyramid(int octavesNum, int levelsNum, double sigmaA, double sigma0)
{
    Pyramid resPyramid;
    double levelInterval = pow(2.0, 1.0 / levelsNum);
    double sigmaB = sqrt(sigma0 * sigma0 - sigmaA * sigmaA);

    MatrixConv myConv(inputMatrix);

    ImgMatrix workMatrix = myConv.Gauss(sigmaB);
    myConv.SetMatrix(workMatrix);

    double actSigma = sigma0;
    for(int i = 0; i < octavesNum; i++)
    {
        double testSigma = sigma0;
        double currSigma = sigma0;
        Octave currOctave;
        currOctave.octaveLevel = i;
        Level currLevel(workMatrix,currSigma,actSigma);
        currOctave.levels.append(currLevel);
        qDebug() <<endl<< "Octave " << i << endl;
        qDebug()<<"currSigma = " << currSigma << "; actSigma = " << actSigma<<endl;

        ImgMatrix downsampleMatrix(workMatrix);
        for (int j = 1; j < levelsNum+3; j++)
        {
            sigmaB = sqrt(pow(currSigma*levelInterval,2) - pow(currSigma,2));
            testSigma = sqrt(testSigma*testSigma + sigmaB*sigmaB);
            currSigma *= levelInterval;
            actSigma *= levelInterval;
            qDebug()<<"currSigma = " << currSigma << "; actSigma = " << actSigma<<endl;
            workMatrix = myConv.Gauss(sigmaB);
            myConv.SetMatrix(workMatrix);

            currLevel.matrix = workMatrix;
            currLevel.localSigma = currSigma;
            currLevel.effectiveSigma = actSigma;
            currOctave.levels.append(currLevel);
            if(j == levelsNum-1)
                downsampleMatrix = workMatrix;
        }
        resPyramid.octaves.append(currOctave);

        if (i < octavesNum - 1)
        {
            workMatrix =  Downsample(downsampleMatrix);
            myConv.SetMatrix(workMatrix);
        }
    }

    return resPyramid;
}


ImgMatrix PyramidFabric::GetL(PyramidFabric::Pyramid inputPyramid, double sigma)
{
    Level targetLevel = inputPyramid.octaves[0].levels[0];
    int     octaveLevel = 0,
            octaveCount = 0;
    foreach(Octave currOctave, inputPyramid.octaves)
    {
        foreach(Level currLevel, currOctave.levels)
        {
            if(fabs(currLevel.effectiveSigma - sigma) < fabs(targetLevel.effectiveSigma - sigma))
            {
                targetLevel = currLevel;
                octaveLevel = octaveCount;
            }
        }
        octaveCount++;
    }
    int trans_coeff = pow(2., octaveLevel);
    ImgMatrix lMatrix(inputMatrix.Width(),inputMatrix.Height());
    for(int i=0; i<lMatrix.Height(); i++)
        for(int j=0; j<lMatrix.Width(); j++)
        {
            //преобразование координат
            int inew = static_cast<int>(i / trans_coeff);
            int jnew = static_cast<int>(j / trans_coeff);
            if(jnew >= targetLevel.matrix.Width()) jnew = targetLevel.matrix.Width()-1;
            if(inew >= targetLevel.matrix.Height()) inew = targetLevel.matrix.Height()-1;
            lMatrix.SetValue(j, i, targetLevel.matrix.GetValue(jnew,inew));
        }
    return lMatrix;
}
