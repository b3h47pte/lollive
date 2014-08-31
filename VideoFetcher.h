#pragma once
#ifndef _VIDEOFETCH_H
#define _VIDEOFETCH_H

#include <iostream>
#include <functional>
#include <stdint.h>
#include <gst/gst.h>
#include "common.h"

/*
 * This class takes the URL of a livestream (a Twitch link or a Youtube link, etc.) and will
 * return a stream of images via a callback provided to it.
 */
class VideoFetcher {
public:
  VideoFetcher(std::string Id, std::string Url, std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> Callback);
  ~VideoFetcher();

  // Starts the process of getting the stream from the URL, getting the video stream and spitting out images.
  void BeginFetch();

private:
  // Input options so we know where to get the video from and where to spit the images out to.
  std::string mImagePath;
  std::string mID;
  std::string mURL;
  std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> mCallback;

  // Frame count
  IMAGE_FRAME_COUNT_TYPE mFrameCount;

  // Retrieve Stream URL
  std::string GetStreamURL();
  // Actually play the stream
  bool BeginStreamPlayback(std::string& streamUrl);
  // Connects the source stream to the pipeline
  static void SourcePadAddedHandler(GstElement* src, GstPad* pad, GstElement** sink);
};

#endif