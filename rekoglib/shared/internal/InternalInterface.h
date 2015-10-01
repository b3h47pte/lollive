//
// InternalInterface.h
// Author: Michael Bao
// Date: 9/6/2015
//
#pragma once

#include "shared/utility/RekogObject.h"
#include "opencv2/core.hpp"

template<typename InternalImageType>
class InternalInterface: public virtual RekogObject
{
public:
    using ImageType = InternalImageType;
    static ImageType ConvertImageToInternalRepresentation(cv::Mat)
    {
        return ImageType();
    }

    static cv::Mat ConvertInternalRepresentationToCV(ImageType)
    {
        return cv::Mat();
    }
};
