//
// DlibMLInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/algorithms/ml/MLInterface.h"
#include "dlib/opencv/cv_image.h"

template<typename ImagePixelType = unsigned char>
class DlibMLInterface: public MLInterface<dlib::cv_image<ImagePixelType>>
{
public:
    virtual dlib::cv_image<ImagePixelType> ConvertImageToInternalRepresentation(const cv::Mat inputImage) override
    {

        dlib::cv_image<ImagePixelType> newImage(inputImage);
        return newImage;
    }
};
