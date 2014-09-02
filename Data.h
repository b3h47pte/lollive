#pragma once
#ifndef _DATA_H
#define _DATA_H
#include "common.h"
#include <functional>
#include <algorithm>
// Classes to allow us to store whatever data we want
class Data {
public:
  Data() {}
  virtual ~Data() {}
};

template<typename T>
class GenericData : public Data {
public:
  GenericData(const T& inVal) : value(inVal) {}
  virtual ~GenericData() {}
  T value;
};

typedef std::map<std::string, std::shared_ptr<Data> > GenericDataStore;

inline bool DataExists(std::shared_ptr<GenericDataStore> inStore, std::string& key) {
  return (inStore->find(key) != inStore->end());
}

template<typename T>
inline T RetrieveData(std::shared_ptr<GenericDataStore> inStore, std::string& key) {
  return std::static_pointer_cast<GenericData<T>>((*inStore)[key])->value;
}


template<typename T>
inline bool IsValid(T t) {
  return true;
}

template<>
inline bool IsValid<int>(int t) {
  return (t != -1);
}

template<>
inline bool IsValid<std::string>(std::string t) {
  return (t != "");
}

// Smooth out values based on a GenericDataStore history
// Does a 'most likely' analysis on the data. If the value has 
// been constantly changing in recent analysis, then we just assume
// that whatever came in is right. However, if the value has been
// steady for a few frames, then spikes, then goes back down, then we assume
// the spike is false. This function only works on numbers
// 
/// CALL THIS FUNCTION ONLY IF SOMETHING IS BELIEVED TO BE WRONG.
template<typename T>
inline T SmoothValue(T& newValue, T& curValue, std::function<T(std::shared_ptr<GenericDataStore>)> fnc, std::vector<std::shared_ptr<GenericDataStore>>& inHistory) {
  // Need to have a minimum number of frames for analysis to work.
  // TODO: Configurable
  if (!IsValid(newValue)) {
    return curValue;
  }

  if (inHistory.size() < 5 || !IsValid(curValue)) {
    return newValue;
  }

  // Store values we have from most recent to least recent
  std::vector<T> recentValues;
  std::map<T, int> occurenceMap;
  int count = 0;

  for (auto it = inHistory.rbegin(); it < inHistory.rend(); ++it) {
    // Make sure we have something to compare to...would be weird if we didn't though.
    T inVal = fnc(*it);
    if (!IsValid(inVal)) continue;
    recentValues.push_back(inVal);
    ++count;
    ++occurenceMap[inVal];
    if (count == 5) {
      break;
    }
  }
  
  if (recentValues.size() == 0) {
    return newValue;
  }

  ++occurenceMap[newValue];
  T mostFreq = std::max_element(occurenceMap.begin(), occurenceMap.end(), [](std::pair<T, int> t1, std::pair<T, int> t2) {
    return t1.second < t2.second;
  })->first;
  return mostFreq;
}


#endif