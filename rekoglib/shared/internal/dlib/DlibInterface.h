//
// DlibInterface.h
// Author: Michael Bao
// Date: 9/6/2015
//
#pragma once

#include "shared/internal/InternalInterface.h"
#include "dlib/opencv/cv_image.h"
#include "dlib/opencv/to_open_cv.h"

template<typename ImagePixelType>
class DlibInterface: public InternalInterface<dlib::cv_image<ImagePixelType>>
{
public:
    static dlib::cv_image<ImagePixelType> ConvertImageToInternalRepresentation(cv::Mat inputImage)
    {

        dlib::cv_image<ImagePixelType> newImage(inputImage);
        return newImage;
    }

    static cv::Mat ConvertInternalRepresentationToCV(dlib::cv_image<ImagePixelType> inputImage)
    {
        return dlib::toMat(inputImage);
    }
};

