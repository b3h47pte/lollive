//
// KmeansInterface.h
// Author: Michael Bao
// Date: 9/8/2015
//
#pragma once 

#include "shared/algorithms/cluster/RekogClusteringInterface.h"

// K -- # number of clusters
// N x M is the dimension of the samples
template<typename BaseInterface, int K, int N, int M>
class KmeansInterface: public RekogClusteringInterface<BaseInterface, K, N, M>
{
};
