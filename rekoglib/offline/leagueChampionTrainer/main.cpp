//
// main.cpp
// Author: Michael Bao
// Date: 9/9/2015
//

#include "shared/algorithms/cluster/kmeans/dlib/DlibKmeansInterface.h"
#include "shared/algorithms/features/surf/dlib/DlibSURFInterface.h"
#include "shared/algorithms/ml/svm/dlib/DlibMulticlassSVMInterface.h"
#include "shared/utility/parse/simple/ImageLabelParser.h"
#include "shared/utility/type/STLVectorUtility.h"

#include <iostream>

constexpr int totalDictionaryWords = 100;
using PixelType = unsigned char;

template<typename T> 
using KernelType = dlib::radial_basis_kernel<T>;
using Clusterer = DlibKmeansInterface<PixelType, KernelType, totalDictionaryWords, 64, 1>;
using FeatureExtractor = DlibSURFInterface<PixelType, Clusterer, totalDictionaryWords>;
using Classifier = DlibMulticlassSVMInterface<PixelType, FeatureExtractor, std::string>;

int main(int argc, char** argv)
{
    if (argc < 2) {
        REKOG_ERROR("Pass in training file.");
        return 1;
    }

    std::string trainingFile(argv[1]);

    // Training File contains pairs of images to labels.
    std::vector<cv::Mat> images;
    std::vector<std::string> labels;
    ImageLabelParser::Parse(trainingFile, images, labels);

    std::vector<std::pair<cv::Mat, std::string>> classificationData;
    STLVectorUtility::ZipVectors(images, labels, classificationData);

    std::unique_ptr<Classifier> classifier = std::make_unique<Classifier>();
    classifier->InitializeInterface();

    return 0;
}
