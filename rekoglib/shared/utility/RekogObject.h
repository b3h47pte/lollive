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
    // NO CONSTRUCTOR --> This allows inherited class to disable the default constructor should they implement their own constructor.
    // Virtual Destructor for inheritance
    virtual ~RekogObject() = default;

    // Copy
    RekogObject(const RekogObject&) = default;
    RekogObject& operator=(const RekogObject&) = default;

    // Move
    RekogObject(RekogObject&&) = default;
    RekogObject& operator=(RekogObject&&) = default;
};
