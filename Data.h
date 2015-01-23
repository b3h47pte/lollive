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

template<typename K>
using TGenericDataStore = std::unordered_map<K, std::shared_ptr<Data>>;
using GenericDataStore = TGenericDataStore<std::string>;


template<typename K = std::string>
inline bool DataExists(std::shared_ptr<TGenericDataStore<K>> inStore, const K& key) {
  return (inStore->find(key) != inStore->end());
}

template<typename T,typename K = std::string>
inline T RetrieveData(std::shared_ptr<TGenericDataStore<K>> inStore, const K& key) {
  return std::static_pointer_cast<GenericData<T>>((*inStore)[key])->value;
}

template<typename T>
inline bool IsValid(T t) {
  (void)t;
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

// Gets recent values from the data history
// fnc is a function that returns the relevant value from a GenericDataStore.
template<typename T>
inline std::vector<T> GetRecentValues(std::function<T(std::shared_ptr<GenericDataStore>)> fnc, const std::vector<std::shared_ptr<GenericDataStore>>& inHistory, int numToRetrieve) {
  // Store values we have from most recent to least recent
  std::vector<T> recentValues;

  for (auto it = inHistory.rbegin(); it < inHistory.rend(); ++it) {
    // Make sure we have something to compare to...would be weird if we didn't though.
    T inVal = fnc(*it);
    if (!IsValid(inVal)) continue;
    recentValues.push_back(inVal);
    if (recentValues.size() == numToRetrieve) {
      break;
    }
  }
  return recentValues;
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
inline T SmoothValueFreq(T& newValue, T& curValue, std::function<T(std::shared_ptr<GenericDataStore>)> fnc, std::vector<std::shared_ptr<GenericDataStore>>& inHistory) {
  // Need to have a minimum number of frames for analysis to work.
  // TODO: Configurable
  if (!IsValid(newValue)) {
    return curValue;
  }

  if (inHistory.size() < 5 || !IsValid(curValue)) {
    return newValue;
  }

  // Store values we have from most recent to least recent
  std::vector<T> recentValues = GetRecentValues(fnc, inHistory, 5);
  
  if (recentValues.size() == 0) {
    return newValue;
  }

  std::map<T, int> occurenceMap;
  std::for_each(recentValues.begin(), recentValues.end(), [&](T v) {
    ++occurenceMap[v];
  });

  ++occurenceMap[newValue];
  T mostFreq = std::max_element(occurenceMap.begin(), occurenceMap.end(), [](std::pair<T, int> t1, std::pair<T, int> t2) {
    return t1.second < t2.second;
  })->first;
  return mostFreq;
}

// Smooth out values based on a the amount of variance in the data. 
// If the data has low variance and then suddenly we receive a value that's a lot higher/lower
// than usual, then we can safely reject that value. Otherwise, we just keep the old value.
// CALL THIS FUNCTION ONLY IF SOMETHING IS BELIEVED TO BE WRONG.
template<typename T>
inline T SmoothValueVariance(T& newValue, T& curValue, std::function<T(std::shared_ptr<GenericDataStore>)> fnc, std::vector<std::shared_ptr<GenericDataStore>>& inHistory) {
  if (!IsValid(newValue)) {
    return curValue;
  }

  if (inHistory.size() < 5 || !IsValid(curValue)) {
    return newValue;
  }

  // Store values we have from most recent to least recent
  std::vector<T> recentValues = GetRecentValues(fnc, inHistory, 5);

  if (recentValues.size() == 0) {
    return newValue;
  }

  // Calculate mean and variance
  double mean = 0.0;
  for (T value : recentValues) {
    mean += value;
  }
  mean /= recentValues.size();

  double variance = 0.0;
  for (T value : recentValues) {
    double var = value - mean;
    variance += var * var;
  }
  variance /= recentValues.size();

  // Check the new value's variance.
  double newValDiff = newValue - mean;
  double newValueVar = newValDiff * newValDiff;
  if (newValueVar > variance) {
    return curValue;
  }

  return newValue;

}

#endif
