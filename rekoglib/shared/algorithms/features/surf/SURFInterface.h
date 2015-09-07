//
// SURFInterface.h
// Author: Michael Bao
// Date: 9/6/2015
//
#pragma once

#include "shared/algorithms/features/RekogImageFeatureExtractor.h"
#include <limits>

template<typename ClusteringInterface,
    typename InternalInterface, 
    int N>
class SURFInterface: public RekogImageFeatureExtractor<InternalInterface, N>
{
    using Base = RekogImageFeatureExtractor<InternalInterface, N>;
    using FeatureType = typename Base::FeatureType;
    using SurfFeatureType = Eigen::Matrix<double, 64, 1>;

protected:
    struct SURFAggregate {
        tbb::concurrent_vector<cv::Point2d> points;
        tbb::concurrent_vector<SurfFeatureType> features;
    };

public:
    virtual void LoadFromFile(const std::string& path)
    {
        (void)path;
        // Load dictionary -- make sure we have N elements.

        isSetup = true;
    }

    virtual void SaveToFile(const std::string& path)
    {
        (void)path;
        // Save dictionary
    }

    virtual void SetupFeatureExtractor(const std::vector<cv::Mat>& allImages)
    {
        SURFAggregate completeSURF;
        // Compute SURF features for this image using whatever library we want.
        // Note that for now we don't use the location of the SURF features, but in the future, it will be useful to split the image up and just work on smaller sub-regions.
        tbb::parallel_do(allImages.begin(), allImages.end(), [&](const cv::Mat& image) {
            ComputeSURFFeatures(image, completeSURF);
        });

        // Perform clustering on the features to get our 'dictionary'. After we load the dictionary, we 
        // are then able to extract features from images.
        ClusteringInterface::Cluster(completeSURF.features, dictionary);

        isSetup = true;
    }

    virtual FeatureType ExtractFeature(const cv::Mat& inputImage) const
    {
        if (!isSetup) {
            return;
        }

        SURFAggregate completeSURF;
        ComputeSURFFeatures(inputImage, completeSURF);

        // Create normalized histogram and return it.
        // Go through every SURF feature detected, find the closest item in the dictionary
        std::array<std::atomic<double>, N> histogram{};
        tbb::parallel_do(completeSURF.features.begin(), completeSURF.features.end(), [&](const SurfFeatureType& feature) {
            int match = FindClosestMatchInDictionary(feature);
            ++histogram[match];
        });

        std::array<double, N> nonatomicHistogram = histogram;
        FeatureType mappedHistogram = ConvertToEigenMatrix_NoCopy(nonatomicHistogram);
        mappedHistogram.normalize();
        return mappedHistogram;
    }

private:
    virtual void ComputeSURFFeatures(const cv::Mat&, SURFAggregate&) = 0;

    int FindClosestMatchInDictionary(const SurfFeatureType& inputFeature)
    {
        // At this point, we can assume that the dictionary has already been setup.
        assert(isSetup);

        tbb::queuing_mutex closestMutex;
        double minDistance = std::numeric_limits<double>::max();
        int closestMatch = -1;

        tbb::parallel_for(0, N, [&](int i) {
            // Compute distance from the dictionary feature, if it is closer than the closest feature,
            // store the new minimum distance and corresponding feature.
            double dist = (dictionary[i] - inputFeature).norm();
            {
                tbb::queuing_mutex::scoped_lock slock(closestMutex);
                if (dist < minDistance) {
                    minDistance = dist;
                    closestMatch = i;
                }
            }
        });

        assert(closestMatch >= 0);
        return closestMatch;
    }

    std::array<SurfFeatureType, N> dictionary;
    bool isSetup;
};
