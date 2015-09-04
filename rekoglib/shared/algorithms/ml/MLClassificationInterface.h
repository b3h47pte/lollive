//
// MLClasificationInterface.h
// Author: Michael Bao
// Date: 9/4/2015
//
#pragma once

#include "shared/algorithms/ml/MLInterface.h"
#include "shared/utility/type/RekogForceDerivedTemplate.h"

template<typename BaseInterface>
class MLClassificationInterface: public RekogForceDerivedTemplate<BaseInterface, MLInterface<typename BaseInterface::ImageType>>
{
public:
};
