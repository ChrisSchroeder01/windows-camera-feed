#include "windows_camera_feed.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>
#include <windows.h>

using namespace godot;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return TRUE;
}

void initialize_windows_camera_feed(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;
    ClassDB::register_class<WindowsCameraNode>();
}

void uninitialize_windows_camera_feed(ModuleInitializationLevel p_level) {}

extern "C" {
GDExtensionBool GDE_EXPORT windows_camera_feed_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    GDExtensionClassLibraryPtr         p_library,
    GDExtensionInitialization*         r_initialization)
{
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
    init_obj.register_initializer(initialize_windows_camera_feed);
    init_obj.register_terminator(uninitialize_windows_camera_feed);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
    return init_obj.init();
}
}