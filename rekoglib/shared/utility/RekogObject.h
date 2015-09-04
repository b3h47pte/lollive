//
// RekogObject.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/utility/RekogCommon.h"

class RekogObject: public std::enable_shared_from_this<RekogObject>
{
public:
    // Constructor/Destructor
    RekogObject() = default;
    virtual ~RekogObject() = default;

    // Copy
    RekogObject(const RekogObject&) = default;
    RekogObject& operator=(const RekogObject&) = default;

    // Move
    RekogObject(RekogObject&&) = default;
    RekogObject& operator=(RekogObject&&) = default;
};
