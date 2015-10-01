//
// STLVectorUtility.h
// Author: Michael Bao
// Date: 9/4/2015
//
#pragma once

#include <vector>
#include <utility>

namespace STLVectorUtility
{

template<typename T1, typename T2>
void SplitVectorOfPairs(const std::vector<std::pair<T1, T2>>& input, std::vector<T1>& output1, std::vector<T2>& output2)
{
    output1.clear();
    output1.reserve(input.size());

    output2.clear();
    output2.reserve(input.size());

    for (auto i = decltype(input.size()){0}; i < input.size(); ++i) {
        output1.emplace_back(input.at(i).first);
        output2.emplace_back(input.at(i).second);
    }
}

template<typename T1, typename T2>
void ZipVectors(const std::vector<T1>& input1, const std::vector<T2>& input2, std::vector<std::pair<T1, T2>>& output)
{
    output.clear();

    const auto totalSize = std::min(input1.size(), input2.size());
    output.reserve(totalSize);
    
    for (auto i = decltype(totalSize){0}; i < totalSize; ++i) {
        output.emplace_back(std::make_pair(input1.at(i), input2.at(2)));
    }
}

}

