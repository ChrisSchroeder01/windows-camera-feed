#include "windows_camera_feed.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/camera_server.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <opencv2/imgproc.hpp>

namespace godot
{

    WindowsCameraNode::WindowsCameraNode() {}

    WindowsCameraNode::~WindowsCameraNode()
    {
        _stop();
    }

    void WindowsCameraNode::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("set_camera_index", "index"), &WindowsCameraNode::set_camera_index);
        ClassDB::bind_method(D_METHOD("get_camera_index"), &WindowsCameraNode::get_camera_index);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "camera_index"), "set_camera_index", "get_camera_index");
    }

    void WindowsCameraNode::_notification(int p_what)
    {
        switch (p_what)
        {
        case NOTIFICATION_READY:
            if (Engine::get_singleton()->is_editor_hint())
            {
                return;
            }
            _start();
            break;
        case NOTIFICATION_EXIT_TREE:
            _stop();
            break;
        }
    }

    void WindowsCameraNode::set_camera_index(int p_index) { _camera_index = p_index; }
    int WindowsCameraNode::get_camera_index() const { return _camera_index; }

    void WindowsCameraNode::_start()
    {
        if (_running)
            return;

        // Create a plain CameraFeed — no subclassing needed
        _feed.instantiate();
        _feed->set_name("OpenCVCamera");
        _feed->set_position(CameraFeed::FEED_FRONT);

        CameraServer *cs = CameraServer::get_singleton();
        if (!cs)
        {

            return;
        }
        cs->add_feed(_feed);

        _cap.open(_camera_index, cv::CAP_DSHOW);
        if (!_cap.isOpened())
        {

            cs->remove_feed(_feed);
            _feed.unref();
            return;
        }

        _running = true;
        _thread = std::thread(&WindowsCameraNode::_capture_loop, this);
    }

    void WindowsCameraNode::_stop()
    {
        if (!_running)
            return;
        _running = false;
        if (_thread.joinable())
            _thread.join();
        _cap.release();

        CameraServer *cs = CameraServer::get_singleton();
        if (cs && _feed.is_valid())
        {
            cs->remove_feed(_feed);
        }
        _feed.unref();
    }

    void WindowsCameraNode::_capture_loop()
    {
        cv::Mat frame_bgr, frame_rgb;
        while (_running)
        {
            if (!_cap.read(frame_bgr) || frame_bgr.empty())
                continue;

            cv::cvtColor(frame_bgr, frame_rgb, cv::COLOR_BGR2RGB);

            PackedByteArray bytes;
            bytes.resize(frame_rgb.total() * frame_rgb.elemSize());
            memcpy(bytes.ptrw(), frame_rgb.data, bytes.size());

            Ref<Image> img = Image::create_from_data(
                frame_rgb.cols, frame_rgb.rows,
                false, Image::FORMAT_RGB8, bytes);

            _feed->set_rgb_image(img);
        }
    }

} // namespace godot