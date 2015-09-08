//
// RekogClusteringInterface.h
// Author: Michael Bao
// Date: 9/8/2015
//

#pragma once

#include "shared/internal/InternalInterface.h"
#include "Eigen/Core"

template<typename InternalInterface, int K, int N, int M>
class RekogClusteringInterface: public InternalInterface
{
public:
    using SampleType = Eigen::Matrix<double, N, M>;
    virtual void Cluster(const tbb::concurrent_vector<SampleType>&, std::array<SampleType, K>&) = 0;
};
