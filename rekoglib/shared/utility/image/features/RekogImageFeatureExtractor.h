//
// RekogImageFeatureExtractor.h
// Author: Michael Bao
// Date: 9/4/2015
//
#pragma once

#include "shared/utility/RekogObject.h"
#include "opencv2/core.hpp"

template<typename ImageType, typename OutputFeatureType>
class RekogImageFeatureExtractor: public RekogObject
{
public:
    using FeatureType = OutputFeatureType;

    RekogImageFeatureExtractor(const ImageType& inputImage):
        storedImage(inputImage)
    {
    }

    virtual FeatureType operator()() const
    {
        return ExtractFeature();
    }

    virtual FeatureType ExtractFeature() const = 0;
protected:
    const ImageType storedImage;
};
