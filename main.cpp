#include <gst/gst.h>
#include "VideoFetcher.h"

void test(void* t) {

}

int main() {
  VideoFetcher* vid = new VideoFetcher("twitch.tv/tsm_theoddone", test);
  vid->BeginFetch();
  return 0;
}