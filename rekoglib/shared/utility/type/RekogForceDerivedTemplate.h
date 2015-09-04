//
// RekogForceDerivedTemplate.h
// Author: Michael Bao
// Date: 9/4/2015
//
#pragma once

#include <type_traits>

// This class will force InputClass to be derived from BaseClass.
// If this passes (i.e. compiles), we will then the class will derive itself from 
// InputClass.

// Use Case: MLClassificationInterface.h knows that it is an MLInterface. But what kind of MLInterface exactly? 
//  Maybe it's a DlibInterface or maybe (in the future) I'll want a MLPack interface. Both have derived classes
//  that will want to inherit the classification interface. As a result, their classification interface (i.e.
//  DlibClassificationInterface) will inherit from MLClassificationInterface but will pass in the appropriate
//  base MLInterface (i.e. DlibInterface). MLClassificationInterface will inherit from RekogForceDerivedTemplate<T, MLInterface<T::ImageType>> 
//  to have a compile time guarantee that T is a derived class of MLInterface. I'd imagine there'd be other errors
//  should that inheritance not hold, but this is just for safety!

template<typename InputClass, typename BaseClass,
    typename = std::enable_if_t<std::is_base_of<BaseClass, InputClass>::value>>
class RekogForceDerivedTemplate: public InputClass
{};


