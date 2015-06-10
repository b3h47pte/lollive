#pragma once
#ifndef __AUTHENTICATION__
#define __AUTHENTICATION__

#include <string>

class Authentication {
public:
  static Authentication* Get() { return singleton; }
  std::string CalculateHMAC(const std::string& message, const std::string& key) const;
private:
  Authentication();
  ~Authentication();

  static Authentication* singleton;
};

#endif
