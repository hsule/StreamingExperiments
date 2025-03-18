#include <atomic>
#include <gst/gst.h>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

static GstElement *encoder = nullptr;
static GstElement *capsfilter = nullptr;
static GstPipeline *pipeline = nullptr;
static atomic<bool> running(true);

void update_encoder_property(const string &key, const string &value) {
  if (!encoder || !capsfilter) {
    g_printerr("Encoder or capsfilter not initialized.\n");
    return;
  }

  try {
    if (key == "br") {
      int bitrate = stoi(value);
      // ready
      GstStateChangeReturn ret =
          gst_element_set_state((GstElement *)pipeline, GST_STATE_READY);
      if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set pipeline to READY state.\n");
        return;
      }
      GstState current_state;
      gst_element_get_state((GstElement *)pipeline, &current_state, NULL,
                            GST_CLOCK_TIME_NONE);
      if (current_state != GST_STATE_READY) {
        g_printerr("Pipeline is not in READY state. Cannot set CQ.\n");
        return;
      }

      g_object_set(G_OBJECT(encoder), "maxbitrate", bitrate, NULL);
      g_object_get(G_OBJECT(encoder), "maxbitrate", &bitrate, NULL);
      g_printerr("Max bitrate updated to %d bps\n", bitrate);

      // PLAY
      ret = gst_element_set_state((GstElement *)pipeline, GST_STATE_PLAYING);
      if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set pipeline back to PLAYING state.\n");
      }

    } else if (key == "cq") {
      int cq = stoi(value);
      if (cq >= 0 && cq <= 51) { // Valid range: 0 to 51
        // ready
        GstStateChangeReturn ret =
            gst_element_set_state((GstElement *)pipeline, GST_STATE_READY);
        if (ret == GST_STATE_CHANGE_FAILURE) {
          g_printerr("Failed to set pipeline to READY state.\n");
          return;
        }
        GstState current_state;
        gst_element_get_state((GstElement *)pipeline, &current_state, NULL,
                              GST_CLOCK_TIME_NONE);
        if (current_state != GST_STATE_READY) {
          g_printerr("Pipeline is not in READY state. Cannot set CQ.\n");
          return;
        }

        g_object_set(G_OBJECT(encoder), "cq", cq, NULL);
        g_object_get(G_OBJECT(encoder), "cq", &cq, NULL);
        g_printerr("CQ value updated to %d\n", cq);

        // PLAY
        ret = gst_element_set_state((GstElement *)pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE) {
          g_printerr("Failed to set pipeline back to PLAYING state.\n");
        }
      } else {
        g_printerr("Invalid CQ value: %s. Accepted range: 0-51\n",
                   value.c_str());
      }
    } else if (key == "idr") {
      int idr_interval = stoi(value);
      g_object_set(G_OBJECT(encoder), "idrinterval", idr_interval, NULL);
      g_object_get(G_OBJECT(encoder), "idrinterval", &idr_interval, NULL);
      g_printerr("IDR interval updated to %d\n", idr_interval);
    } else if (key == "res") {
      size_t x_pos = value.find('x');
      if (x_pos == string::npos) {
        g_printerr("Invalid resolution format. Use widthxheight (e.g., 1280x720)\n");
        return;
      }

      int width = stoi(value.substr(0, x_pos));
      int height = stoi(value.substr(x_pos + 1));

      GstStateChangeReturn ret = gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_READY);
      if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set pipeline to READY state.\n");
        return;
      }

      GstCaps *new_caps = gst_caps_new_simple("video/x-raw",
                                              "format", G_TYPE_STRING, "NV12",
                                              "width", G_TYPE_INT, width,
                                              "height", G_TYPE_INT, height,
                                              NULL);
      g_object_set(G_OBJECT(capsfilter), "caps", new_caps, NULL);
      gst_caps_unref(new_caps);
      g_printerr("Resolution updated to %dx%d\n", width, height);

      gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
    } else {
      g_printerr("Unknown property: %s. Accepted properties: br, cq, idr, res\n",
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
  launch_stream
      << "videotestsrc is-live=1 ! "
      << "videoscale ! capsfilter name=capsfilter caps=video/x-raw,format=NV12,width=1920,height=1080 ! "
      << "timeoverlay valignment=4 halignment=1 ! queue ! "
      << "tslatencystamper ! "
      << "nvvideoconvert ! "
      << "nvv4l2h265enc control-rate=0 name=video_enc ! "
      << "filesink location=/dev/stdout";

  string launch_string = launch_stream.str();

  GError *error = nullptr;
  pipeline = (GstPipeline *)gst_parse_launch(launch_string.c_str(), &error);
  if (!pipeline) {
    cerr << "Failed to parse launch string: "
        << (error ? error->message : "Unknown error") << endl;
    if (error) 
      g_error_free(error);
    return -1;
  }

  encoder = gst_bin_get_by_name(GST_BIN(pipeline), "video_enc");
  capsfilter = gst_bin_get_by_name(GST_BIN(pipeline), "capsfilter");

  if (!encoder || !capsfilter) {
    cerr << "Failed to find encoder or capsfilter in pipeline." << endl;
    gst_object_unref(pipeline);
    return -1;
  }

  gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

  thread stdin_thread(stdin_listener);

  GstBus *bus = gst_pipeline_get_bus(pipeline);
  gst_bus_poll(bus, GST_MESSAGE_EOS, GST_CLOCK_TIME_NONE);

  running = false;
  stdin_thread.join();

  gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
  gst_object_unref(bus);
  gst_object_unref(pipeline);

  return 0;
}
