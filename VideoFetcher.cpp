#include "VideoFetcher.h"
#include <stdio.h>
#include <assert.h>

VideoFetcher::VideoFetcher(std::string Url, std::function<void(void*)> Callback): mURL(Url), mCallback(Callback) {

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
  FILE* ls = _popen(lsCommand.c_str(), "r");
  assert(ls != NULL);

  char buffer[300];
  std::string streamUrl = "";
  while (!feof(ls)) {
    if (fgets(buffer, 300, ls) != NULL) {
      streamUrl += buffer;
    }
  }
  _pclose(ls);
  return streamUrl;
}

/*
 * Start playing the stream using GStreamer. 
 */
bool VideoFetcher::BeginStreamPlayback(std::string& streamUrl) {
  GstElement* pipeline;
  GstElement* source;
  GstElement* convert;
  GstElement* sink;
  bool retFlag = true;
  
  // Initialize gst
  gst_init(NULL, NULL);

  // Create the pipeline element
  std::string pipelineName = "lollive-pipeline";
  pipeline = gst_pipeline_new(pipelineName.c_str());
  if (!pipeline) {
    // TODO: ERROR
    retFlag = false;
    goto cleanup;
  }

  // Create the individual pipeline stage elements
  // For the source, we use the passed in streamUrl.
  // For the sink, we want to create images and pass them back to whoever requested this stream.
  source = gst_element_factory_make("uridecodebin", "source");
  if (!source) {
    // TODO: ERROR
    retFlag = false;
    goto cleanup;
  }

  convert = gst_element_factory_make("videoconvert", "convert");
  if (!convert) {
    // TODO: ERROR
    retFlag = false;
    goto cleanup;
  }

  sink = gst_element_factory_make("autovideosink", "sink"); // TEMPORARY. Change to a file system thingy.
  if (!sink) {
    // TODO: ERROR
    retFlag = false;
    goto cleanup;
  }

  // Setup the pipeline
  gst_bin_add_many(GST_BIN(pipeline), source, convert, sink, NULL);
  if (!gst_element_link(convert, sink)) {
    // TODO: ERROR
    retFlag = false;
    goto cleanup;
  }

  // This sets up the source to stream from the stream URL.
  g_object_set(source, "uri", streamUrl.c_str(), NULL);

  // Now we want to link up the source to the rest of the pipeline.
  // So when a pad is available in the source, make sure to add it.
  g_signal_connect(source, "pad-added", G_CALLBACK(VideoFetcher::SourcePadAddedHandler), &convert);

  // Start playing the video
  if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
    // TODO: ERROR
    retFlag = false;
    goto cleanup;
  }

  // Listen to any messages we might get
  GstBus* bus = gst_element_get_bus(pipeline);
  GstMessage* msg;

  bool bComplete = false;
  while (!bComplete) {
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    if (!msg) continue;

    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &err, &debug_info);
      g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
      g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
      g_clear_error(&err);
      g_free(debug_info);
      bComplete = true;
      break;
    default:
      break;
    }
  }


cleanup:
  return retFlag;
}

void VideoFetcher::SourcePadAddedHandler(GstElement* src, GstPad* pad, GstElement** sink) {
  GstPad* sinkPad = gst_element_get_static_pad(*sink, "sink");
  GstCaps* padCapabilities = NULL;
  // Nothing to do if this sink pad is already linked.
  if (gst_pad_is_linked(sinkPad)) {
    goto cleanup;
  }

  // We want the pad that is outputting video. So we check the pad's capabilities.
  padCapabilities = gst_pad_query_caps(pad, NULL);
  GstStructure* padStruct = gst_caps_get_structure(padCapabilities, 0);
  const gchar* newPadType = gst_structure_get_name(padStruct);
  if (!g_str_has_prefix(newPadType, "video/x-raw")) {
    // Not a video ignore.
    goto cleanup;
  }

  // Link the source to the sink
  GstPadLinkReturn ret = gst_pad_link(pad, sinkPad);
  if (GST_PAD_LINK_FAILED(ret)) {
    goto cleanup;
  }

cleanup:
  gst_object_unref(sinkPad);
  if (padCapabilities) gst_object_unref(padCapabilities);
}