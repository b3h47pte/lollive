//
// DlibClassificationInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/algorithms/ml/dlib/DlibMLInterface.h"
#include "shared/algorithms/ml/MLClassificationInterface.h"

template<typename ImagePixelType, 
    typename ClassificationFeatureExtractor,
    typename ClassificationResultType>
class DlibClassificationInterface: public MLClassificationInterface<DlibMLInterface<ImagePixelType>, ClassificationFeatureExtractor, ClassificationResultType>
{
public:
};
