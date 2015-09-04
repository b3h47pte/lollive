//
// RekogLogging.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#ifndef REKOG_DISABLE_LOG

#include <boost/log/trivial.hpp>
#define GENERIC_REKOG_LOG(PREFIX, MSG, TYPE) BOOST_LOG_TRIVIAL(TYPE) << PREFIX << MSG;
#define REKOG_LOG(MSG) GENERIC_REKOG_LOG("INFO: ", MSG, info)
#define REKOG_WARN(MSG) GENERIC_REKOG_LOG("WARNING: ", MSG, warning)
#define REKOG_ERROR(MSG) GENERIC_REKOG_LOG("ERROR: ", MSG, error)
#include <string>

class RekogLogging
{
public:
    static void Initialize(std::string filename = "");
private:
    static bool isInitialized;
};

#else
#define REKOG_LOG(MSG)
#define REKOG_WARN(MSG)
#define REKOG_ERROR(MSG)
#endif
