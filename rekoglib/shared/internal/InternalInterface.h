//
// InternalInterface.h
// Author: Michael Bao
// Date: 9/6/2015
//
#pragma once

#include "shared/utility/RekogObject.h"
#include "opencv2/core.hpp"
#include "Eigen/Core"

template<typename InternalImageType, template<class,int,int> class InternalMatrixType>
class InternalInteface: public virtual RekogObject
{
public:
    using ImageType = InternalImageType;
    virtual ImageType ConvertImageToInternalRepresentation(const cv::Mat inputImage) = 0;

    template<class T, int N, int M> using MatrixType = InternalMatrixType<T, N, M>;
};
