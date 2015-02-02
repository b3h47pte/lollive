#pragma once
#ifndef _FILEUTILITY_H
#define _FILEUTILITY_H

#include <iostream>
#if defined(_WIN32)

#else
#include <unistd.h>
#endif


static std::string GetRelativeFilePath(const std::string& input) {
  const size_t maxPathSize = 255;
  char buffer[maxPathSize];
  char* cwd = NULL;
#if defined(_WIN32)
#else
  cwd = getcwd(buffer, maxPathSize);
#endif
  if(!cwd) return "";
  std::string dir(cwd);
  dir += ("/" + input);
  return dir;
}

#endif
