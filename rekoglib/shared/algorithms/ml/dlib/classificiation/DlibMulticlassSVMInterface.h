//
// DlibMulticlassSVMInterface.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#include "shared/algorithms/ml/dlib/DlibClassificationInterface.h"
#include "shared/utility/image/features/RekogImageFeatureExtractor.h"
#include "shared/utility/type/STLVectorUtility.h"

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

    using DlibKernelType = dlib::linear_kernel<ClassificationFeatureType>;
    using DlibSvmType = dlib::svm_multiclass_linear_trainer<dlib::linear_kernel<ClassificationFeatureType>, ClassificationResultType>;
    using DlibDecisionType = typename DlibSvmType::trained_function_type;
public:
    virtual void InitializeInterface() override
    {
        multiclassSVM = std::make_unique<DlibSvmType>();
        assert(multiclassSVM);
    }

    virtual void SaveInterface(const std::string& filename) override
    {
        if (!decisionFunction) {
            REKOG_ERROR("Trying to save an invalid DlibMulticlassSVMInterface.");
            return;
        }
        dlib::serialize(filename) << *decisionFunction;
    }

    virtual void InitializeInterface(const std::string& filename) override
    {
        InitializeInterface();
        dlib::deserialize(filename) >> *decisionFunction;
    }

    virtual ClassificationResultType Classify(const ImageType& image, double* weight) const override
    {
        if (!decisionFunction) {
            REKOG_ERROR("Can not classify without either loading or training the DlibMulticlassSVMInterface.");
            return ClassificationResultType();
        }
        auto features = ClassificationFeatureExtractor(image)();
        auto prediction = decisionFunction->predict(features);

        if (weight) {
            // prediction->second is a scalar type so we can definitely cast it to double.
            *weight = static_cast<double>(prediction->second);
        }

        return prediction->first;
    }

    virtual void Train(const std::vector<std::pair<ImageType, ClassificationResultType>>& trainingData) override
    {
        if (!multiclassSVM) {
            REKOG_ERROR("Failed to train SVM since DlibMulticlassSVMInterface was not initalized properly.");
            return;
        }
        // Separate into images and labels because Dlib requires two separate vectors.
        std::vector<ImageType> imageSamples;
        std::vector<ClassificationResultType> imageLabels;
        STLVectorUtility::SplitVectorOfPairs(trainingData, imageSamples, imageLabels);

        // Use the user specified extractor to get image features for each image.
        std::vector<ClassificationFeatureType> imageFeatureVectors;
        std::transform(imageSamples.begin(), imageSamples.end(), std::inserter(imageFeatureVectors, imageFeatureVectors.begin()), 
                [](const ImageType& image) { return ClassificationFeatureExtractor(image)(); });

        // Train the SVM and remember its decision function.
        assert(imageFeatureVectors.size() == imageLabels.size());
        DlibDecisionType trained = multiclassSVM->train(imageFeatureVectors, imageLabels);
        decisionFunction = std::make_unique<DlibDecisionType>(std::move(trained));

        // ?? Set prior so that each call to Train builds off of the previous call.
        multiclassSVM->set_prior(*decisionFunction);
    }

private:
    std::unique_ptr<DlibSvmType> multiclassSVM;
    std::unique_ptr<DlibDecisionType> decisionFunction;
};
