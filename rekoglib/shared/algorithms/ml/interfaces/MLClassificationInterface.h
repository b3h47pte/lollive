//
// MLClasificationInterface.h
// Author: Michael Bao
// Date: 9/4/2015
//
#pragma once

#include "shared/algorithms/ml/interfaces/MLInterface.h"

template<typename BaseInterface, 
    typename ClassificationFeatureExtractor,
    typename ClassificationResultType>
class MLClassificationInterface: public MLInterface<BaseInterface>
{
    using ImageType = typename BaseInterface::ImageType;
public:
    using ClassificationFeatureType = typename ClassificationFeatureExtractor::FeatureType;
    virtual ClassificationFeatureType ExtractFeatures(const ImageType& image) const 
    {
        if (!featureExtractor) {
            REKOG_WARN("Please set the feature extractor using SetFeatureExtractor. Returning a value-initialized result, you will see wrong results.");
            return ClassificationFeatureType();
        }
        return (*featureExtractor)(image);
    }

    virtual ClassificationResultType Classify(const cv::Mat& image,double* = nullptr) const = 0;
    virtual void Train(const std::vector<std::pair<cv::Mat, ClassificationResultType>>&) = 0;

    void SetFeatureExtractor(const std::shared_ptr<ClassificationFeatureExtractor>& input)
    {
        featureExtractor = input;
    }
protected:
    std::shared_ptr<ClassificationFeatureExtractor> featureExtractor;
};
