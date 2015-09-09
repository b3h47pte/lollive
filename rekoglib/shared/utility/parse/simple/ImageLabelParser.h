//
// ImageLabelParser.h
// Author: Michael Bao
// Date: 9/9/2015
//
#pragma once

#include <string.h>
#include <vector>
#include <fstream>
#include "shared/utility/log/RekogLogging.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "boost/algorithm/string/trim.hpp"

class ImageLabelParser
{
public:
    static void Parse(const std::string& filename, std::vector<cv::Mat>& images, std::vector<std::string>& labels)
    {
        std::ifstream fs(filename, std::ifstream::in);

        if (!fs.is_open()) {
            REKOG_ERROR("Failed to open file: " << filename);
            return;
        }

        try {
            while(!fs.eof()) {
                std::string imageFilename;
                fs >> imageFilename;
                boost::trim(imageFilename);

                if (imageFilename == "") {
                    continue;
                }
                
                // In the future would definitely be useful to load images with an alpha channel.
                cv::Mat image = cv::imread(imageFilename, CV_LOAD_IMAGE_COLOR);
                images.emplace_back(std::move(image));

                std::string imageLabel;
                fs >> imageLabel;
                labels.emplace_back(std::move(imageLabel));
            }
        } catch (...) {
            REKOG_ERROR("Error parsing file: " << filename);
        }
    }
};
