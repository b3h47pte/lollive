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

    for (auto i = 0; i < input.size(); ++i) {
        output1[i].emplace_back(input[i]->first);
        output2[i].emplace_back(input[i]->second);
    }
}

}

