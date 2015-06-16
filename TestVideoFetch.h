#pragma once
#ifndef _TESTVIDEOFETCH_H
#define _TESTVIDEOFETCH_H

#include "VideoFetcher.h"

/*
 * Simulates a video stream by replaying files.
 */
class TestVideoFetch : public VideoFetcher {
public:
  TestVideoFetch(std::string Id, std::string Url, std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> Callback);
  virtual ~TestVideoFetch();
  virtual void BeginFetch();

  void SetStartFrame(int f) { startFrame = f; }
private:
  int startFrame;

  // Retrieve Stream URL
  virtual std::string GetStreamURL();
  virtual bool BeginStreamPlayback(std::string& streamUrl);
};

#endif