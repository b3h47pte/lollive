#pragma once
#ifndef _COMMANDPARSER_H
#define _COMMANDPARSER_H
#include "common.h"

/*
 * Class for convenient access to the arguments passed into the program.
 */
class CommandParser {
public:
  CommandParser(int argc, char** argv);

  std::string GetValue(const std::string& key, const std::string& defaultValue = "") const;
  int GetIntValue(const std::string& key, const int defaultValue = 0) const;
  bool Exists(const std::string& key) const;

protected:
  void ReparseArguments();

private:
  int argc;
  char** argv;

  std::unordered_map<std::string, std::string> mKeyValuePairs;

};

#endif