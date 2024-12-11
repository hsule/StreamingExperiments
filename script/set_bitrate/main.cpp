#include <iostream>
#include <sstream>
#include <thread>
#include <atomic>
#include <gst/gst.h>

using namespace std;

static GstElement *encoder = nullptr;
static atomic<bool> running(true); 

void stdin_listener() {
    string input;
    while (running) {
        cout << "Enter new bitrate (in bps): ";
        if (getline(cin, input)) {

            try {
                int new_bitrate = stoi(input);
                if (new_bitrate > 0 && encoder) {
                    g_object_set(G_OBJECT(encoder), "bitrate", new_bitrate, NULL);
                    // g_printerr("Bitrate updated to %d bps\n",new_bitrate);
                } else {
                    // g_printerr("Invalid bitrate or encoder not initialized.\n");
                }
            } catch (exception &e) {
                g_printerr("Error: %s\n",e.what());
            }
        }
    }
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    // GStreamer Pipeline
    stringstream launch_stream;
    launch_stream
        << "videotestsrc is-live=1 ! "
        << "video/x-raw,format=NV12,width=1920,height=1080 ! "
        << "timeoverlay valignment=4 halignment=1 ! queue ! "
        << "nvvideoconvert ! nvv4l2h265enc idrinterval=15 name=video_enc ! "
        << "filesink location=/dev/stdout";

    string launch_string = launch_stream.str();
    cout << "Using launch string: " << launch_string << endl;

    GError *error = nullptr;
    GstPipeline *pipeline = (GstPipeline *)gst_parse_launch(launch_string.c_str(), &error);
    if (!pipeline) {
        cerr << "Failed to parse launch string: " << (error ? error->message : "Unknown error") << endl;
        if (error) g_error_free(error);
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

    // cout << "Exiting program." << endl;
    return 0;
}
