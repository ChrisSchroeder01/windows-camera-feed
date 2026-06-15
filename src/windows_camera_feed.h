#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/camera_feed.hpp>
#include <opencv2/videoio.hpp>
#include <atomic>
#include <thread>
#include <fstream>
#include <string>

namespace godot {

class WindowsCameraNode : public Node {
    GDCLASS(WindowsCameraNode, Node)

public:
    WindowsCameraNode();
    ~WindowsCameraNode();

    void set_camera_index(int p_index);
    int  get_camera_index() const;

protected:
    static void _bind_methods();
    void _notification(int p_what);

private:
    void _start();
    void _stop();
    void _capture_loop();

    cv::VideoCapture _cap;
    std::thread      _thread;
    std::atomic_bool _running{ false };
    int              _camera_index{ 0 };
    Ref<CameraFeed>  _feed;
};

} // namespace godot