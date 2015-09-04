//
// MLInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#include "shared/utility/RekogObject.h"
#include "opencv2/core.hpp"


template<typename InternalImageType>
class MLInterface: RekogObject
{
public:
    virtual InternalImageType ConvertImageToInternalRepresentation(const cv::Mat inputImage) = 0;

    virtual void InitializeInterface() = 0;                     // Create a new interface from scratch.
    virtual void SaveInterface(const std::string&) = 0;         // Save the inferface to a file.
    virtual void InitializeInterface(const std::string&) = 0;   // Load a saved interface from the specified file.
private:
};
