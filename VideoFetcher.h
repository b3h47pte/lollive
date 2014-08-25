#pragma once
#ifndef _VIDEOFETCH_H
#define _VIDEOFETCH_H

#include <iostream>
#include <functional>

/*
 * This class takes the URL of a livestream (a Twitch link or a Youtube link, etc.) and will
 * return a stream of images via a callback provided to it.
 */
class VideoFetcher {
public:
  VideoFetcher(std::string Url, std::function<void(void*)> Callback);
  ~VideoFetcher();

private:
};

#endif