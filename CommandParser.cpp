#include "CommandParser.h"

CommandParser::CommandParser(int argc, char** argv): argc(argc), argv(argv) {
  ReparseArguments();
}

void CommandParser::ReparseArguments() {
  for (int i = 1; i < argc; ++i) {
    std::string key = argv[i];
    if (key[0] != '-') {
      continue;
    }
    key = key.erase(0, 1);
    mKeyValuePairs[key] = "";
    if (i + 1 >= argc) {
      continue;
    }

    std::string value = argv[i + 1];
    if (value[0] == '-') {
      continue;
    }
    mKeyValuePairs[key] = value;
  }
}

std::string CommandParser::GetValue(const std::string& key, const std::string& defaultValue) const {
  try {
    return mKeyValuePairs.at(key);
  } catch (...) {
    return defaultValue;
  }
}

bool CommandParser::Exists(const std::string& key) const {
  return (mKeyValuePairs.find(key) != mKeyValuePairs.end());
}