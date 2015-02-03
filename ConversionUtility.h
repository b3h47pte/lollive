#pragma once
#ifndef _CONVERSIONUTILITY_H
#define _CONVERSIONUTILITY_H

#include "common.h"
#include "StringUtility.h"
#include "opencv2/core.hpp"
#include "MultiRectangle.h"

// Assume we store rectangles in the format:
// [x] [y] [width] [height]
inline cv::Rect CreateRectFromString(const std::string& input) {
  std::vector<std::string> components = SplitString(input);
  if(components.size() < 4) {
    std::cout << "ERROR: Invalid string to convert into rect: " << input << std::endl;
    return cv::Rect();
  }
  try {
    cv::Rect rect(std::stod(components[0]), std::stod(components[1]), std::stod(components[2]), std::stod(components[3]));
    return rect;
  }
  catch (...) {
    std::cout << "ERROR: Invalid string to convert into rectangle: " << input << std::endl;
    return cv::Rect();
  }

  return cv::Rect();
}

// Components: [initial rectangle] [x step] [y step] [rectangle count] [row size]
inline MultiRectangle CreateMultiRectFromString(const std::string& input) {
  std::vector<std::string> components = SplitString(input);
  if(components.size() < 8) {
    std::cout << "ERROR: Invalid string to convert into multi rect: " << input << std::endl;
    return MultiRectangle();
  }

  try {
    std::stringstream rectangleStream;
    rectangleStream << components[0] << " " << components[1] << " " << components[2] << " " << components[3];
    return MultiRectangle(CreateRectFromString(rectangleStream.str()), std::stod(components[4]), std::stod(components[5]), 
        std::atoi(components[6].c_str()), std::atoi(components[7].c_str()));
  }
  catch (...) {
    std::cout << "ERROR: Invalid string to convert into multi rectangle: " << input << std::endl;
    return MultiRectangle();
  }

 return MultiRectangle(); 
}

#endif 
