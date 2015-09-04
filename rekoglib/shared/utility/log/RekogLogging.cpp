//
// RekogLogging.h
// Author: Michael Bao
// Date: 9/3/2015
//
#include "RekogLogging.h"

#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>

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
