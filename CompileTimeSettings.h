#pragma once
#ifndef _COMPILE_TIME_SETTINGS_H
#define _COMPILE_TIME_SETTINGS_H

#ifdef __PRODUCTION__
static const bool USE_REMOTE_CONFIG_PATH = true;
#else
static const bool USE_REMOTE_CONFIG_PATH = false;
#endif

#ifdef __USE_SVM__
static const bool USE_SVM = true;
#else
static const bool USE_SVM = false;
#endif

#endif