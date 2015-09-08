//
// DlibEigenInterop.h
// Author: Michael Bao
// Date: 9/7/2015
//
#pragma once

#include "dlib/matrix.h"
#include "Eigen/Core"

template<typename T, int N, int M = 1>
dlib::matrix<T, N, M> ConvertToDlibMatrix(const Eigen::Matrix<T, N, M>& input)
{
    Eigen::Matrix<T, N, M, Eigen::RowMajor> rmInput = input;
    T (&arr)[N*M] = *rmInput.data();
    return dlib::matrix<T,N,M>(arr);
}

template<typename T, int N, int M = 1>
Eigen::Matrix<T, N, M> ConvertToEigenMatrix_Copy(const dlib::matrix<T, N, M>& input)
{
    Eigen::Matrix<T,N,M> matrix;
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < M; ++c) {
            matrix(r, c) = input(r, c);
        }
    }
    return matrix;

}
