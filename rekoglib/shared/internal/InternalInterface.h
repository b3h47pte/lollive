//
// InternalInterface.h
// Author: Michael Bao
// Date: 9/6/2015
//
#pragma once

#include "shared/utility/RekogObject.h"
#include "opencv2/core.hpp"

template<typename InternalImageType>
class InternalInteface: public virtual RekogObject
{
public:
    using ImageType = InternalImageType;
    virtual ImageType ConvertImageToInternalRepresentation(const cv::Mat inputImage) = 0;
};
