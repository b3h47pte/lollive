//
// DlibSTLInterop.h
// Author: Michael Bao
// Date: 9/6/2015
//
#pragma once

#include "dlib/matrix.h"
#include <array>

template<typename T, int N>
dlib::matrix<T,N,1> ConvertToDlibMatrix(const std::array<T, N>& input)
{
    T (&arr)[N] = *input.data();
    return dlib::matrix<T,N,1>(arr);
}

