//
// RekogImageFeatureExtractor.h
// Author: Michael Bao
// Date: 9/4/2015
//
#pragma once

#include "shared/utility/RekogObject.h"
#include "opencv2/core.hpp"

template<typename ImageType, int N>
class RekogImageFeatureExtractor: public RekogObject
{
public:
    using FeatureType = std::array<double, N>;
    const static int FeatureCount = N;
    virtual FeatureType operator()(const ImageType& inputImage) const
    {
        return ExtractFeature(inputImage);
    }

    virtual FeatureType ExtractFeature(const ImageType& inputImage) const = 0;
protected:
};
