//
// DlibInterface.h
// Author: Michael Bao
// Date: 9/6/2015
//
#pragma once

#include "shared/internal/InternalInterface.h"
#include "dlib/opencv/cv_image.h"

template<typename ImagePixelType>
class DlibInterface: public InternalInterface<dlib::cv_image<ImagePixelType>, dlib::matrix>
{
public:
    virtual dlib::cv_image<ImagePixelType> ConvertImageToInternalRepresentation(const cv::Mat inputImage) override
    {

        dlib::cv_image<ImagePixelType> newImage(inputImage);
        return newImage;
    }
};

