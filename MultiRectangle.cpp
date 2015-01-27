#include "MultiRectangle.h"

MultiRectangle::MultiRectangle():
  xStep(0.0), yStep(0.0), totalRectangles(0) {
}

MultiRectangle::MultiRectangle(const cv::Rect& initialRectangle, double xStep, double yStep, int totalRectangles, int rowSize):
  initialRectangle(initialRectangle), xStep(xStep), yStep(yStep), totalRectangles(totalRectangles), rowSize(rowSize) {
}

MultiRectangle::~MultiRectangle() {
}


cv::Rect MultiRectangle::
GetRectangle(int yIndex, int xIndex) const {
  cv::Rect newRect = initialRectangle;
  newRect.x += xStep * xIndex;
  newRect.y += yStep * yIndex;
  return newRect;
}
