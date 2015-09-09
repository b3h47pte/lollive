//
// main.cpp
// Author: Michael Bao
// Date: 9/9/2015
//

#include "shared/algorithms/ml/svm/dlib/DlibMulticlassSVMInterface.h"
#include "shared/utility/parse/simple/ImageLabelParser.h"
#include <iostream>

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


    return 0;
}
