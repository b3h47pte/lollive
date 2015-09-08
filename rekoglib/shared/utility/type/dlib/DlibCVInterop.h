//
// DlibCVInterop.h
// Author: Michael Bao
// Date: 9/8/2015
//
#pragma once

#include "dlib/geometry/vector.h"
#include "opencv2/core.hpp"

cv::Point2d ConvertToPoint2D(const dlib::vector<double, 2>& input)
{
    return cv::Point2d(input.x(), input.y());
}

