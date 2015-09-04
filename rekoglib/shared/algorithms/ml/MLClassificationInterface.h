//
// MLClasificationInterface.h
// Author: Michael Bao
// Date: 9/4/2015
//
#pragma once

#include "shared/algorithms/ml/MLInterface.h"
#include "shared/utility/type/RekogForceDerivedTemplate.h"

template<typename BaseInterface, 
    typename ClassificationFeatureExtractor,
    typename ClassificationResultType>
class MLClassificationInterface: public RekogForceDerivedTemplate<BaseInterface, MLInterface<typename BaseInterface::ImageType>>
{
    using ImageType = typename BaseInterface::ImageType;
public:
    using ClassificationFeatureType = typename ClassificationFeatureExtractor::FeatureType;
    virtual ClassificationFeatureType ExtractFeatures(const ImageType& image) const 
    {
        return ClassificationFeatureExtractor(image)();
    }

    virtual ClassificationResultType Classify(const ImageType&) const = 0;
    virtual void Train(const std::vector<ImageType, ClassificationResultType>&) = 0;

};
