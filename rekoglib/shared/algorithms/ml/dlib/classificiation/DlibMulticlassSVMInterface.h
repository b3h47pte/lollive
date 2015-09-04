//
// DlibMulticlassSVMInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/algorithms/ml/dlib/DlibClassificationInterface.h"
#include "shared/utility/image/features/RekogImageFeatureExtractor.h"

#include "dlib/svm_threaded.h"
#include "dlib/matrix/matrix.h"
#include "dlib/serialize.h"

template<typename ImagePixelType,
    typename ClassificationFeatureExtractor,
    typename ClassificationResultType>
class DlibMulticlassSVMInterface: public DlibClassificationInterface<ImagePixelType, ClassificationFeatureExtractor, ClassificationResultType>
{
    using Base = DlibClassificationInterface<ImagePixelType, ClassificationFeatureExtractor, ClassificationResultType>;
    using ImageType = typename Base::ImageType;
    using ClassificationFeatureType = typename Base::ClassificationFeatureType;
public:
    virtual void InitializeInterface() override
    {
        multiclassSVM = std::make_unique<DlibSvmType>();
        assert(multiclassSVM);
    }

    virtual void SaveInterface(const std::string& filename) override
    {
        if (!multiclassSVM) {
            REKOG_ERROR("Trying to save an invalid DlibMulticlassSVMInterface");
            return;
        }
        dlib::serialize(filename) << *multiclassSVM;
    }

    virtual void InitializeInterface(const std::string& filename) override
    {
        InitializeInterface();
        dlib::deserialize(filename) >> *multiclassSVM;
    }

    virtual ClassificationResultType Classify(const ImageType&) const override
    {
        return ClassificationResultType();
    }

    virtual void Train(const std::vector<ImageType, ClassificationResultType>&) override
    {
    }

private:
    using DlibSvmType = dlib::svm_multiclass_linear_trainer<dlib::linear_kernel<ClassificationFeatureType>, ClassificationResultType>;
    std::unique_ptr<DlibSvmType> multiclassSVM;
};
