#pragma once
#ifndef _DATA_H
#define _DATA_H
#include "common.h"
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

#endif