//
// DlibSURFInterface.h
// Author: Michael Bao
// Date: 9/8/2015
// 
#pragma once

#include "shared/algorithms/features/surf/SURFInterface.h"
#include "shared/internal/dlib/DlibInterface.h"
#include "shared/utility/type/dlib/DlibEigenInterop.h"
#include "shared/utility/type/dlib/DlibCVInterop.h"
#include "dlib/image_keypoint.h"

template<typename ImagePixelType,
    typename ClusteringInterface,
    int N>
class DlibSURFInterface: public SURFInterface<ClusteringInterface, DlibInterface<ImagePixelType>, N>
{
    using Base = SURFInterface<ClusteringInterface, DlibInterface<ImagePixelType>, N>;
    using ImageType = typename Base::ImageType;
    using SurfFeatureType = typename Base::SurfFeatureType;
    using SURFAggregate = typename Base::SURFAggregate;
public:
    // Defaults are the same as Dlib's.
    DlibSURFInterface():
        internalMaxPoints(10000), internalDetectionThreshold(30.0)
    {
    }

private:
    virtual void ComputeSURFFeatures(const cv::Mat& inputImage, SURFAggregate& aggregate)
    {
        ImageType internalImage = this->ConvertImageToInternalRepresentation(inputImage);
        std::vector<dlib::surf_point> features = dlib::get_surf_points(internalImage, internalMaxPoints, internalDetectionThreshold);
        tbb::parallel_for(0, features.size(), [&](size_t i) {
            SurfFeatureType surfFeature = ConvertToEigenMatrix_Copy<double, 64, 1>(features[i].des);
            aggregate.features.emplace_back(std::move(surfFeature));

            cv::Point2d point = ConvertToPoint2D(features[i].p.center);
            aggregate.points.emplace_back(std::move(point));
        });
    } 

    long internalMaxPoints;
    double internalDetectionThreshold;
};
