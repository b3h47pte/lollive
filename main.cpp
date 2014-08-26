#include <gst/gst.h>
#include "VideoFetcher.h"

void test(void* t) {

}

int main() {
  VideoFetcher* vid = new VideoFetcher("twitch", "twitch.tv/scarra", test);
  vid->BeginFetch();
  return 0;
}