//
// MLInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/internal/InternalInterface.h"

template<typename InternalInterface>
class MLInterface: public InternalInterface
{
public:
    virtual void InitializeInterface() = 0;                     // Create a new interface from scratch.
    virtual void SaveInterface(const std::string&) = 0;         // Save the inferface to a file.
    virtual void InitializeInterface(const std::string&) = 0;   // Load a saved interface from the specified file.
private:
};
