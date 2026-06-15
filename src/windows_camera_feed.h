#pragma once

#include <godot_cpp/classes/camera_feed.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <opencv2/videoio.hpp>

#include <atomic>
#include <thread>

namespace godot {

class WindowsCameraFeed : public CameraFeed {
    GDCLASS(WindowsCameraFeed, CameraFeed)

public:
    WindowsCameraFeed();
    ~WindowsCameraFeed();

    // Called by Godot when the feed is activated/deactivated
    bool _activate_feed() override;
    void _deactivate_feed() override;

    // Open a specific camera index (default 0)
    void set_camera_index(int p_index);
    int  get_camera_index() const;

protected:
    static void _bind_methods();

private:
    void _capture_loop();

    cv::VideoCapture _cap;
    std::thread      _thread;
    std::atomic_bool _running{ false };
    int              _camera_index{ 0 };
};

} // namespace godot