//
// RekogLogging.h
// Author: Michael Bao
// Date: 9/3/2015
//
#pragma once

#ifndef REKOG_DISABLE_LOG

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <string>

#define GENERIC_REKOG_LOG(PREFIX, MSG, TYPE) BOOST_LOG_TRIVIAL(TYPE) << PREFIX << MSG;
#define REKOG_LOG(MSG) GENERIC_REKOG_LOG("INFO: ", MSG, info)
#define REKOG_WARN(MSG) GENERIC_REKOG_LOG("WARNING: ", MSG, warning)
#define REKOG_ERROR(MSG) GENERIC_REKOG_LOG("ERROR: ", MSG, error)

class RekogLogging
{
public:
    static void Initialize(std::string filename = "");
private:
    static bool isInitialized;
};


bool RekogLogging::isInitialized = false;
void RekogLogging::Initialize(std::string filename)
{
    if (isInitialized) {
        return;
    }

    if (!filename.empty())
    {
        boost::log::add_file_log(
            boost::log::keywords::file_name = filename,
            boost::log::keywords::format = "[%TimeStamp%] %Message%"
        );
    }

    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::info
    );

    isInitialized = true;
}

#else
#define REKOG_LOG(MSG)
#define REKOG_WARN(MSG)
#define REKOG_ERROR(MSG)
#endif
