//
// DlibClassificationInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/internal/dlib/DlibInterface.h"
#include "shared/algorithms/ml/interfaces/MLClassificationInterface.h"

template<typename ImagePixelType, 
    typename ClassificationFeatureExtractor,
    typename ClassificationResultType>
class DlibClassificationInterface: public MLClassificationInterface<DlibInterface<ImagePixelType>, ClassificationFeatureExtractor, ClassificationResultType>
{
public:
};
