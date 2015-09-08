//
// DlibKmeansInterface.h
// Author: Michael Bao
// Date: 9/8/2015
//

#pragma once

#include "shared/internal/dlib/DlibInterface.h"
#include "shared/algorithms/cluster/kmeans/KmeansInterface.h"
#include "shared/utility/type/dlib/DlibEigenInterop.h"
#include "dlib/clustering.h"

template<typename ImagePixelType, template<class> class KernelType, int K, int N, int M>
class DlibKmeansInterface: public KmeansInterface<DlibInterface<ImagePixelType>, K, N, M>
{
    using Base = KmeansInterface<DlibInterface<ImagePixelType>, K, N, M>;
    using SampleType = typename Base::SampleType;
    using DlibSampleType = dlib::matrix<double, N, M>;
    using FinalKernelType = KernelType<DlibSampleType>;
public:
    DlibKmeansInterface():
        accuracyParameter(0.01), maximumDictionaryParameters(8)
    {
    }

    virtual void Cluster(const tbb::concurrent_vector<SampleType>& samples, std::array<SampleType, K>& clusters)
    {
        // Convert Eigen Matrix samples to Dlib Samples
        std::vector<DlibSampleType> dlibSamples;
        dlibSamples.reserve(samples.size());
        std::transform(samples.begin(), samples.end(), std::inserter(dlibSamples, dlibSamples.begin()),
            [](auto& s) {
                return ConvertToDlibMatrix(s);
            });
        
        // Use Dlib K-Means to perform clustering
        // Code is based off of: http://dlib.net/kkmeans_ex.cpp.html
        dlib::kcentroid<FinalKernelType> kc(*kernel, accuracyParameter, maximumDictionaryParameters);

        dlib::kkmeans<FinalKernelType> kmeans(kc);
        kmeans.set_number_of_centers(K);

        std::vector<DlibSampleType> dlibCenters;
        dlibCenters.reserve(K);
        dlib::pick_initial_centers(K, dlibCenters, dlibSamples, kmeans.get_kernel());
        kmeans.train(dlibSamples, dlibCenters);

        // Accumulate the samples in the output 'clusters' array to get the cluster means that we want to use.
        int clusterCounter[K] = {};
        for (auto i = 0; i < dlibSamples.size(); ++i) {
            auto clusterIndex = kmeans(dlibSamples[i]);
            clusters[clusterIndex] += ConvertToEigenMatrix_Copy(dlibSamples[i]);
            ++clusterCounter[clusterIndex];
        }

        for (int i = 0; i < K; ++i) {
            clusters[i] /= (double)clusterCounter[i];
        }
    }

    // Need to be able to change and modify kernel parameters.
    void SetKernel(std::unique_ptr<FinalKernelType> inputKernel) {
        kernel = std::move(inputKernel);
    }
private:
    std::unique_ptr<FinalKernelType> kernel;

    // Centroid Parameters
    double accuracyParameter;
    int maximumDictionaryParameters;
};
