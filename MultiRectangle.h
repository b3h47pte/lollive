#pragma once
#ifndef _MULTIRECTANGLE_H
#define _MULTIRECTANGLE_H

#include "common.h"
#include "opencv2/core.hpp"

/*
 * A class that is able to specify multiple rectangles.
 * When this class gets read in from a string, it assumes all the
 * separate rectangles are the same size and are all some fixed
 * step away from eachother.
 */
class MultiRectangle {
public:
  MultiRectangle();
  MultiRectangle(const cv::Rect& initialRectangle, double xStep, double yStep, int totalRectangles, int rowSize = 1);
  ~MultiRectangle();

  cv::Rect GetRectangle(int yIndex, int xIndex = 0) const;

private:
  cv::Rect initialRectangle;
  double xStep;
  double yStep;
  int totalRectangles;
  int rowSize;
};

#endif
