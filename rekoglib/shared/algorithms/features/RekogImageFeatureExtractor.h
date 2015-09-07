//
// RekogImageFeatureExtractor.h
// Author: Michael Bao
// Date: 9/4/2015
//
#pragma once

#include "shared/utility/RekogObject.h"
#include "opencv2/core.hpp"
#include "Eigen/Core"

template<typename InternalInterface, int N>
class RekogImageFeatureExtractor: public InternalInterface
{
public:
    using FeatureType = Eigen::Matrix<double, N, 1>;
    constexpr static int FeatureCount = N;

    virtual void LoadFromFile(const std::string&) {}
    virtual void SaveToFile(const std::string&) {}
    virtual void SetupFeatureExtractor(const std::vector<cv::Mat>&) {}

    virtual FeatureType operator()(const cv::Mat& inputImage) const
    {
        return ExtractFeature(inputImage);
    }

    virtual FeatureType ExtractFeature(const cv::Mat&) const = 0;
protected:
};
