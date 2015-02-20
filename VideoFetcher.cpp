#include "VideoFetcher.h"
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#include <string>
#define popen _popen
#define pclose _pclose
#else
#include <sys/stat.h>
#include <string.h>
#endif


// TODO: Need to print out some sort of error
#define CHECK_FALSE_SETFLAG_JUMP(x,flag,label) \
if (!x) {\
  flag = false; \
  goto label;\
}

VideoFetcher::VideoFetcher(std::string inID, std::string Url, std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> Callback) : mID(inID), mURL(Url), mCallback(Callback), mFrameCount(0) {

}

VideoFetcher::~VideoFetcher() {

}

/*
 * The function the client calls to initiate the process of getting pictures of the stream.
 * Goes through three steps:
 *  1) Gets the right URL to the stream. 
 *  2) Start streaming the video using GStreamer
 *  3) Capture images and send them back.
 */
void VideoFetcher::BeginFetch() {
  // Need to create a directory for the images.
  // If create directory fails with an already exist error, then we want to slap an id onto it.
  // Increment the id every time it fails.
  mImagePath = "Images/" + mID;
  int suffixId = 0;
#ifdef _WIN32
  std::wstring stemp = std::wstring(mImagePath.begin(), mImagePath.end());
  CreateDirectoryW(L"Images", NULL);
  while (!CreateDirectoryW(stemp.c_str(), NULL)) {
#else
  std::string stemp = std::string(mImagePath.begin(), mImagePath.end());
  mkdir("Images", 0755);
  while (mkdir(stemp.c_str(), 0755) ) {
#endif
    ++suffixId;
    mImagePath = "Images/" + mID + std::to_string(suffixId);
#ifdef _WIN32
    stemp = std::wstring(mImagePath.begin(), mImagePath.end());
#else
    stemp = std::string(mImagePath.begin(), mImagePath.end());
#endif
  }

  std::string streamUrl = GetStreamURL();
  if (!BeginStreamPlayback(streamUrl)) {
    return;
  }
}

/*
 * Execute the livestreamer executable to get the stream URL.Assumes livestreamer is in the computer's path
 */
std::string VideoFetcher::GetStreamURL() {
  // TODO: Support more options than Twitch.tv (since I only know this works for twitch)
  std::string lsCommand = "livestreamer " + mURL + " best --stream-url";
  FILE* ls = popen(lsCommand.c_str(), "r");
  assert(ls != NULL);

  char buffer[300];
  std::string streamUrl = "";
  while (!feof(ls)) {
    if (fgets(buffer, 300, ls) != NULL) {
      streamUrl += buffer;
    }
  }
  pclose(ls);

  streamUrl.erase(std::remove(streamUrl.begin(), streamUrl.end(), '\r'));
  streamUrl.erase(std::remove(streamUrl.begin(), streamUrl.end(), '\n'));
  return mURL;
//  return streamUrl;
}

/*
 * Start playing the stream using GStreamer. 
 */
bool VideoFetcher::BeginStreamPlayback(std::string& streamUrl) {
  GstElement* pipeline = NULL;
  GstElement* source = NULL;
  GstElement* convert = NULL;
  GstElement* filter = NULL;
  GstElement* toPng = NULL;
  GstElement* sink = NULL;
  GstCaps* filterCaps = NULL;
  GstBus* bus = NULL;
  bool retFlag = true;
  std::string outputLocation = "";
  GstMessage* msg = NULL;
  bool bComplete = false;
  GError *err = NULL;
  gchar *debug_info;
  
  // Initialize gst
  gst_init(NULL, NULL);

  // Create the pipeline element
  std::string pipelineName = "lollive-pipeline";
  pipeline = gst_pipeline_new(pipelineName.c_str());
  CHECK_FALSE_SETFLAG_JUMP(pipeline, retFlag, cleanup);

  // Create the individual pipeline stage elements
  // For the source, we use the passed in streamUrl.
  // For the sink, we want to create images and pass them back to whoever requested this stream.
  source = gst_element_factory_make("uridecodebin", "source");
  CHECK_FALSE_SETFLAG_JUMP(source, retFlag, cleanup);

  convert = gst_element_factory_make("videoconvert", "convert");
  CHECK_FALSE_SETFLAG_JUMP(convert, retFlag, cleanup);

  // Need to filter it so that we only get the screenshots we want
  // TODO: Provide ourselves with a parameter to specify how often we want screenshots. For now, default to 1 screenshot/second.
  filter = gst_element_factory_make("videorate", "filter");
  CHECK_FALSE_SETFLAG_JUMP(filter, retFlag, cleanup);

  // Set the capabilities of the filter so that we get the framerate we desire.
  filterCaps = gst_caps_new_simple("video/x-raw", "framerate", GST_TYPE_FRACTION, 1, 3, NULL);

  // Convert video frames into PNG images. 
  toPng = gst_element_factory_make("pngenc", "toPng");
  CHECK_FALSE_SETFLAG_JUMP(toPng, retFlag, cleanup);
  // Use a maximum quality PNG so that it's clearer for analysis
  // Though from a basic eye test, I can't see the difference betwee 1 and 9.
  g_object_set(toPng, "compression-level", 1, NULL);

  // Print out images to files as I don't want to keep all the images in memory since it can get expensive.
  sink = gst_element_factory_make("multifilesink", "sink"); 
  CHECK_FALSE_SETFLAG_JUMP(sink, retFlag, cleanup);
  // Make sure the PNG is generated in the right place
  outputLocation = mImagePath + "/frame%d.png";
  g_object_set(sink, "location", outputLocation.c_str(), NULL);
  // We need to be notified every time a PNG is generated to activate our callback.
  g_object_set(sink, "post-messages", TRUE, NULL);

  // Setup the pipeline
  gst_bin_add_many(GST_BIN(pipeline), source, convert, filter, toPng, sink, NULL);
  CHECK_FALSE_SETFLAG_JUMP(gst_element_link(convert, filter), retFlag, cleanup);
  CHECK_FALSE_SETFLAG_JUMP(gst_element_link_filtered(filter, toPng, filterCaps), retFlag, cleanup);
  CHECK_FALSE_SETFLAG_JUMP(gst_element_link(toPng, sink), retFlag, cleanup);

  // This sets up the source to stream from the stream URL.
  g_object_set(source, "uri", streamUrl.c_str(), NULL);

  // Now we want to link up the source to the rest of the pipeline.
  // So when a pad is available in the source, make sure to add it.
  g_signal_connect(source, "pad-added", G_CALLBACK(VideoFetcher::SourcePadAddedHandler), &convert);

  // Start playing the video
  if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
    // TODO: ERROR
    std::cout << "ERROR: Could not start playing the video." << std::endl;
    retFlag = false;
    goto cleanup;
  }

  // Listen to any messages we might get
  bus = gst_element_get_bus(pipeline);

  while (!bComplete) {
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_ELEMENT));
    if (!msg) continue;
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &err, &debug_info);
      g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
      g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
      g_clear_error(&err);
      g_free(debug_info);
      bComplete = true;
      break;
    case GST_MESSAGE_EOS:
      bComplete = true;
      break;
    case GST_MESSAGE_ELEMENT:
    {
      // Check to see if it's the multifilesink telling us that it printed out another file. 
      // In that case, notify the client about the event with the file name as well as its timestamp.
      const GstStructure* data = gst_message_get_structure(msg);
      // First check our name and make sure it is GstMultiFileSink
      const gchar* name = gst_structure_get_name(data);
      if (strcmp(name, "GstMultiFileSink") == 0) {
        std::string path = gst_structure_get_string(data, "filename");
        mCallback(path, mFrameCount++);
      }
      break;
    }
    default:
      break;
    }
    gst_message_unref(msg);
  }
