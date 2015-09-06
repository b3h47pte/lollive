//
// DlibSTLInterop.h
// Author: Michael Bao
// Date: 9/6/2015
//
#pragma once

template<typename T, int N>
dlib::matrix<T,N,1> ConvertToDlibMatrix(const std::array<T, N>& input)
{
    return dlib::matrix<T,N,1>(input.data());
}

