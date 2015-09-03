//
// RecognitionInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/RecognitionTrainingParameters.h"
#include "shared/utility/RekogObject.h"
#include <opencv2/core.hpp>
#include <vector>
#include <utility>

//
// Base Class for interacting with the underlying recognition data structure/algorithm (machine learning, neural network, etc.).
// Steps for RECOGNITION using a RecognitionInterface:
//  1) Construct the appropriate interface. Note that the interface can and should be reused.
//  2) Query the interface with an image --> This returns some result of type T (depending on the interface subclass).
//      Voila! The interface takes care of running everything behind the scene. Note that the interface assumes 
//      that the user wants to block until the results return. Should the need for an async interface come up, it should be
//      trivial to wrap around.
//
// Steps for TRAINING using a RecognitionInterface:
//  1) Construct the appropriate interface. Note that the interface can and should be reused.
//  2) Pass in a std::vector that contains std::pair's. Each std::pair contains the image (cv::Mat) and its label (T).
// Additionally, you may pass a 'RecognitionTrainingParameters' object to fine-tune the parameters for training.
//
template<typename T>
class RecognitionInterface: public RekogObject
{
public:
    virtual T RequestDetection(cv::Mat& inputImage) = 0;
    virtual void RequestTraining(const std::vector<std::pair<const cv::Mat&, T>>& inputTraining, std::unique_ptr<const RecognitionTrainingParameters> params = nullptr) = 0;
private:
};
