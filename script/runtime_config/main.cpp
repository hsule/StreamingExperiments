#include <atomic>
#include <gst/gst.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_map>

using namespace std;

static GstElement *encoder = nullptr;
static atomic<bool> running(true);

void update_encoder_property(const string &key, const string &value) {
  if (!encoder) {
    g_printerr("Encoder not initialized.\n");
    return;
  }

  try {
    if (key == "br") {
      int bitrate = stoi(value);
      g_object_set(G_OBJECT(encoder), "bitrate", bitrate, NULL);
      g_printerr("Bitrate updated to %d bps\n", bitrate);
    } else if (key == "cq") {
      int cq = stoi(value);
      if (cq >= 0 && cq <= 51) { // Valid range: 0 to 51
        g_object_set(G_OBJECT(encoder), "cq", cq, NULL);
        g_printerr("CQ value updated to %d\n", cq);
      } else {
        g_printerr("Invalid CQ value: %s. Accepted range: 0-51\n",
                   value.c_str());
      }
    } else if (key == "idr") {
      int idr_interval = stoi(value);
      g_object_set(G_OBJECT(encoder), "idrinterval", idr_interval, NULL);
      g_printerr("IDR interval updated to %d\n", idr_interval);
    } else {
      g_printerr("Unknown property: %s. Accepted properties: br, cq, idr\n",
                 key.c_str());
    }
  } catch (exception &e) {
    g_printerr("Error processing input: %s\n", e.what());
  }
}

void stdin_listener() {
  string input;
  while (running) {
    if (getline(cin, input)) {
      size_t delimiter_pos = input.find('=');
      if (delimiter_pos == string::npos) {
        g_printerr("Invalid input format. Use key=value\n");
        continue;
      }

      string key = input.substr(0, delimiter_pos);
      string value = input.substr(delimiter_pos + 1);

      update_encoder_property(key, value);
    }
  }
}

int main(int argc, char **argv) {
  gst_init(&argc, &argv);

  // GStreamer Pipeline
  stringstream launch_stream;
  launch_stream << "videotestsrc is-live=1 ! "
                << "video/x-raw,format=NV12,width=1920,height=1080 ! "
                << "timeoverlay valignment=4 halignment=1 ! queue ! "
                << "nvvideoconvert ! nvv4l2h265enc name=video_enc ! "
                << "filesink location=/dev/stdout";

  string launch_string = launch_stream.str();
  cout << "Using launch string: " << launch_string << endl;

  GError *error = nullptr;
  GstPipeline *pipeline =
      (GstPipeline *)gst_parse_launch(launch_string.c_str(), &error);
  if (!pipeline) {
    cerr << "Failed to parse launch string: "
         << (error ? error->message : "Unknown error") << endl;
    if (error)
      g_error_free(error);
    return -1;
  }

  encoder = gst_bin_get_by_name(GST_BIN(pipeline), "video_enc");
  if (!encoder) {
    cerr << "Failed to find encoder in pipeline." << endl;
    gst_object_unref(pipeline);
    return -1;
  }

  gst_element_set_state((GstElement *)pipeline, GST_STATE_PLAYING);

  thread stdin_thread(stdin_listener);

  GstBus *bus = gst_pipeline_get_bus(pipeline);
  gst_bus_poll(bus, GST_MESSAGE_EOS, GST_CLOCK_TIME_NONE);

  running = false;
  stdin_thread.join();

  gst_element_set_state((GstElement *)pipeline, GST_STATE_NULL);
  gst_object_unref(bus);
  gst_object_unref(pipeline);

  return 0;
}