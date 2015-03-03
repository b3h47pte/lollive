#pragma once
#ifndef _FILEUTILITY_H
#define _FILEUTILITY_H

#include <iostream>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif


static std::string GetRelativeFilePath(const std::string& input, bool relativeToSource = true) {
  const size_t maxPathSize = 255;
  char buffer[maxPathSize];
  char* cwd = NULL;
#if defined(_WIN32)
  GetCurrentDirectoryA(maxPathSize, buffer);
  cwd = buffer;
#else
  cwd = getcwd(buffer, maxPathSize);
#endif
  if(!cwd) return "";
  std::string dir(cwd);
  if (relativeToSource) {
    char* llldb_dir = getenv("LLLDB_DIR");
    dir = llldb_dir ? std::string(llldb_dir) : "";
  }
  dir += ("/" + input);
  return dir;
}

#endif
