#pragma once
#ifndef _COMPILE_TIME_SETTINGS_H
#define _COMPILE_TIME_SETTINGS_H

#ifdef __PRODUCTION__
static const bool USE_REMOTE_CONFIG_PATH = true;
static const bool ALLOW_DEBUG_VIDEO_FETCH = false;
#else
static const bool USE_REMOTE_CONFIG_PATH = false;
static const bool ALLOW_DEBUG_VIDEO_FETCH = true;
#endif

#ifdef __USE_SVM__
static const bool USE_SVM = true;
#else
static const bool USE_SVM = false;
#endif

#endif