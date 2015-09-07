//
// EigenSTLInterop.h
// Author: Michael Bao
// Date: 9/7/2015
//
#pragma once

#include "Eigen/Core"
#include <array>

template<typename T, int N>
Eigen::Map<Eigen::Matrix<T,N,1>> ConvertToEigenMatrix_NoCopy(const std::array<T, N>& input)
{
    return Eigen::Map<Eigen::Matrix<T,N,1>>(input.data());
}

template<typename T, int N>
Eigen::Matrix<T,N,1> ConvertToEigenMatrix_Copy(const std::array<T, N>& input)
{
    Eigen::Matrix<T,N,1> matrix;
    for (int i = 0; i < N; ++i) {
        matrix[i] = input[i];
    }
    return matrix;
}
