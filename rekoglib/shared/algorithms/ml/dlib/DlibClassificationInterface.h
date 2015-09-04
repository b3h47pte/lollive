//
// DlibClassificationInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/algorithms/ml/dlib/DlibMLInterface.h"
#include "shared/algorithms/ml/MLClassificationInterface.h"
#include "dlib/opencv/cv_image.h"

template<typename ImagePixelType = unsigned char>
class DlibClassificationInterface: public MLClassificationInterface<DlibMLInterface<ImagePixelType>>
{
public:
};