cleanup:
  gst_element_set_state(pipeline, GST_STATE_NULL);
  if (bus) gst_object_unref(bus);
  if (pipeline) gst_object_unref(pipeline);
  if (filterCaps) gst_caps_unref(filterCaps);
  return retFlag;
}

void VideoFetcher::SourcePadAddedHandler(GstElement* src, GstPad* pad, GstElement** sink) {
  GstPad* sinkPad = gst_element_get_static_pad(*sink, "sink");
  GstCaps* padCapabilities = NULL;
  GstPadLinkReturn ret;
  GstStructure* padStruct = NULL;
  // Nothing to do if this sink pad is already linked.
  if (!gst_pad_is_linked(sinkPad)) {
    // We want the pad that is outputting video. So we check the pad's capabilities.
    padCapabilities = gst_pad_query_caps(pad, NULL);
    padStruct = gst_caps_get_structure(padCapabilities, 0);
    const gchar* newPadType = gst_structure_get_name(padStruct);
    if (!g_str_has_prefix(newPadType, "video/x-raw")) {
      // Not a video ignore.
      goto cleanup;
    }

    // Link the source to the sink
    ret = gst_pad_link(pad, sinkPad);
    if (GST_PAD_LINK_FAILED(ret)) {
      goto cleanup;
    }
  }

cleanup:
  gst_object_unref(sinkPad);
  if (padCapabilities) gst_object_unref(padCapabilities);
}
