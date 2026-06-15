#include "windows_camera_feed.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencv2/imgproc.hpp>

namespace godot {

WindowsCameraFeed::WindowsCameraFeed() {
    set_name("WindowsCameraFeed");
    set_position(CameraFeed::FEED_FRONT);
}

WindowsCameraFeed::~WindowsCameraFeed() {
    if (_running) {
        _running = false;
        if (_thread.joinable()) _thread.join();
    }
}

void WindowsCameraFeed::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_camera_index", "index"), &WindowsCameraFeed::set_camera_index);
    ClassDB::bind_method(D_METHOD("get_camera_index"),          &WindowsCameraFeed::get_camera_index);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "camera_index"), "set_camera_index", "get_camera_index");
}

void WindowsCameraFeed::set_camera_index(int p_index) {
    _camera_index = p_index;
}

int WindowsCameraFeed::get_camera_index() const {
    return _camera_index;
}

bool WindowsCameraFeed::_activate_feed() {
    if (_running) return true;

    _cap.open(_camera_index, cv::CAP_DSHOW);
    if (!_cap.isOpened()) {
        UtilityFunctions::printerr("WindowsCameraFeed: failed to open camera index ", _camera_index);
        return false;
    }

    _running = true;
    _thread  = std::thread(&WindowsCameraFeed::_capture_loop, this);
    return true;
}

void WindowsCameraFeed::_deactivate_feed() {
    _running = false;
    if (_thread.joinable()) _thread.join();
    _cap.release();
}

void WindowsCameraFeed::_capture_loop() {
    cv::Mat frame_bgr;
    cv::Mat frame_rgb;

    while (_running) {
        if (!_cap.read(frame_bgr) || frame_bgr.empty()) continue;

        cv::cvtColor(frame_bgr, frame_rgb, cv::COLOR_BGR2RGB);

        // Wrap into a Godot Image (no copy — data pointer is valid for this scope)
        PackedByteArray bytes;
        bytes.resize(frame_rgb.total() * frame_rgb.elemSize());
        memcpy(bytes.ptrw(), frame_rgb.data, bytes.size());

        Ref<Image> img = Image::create_from_data(
            frame_rgb.cols,
            frame_rgb.rows,
            false,
            Image::FORMAT_RGB8,
            bytes
        );

        // Push the frame into the CameraFeed — Godot signals frame_changed automatically
        set_rgb_image(img);
    }
}

} // namespace godot