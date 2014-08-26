#include <gst/gst.h>
#include "VideoFetcher.h"

void test(IMAGE_PATH_TYPE t, IMAGE_TIMESTAMP_TYPE ts) {
}

int main() {
  VideoFetcher* vid = new VideoFetcher("twitch", "twitch.tv/scarra", test);
  vid->BeginFetch();
  return 0;
}