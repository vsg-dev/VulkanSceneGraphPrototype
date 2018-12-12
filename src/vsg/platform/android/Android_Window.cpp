/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/platform/android/Android_Window.h>

#include <android/log.h>
#include <android/looper.h>
#include <android/native_activity.h>

#include <vsg/core/observer_ptr.h>
#include <vsg/ui/KeyEvent.h>
#include <vsg/ui/TouchEvent.h>
#include <vsg/vk/Extensions.h>

#include <iostream>
#include <time.h>

using namespace vsg;
using namespace vsgAndroid;

#define LOG(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

namespace vsg
{
    // Provide the Window::create(...) implementation that automatically maps to an Android_Window
    Window::Result Window::create(const Window::Traits& traits, bool debugLayer, bool apiDumpLayer, vsg::AllocationCallbacks* allocator)
    {
        return vsgAndroid::Android_Window::create(traits, debugLayer, apiDumpLayer, allocator);
    }
} // namespace vsg

namespace vsgAndroid
{
    vsg::Names vsgAndroid::getInstanceExtensions()
    {
        // check the extensions are avaliable first
        Names requiredExtensions = {"VK_KHR_surface", "VK_KHR_android_surface"};

        if (!vsg::isExtensionListSupported(requiredExtensions))
        {
            std::cout << "Error: vsg::getInstanceExtensions(...) unable to create window, VK_KHR_surface or VK_KHR_android_surface not supported." << std::endl;
            return Names();
        }

        return requiredExtensions;
    }

    class AndroidSurface : public vsg::Surface
    {
    public:
        AndroidSurface(vsg::Instance* instance, ANativeWindow* window, vsg::AllocationCallbacks* allocator = nullptr) :
            vsg::Surface(VK_NULL_HANDLE, instance, allocator)
        {
            VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo{};
            surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
            surfaceCreateInfo.pNext = nullptr;
            surfaceCreateInfo.flags = 0;
            surfaceCreateInfo.window = window;

            auto result = vkCreateAndroidSurfaceKHR(*instance, &surfaceCreateInfo, nullptr, &_surface);
        }
    };

    static int64_t now_ms(void)
    {
        struct timespec res;
        clock_gettime(CLOCK_MONOTONIC, &res);
        return 1000 * res.tv_sec + res.tv_nsec / 1e6;
    }

} // namespace vsgAndroid

KeyboardMap::KeyboardMap()
{
    _keycodeMap =
        {
            {0x0, KEY_Undefined},

            {AKEYCODE_SPACE, KEY_Space},

            {AKEYCODE_0, KEY_0},
            {AKEYCODE_1, KEY_1},
            {AKEYCODE_2, KEY_2},
            {AKEYCODE_3, KEY_3},
            {AKEYCODE_4, KEY_4},
            {AKEYCODE_5, KEY_5},
            {AKEYCODE_6, KEY_6},
            {AKEYCODE_7, KEY_7},
            {AKEYCODE_8, KEY_8},
            {AKEYCODE_9, KEY_9},

            {'a', KEY_a},
            {'b', KEY_b},
            {'c', KEY_c},
            {'d', KEY_d},
            {'e', KEY_e},
            {'f', KEY_f},
            {'g', KEY_g},
            {'h', KEY_h},
            {'i', KEY_i},
            {'j', KEY_j},
            {'k', KEY_k},
            {'l', KEY_l},
            {'m', KEY_m},
            {'n', KEY_n},
            {'o', KEY_o},
            {'p', KEY_p},
            {'q', KEY_q},
            {'r', KEY_r},
            {'s', KEY_s},
            {'t', KEY_t},
            {'u', KEY_u},
            {'z', KEY_v},
            {'w', KEY_w},
            {'x', KEY_x},
            {'y', KEY_y},
            {'z', KEY_z},

            {'A', KEY_A},
            {'B', KEY_B},
            {'C', KEY_C},
            {'D', KEY_D},
            {'E', KEY_E},
            {'F', KEY_F},
            {'G', KEY_G},
            {'H', KEY_H},
            {'I', KEY_I},
            {'J', KEY_J},
            {'K', KEY_K},
            {'L', KEY_L},
            {'M', KEY_M},
            {'N', KEY_N},
            {'O', KEY_O},
            {'P', KEY_P},
            {'Q', KEY_Q},
            {'R', KEY_R},
            {'S', KEY_S},
            {'T', KEY_T},
            {'U', KEY_U},
            {'V', KEY_V},
            {'W', KEY_W},
            {'X', KEY_X},
            {'Y', KEY_Y},
            {'Z', KEY_Z},

            {'!', KEY_Exclaim},
            {'"', KEY_Quotedbl},
            {'#', KEY_Hash},
            {'$', KEY_Dollar},
            {'&', KEY_Ampersand},
            {AKEYCODE_APOSTROPHE, KEY_Quote},
            {'(', KEY_Leftparen},
            {')', KEY_Rightparen},
            {'*', KEY_Asterisk},
            {'+', KEY_Plus},
            {AKEYCODE_COMMA, KEY_Comma},
            {AKEYCODE_MINUS, KEY_Minus},
            {AKEYCODE_PERIOD, KEY_Period},
            {AKEYCODE_SLASH, KEY_Slash},
            {':', KEY_Colon},
            {AKEYCODE_SEMICOLON, KEY_Semicolon},
            {'<', KEY_Less},
            {AKEYCODE_EQUALS, KEY_Equals}, // + isnt an unmodded key, why does windows map is as a virtual??
            {'>', KEY_Greater},
            {'?', KEY_Question},
            {AKEYCODE_AT, KEY_At},
            {AKEYCODE_LEFT_BRACKET, KEY_Leftbracket},
            {AKEYCODE_BACKSLASH, KEY_Backslash},
            {AKEYCODE_RIGHT_BRACKET, KEY_Rightbracket},
            {'|', KEY_Caret},
            {'_', KEY_Underscore},
            {'`', KEY_Backquote},

            {AKEYCODE_DEL, KEY_BackSpace}, /* back space, back char */
            {AKEYCODE_TAB, KEY_Tab},
            //    KEY_Linefeed = 0xFF0A, /* Linefeed, LF */
            {AKEYCODE_CLEAR, KEY_Clear},
            {AKEYCODE_ENTER, KEY_Return}, /* Return, enter */
            {AKEYCODE_BREAK, KEY_Pause},  /* Pause, hold */
            {AKEYCODE_SCROLL_LOCK, KEY_Scroll_Lock},
            //    KEY_Sys_Req = 0xFF15,
            {AKEYCODE_ESCAPE, KEY_Escape},
            {AKEYCODE_FORWARD_DEL, KEY_Delete}, /* Delete, rubout */

            /* Cursor control & motion */

            {AKEYCODE_MOVE_HOME, KEY_Home},
            {AKEYCODE_DPAD_LEFT, KEY_Left},          /* Move left, left arrow */
            {AKEYCODE_DPAD_UP, KEY_Up},              /* Move up, up arrow */
            {AKEYCODE_DPAD_RIGHT, KEY_Right},        /* Move right, right arrow */
            {AKEYCODE_DPAD_DOWN, KEY_Down},          /* Move down, down arrow */
            {AKEYCODE_NAVIGATE_PREVIOUS, KEY_Prior}, /* Prior, previous */
            {AKEYCODE_PAGE_UP, KEY_Page_Up},
            {AKEYCODE_NAVIGATE_NEXT, KEY_Next}, /* Next */
            {AKEYCODE_PAGE_DOWN, KEY_Page_Down},
            {AKEYCODE_MOVE_END, KEY_End}, /* EOL */
            //{ KEY_Begin = 0xFF58, /* BOL */

            /* Misc Functions */

            //{ VK_SELECT, KEY_Select }, /* Select, mark */
            //{ VK_PRINT, KEY_Print },
            //{ VK_EXECUTE, KEY_Execute }, /* Execute, run, do */
            {AKEYCODE_INSERT, KEY_Insert}, /* Insert, insert here */
            //{ KEY_Undo = 0xFF65,    /* Undo, oops */
            //KEY_Redo = 0xFF66,    /* redo, again */
            {AKEYCODE_MENU, KEY_Menu}, /* On Windows, this is VK_APPS, the context-menu key */
            // KEY_Find = 0xFF68,    /* Find, search */
            //{ VK_CANCEL, KEY_Cancel },  /* Cancel, stop, abort, exit */
            {AKEYCODE_HELP, KEY_Help}, /* Help */
            //{ KEY_Break = 0xFF6B,
            //KEY_Mode_switch = 0xFF7E,   /* Character set switch */
            //KEY_Script_switch = 0xFF7E, /* Alias for mode_switch */
            {AKEYCODE_NUM_LOCK, KEY_Num_Lock},

            /* Keypad Functions, keypad numbers cleverly chosen to map to ascii */

            //KEY_KP_Space = 0xFF80, /* space */
            //KEY_KP_Tab = 0xFF89,
            {AKEYCODE_NUMPAD_ENTER, KEY_KP_Enter}, /* enter */
            //KEY_KP_F1 = 0xFF91,    /* PF1, KP_A, ... */
            //KEY_KP_F2 = 0xFF92,
            //KEY_KP_F3 = 0xFF93,
            //KEY_KP_F4 = 0xFF94,
            //KEY_KP_Home = 0xFF95,
            //KEY_KP_Left = 0xFF96,
            //KEY_KP_Up = 0xFF97,
            //KEY_KP_Right = 0xFF98,
            //KEY_KP_Down = 0xFF99,
            //KEY_KP_Prior = 0xFF9A,
            //KEY_KP_Page_Up = 0xFF9A,
            //KEY_KP_Next = 0xFF9B,
            //KEY_KP_Page_Down = 0xFF9B,
            //KEY_KP_End = 0xFF9C,
            //KEY_KP_Begin = 0xFF9D,
            //KEY_KP_Insert = 0xFF9E,
            //KEY_KP_Delete = 0xFF9F,
            {AKEYCODE_NUMPAD_EQUALS, KEY_KP_Equal}, /* equals */
            {AKEYCODE_NUMPAD_MULTIPLY, KEY_KP_Multiply},
            {AKEYCODE_NUMPAD_ADD, KEY_KP_Add},
            {AKEYCODE_NUMPAD_COMMA, KEY_KP_Separator}, /* separator, often comma */
            {AKEYCODE_NUMPAD_SUBTRACT, KEY_KP_Subtract},
            {AKEYCODE_NUMPAD_DOT, KEY_KP_Decimal},
            {AKEYCODE_NUMPAD_DIVIDE, KEY_KP_Divide},

            {AKEYCODE_NUMPAD_0, KEY_KP_0},
            {AKEYCODE_NUMPAD_1, KEY_KP_1},
            {AKEYCODE_NUMPAD_2, KEY_KP_2},
            {AKEYCODE_NUMPAD_3, KEY_KP_3},
            {AKEYCODE_NUMPAD_4, KEY_KP_4},
            {AKEYCODE_NUMPAD_5, KEY_KP_5},
            {AKEYCODE_NUMPAD_6, KEY_KP_6},
            {AKEYCODE_NUMPAD_7, KEY_KP_7},
            {AKEYCODE_NUMPAD_8, KEY_KP_8},
            {AKEYCODE_NUMPAD_9, KEY_KP_9},

            /*
        * Auxiliary Functions; note the duplicate definitions for left and right
        * function keys;  Sun keyboards and a few other manufactures have such
        * function key groups on the left and/or right sides of the keyboard.
        * We've not found a keyboard with more than 35 function keys total.
        */

            {AKEYCODE_F1, KEY_F1},
            {AKEYCODE_F2, KEY_F2},
            {AKEYCODE_F3, KEY_F3},
            {AKEYCODE_F4, KEY_F4},
            {AKEYCODE_F5, KEY_F5},
            {AKEYCODE_F6, KEY_F6},
            {AKEYCODE_F7, KEY_F7},
            {AKEYCODE_F8, KEY_F8},
            {AKEYCODE_F9, KEY_F9},
            {AKEYCODE_F10, KEY_F10},
            {AKEYCODE_F11, KEY_F11},
            {AKEYCODE_F12, KEY_F12},
            //{ VK_F13, KEY_F13 },
            //{ VK_F14, KEY_F14 },
            //{ VK_F15, KEY_F15 },
            //{ VK_F16, KEY_F16 },
            //{ VK_F17, KEY_F17 },
            //{ VK_F18, KEY_F18 },
            //{ VK_F19, KEY_F19 },
            //{ VK_F20, KEY_F20 },
            //{ VK_F21, KEY_F21 },
            //{ VK_F22, KEY_F22 },
            //{ VK_F23, KEY_F23 },
            //{ VK_F24, KEY_F24 },

            //KEY_F25 = 0xFFD6,
            //KEY_F26 = 0xFFD7,
            //KEY_F27 = 0xFFD8,
            //KEY_F28 = 0xFFD9,
            //KEY_F29 = 0xFFDA,
            //KEY_F30 = 0xFFDB,
            //KEY_F31 = 0xFFDC,
            //KEY_F32 = 0xFFDD,
            //KEY_F33 = 0xFFDE,
            //KEY_F34 = 0xFFDF,
            //KEY_F35 = 0xFFE0,

            /* Modifiers */

            {AKEYCODE_SHIFT_LEFT, KEY_Shift_L},   /* Left shift */
            {AKEYCODE_SHIFT_RIGHT, KEY_Shift_R},  /* Right shift */
            {AKEYCODE_CTRL_LEFT, KEY_Control_L},  /* Left control */
            {AKEYCODE_CTRL_RIGHT, KEY_Control_R}, /* Right control */
            {AKEYCODE_CAPS_LOCK, KEY_Caps_Lock},  /* Caps lock */
            //KEY_Shift_Lock = 0xFFE6, /* Shift lock */

            {AKEYCODE_META_LEFT, KEY_Meta_L},  /* Left meta */
            {AKEYCODE_META_RIGHT, KEY_Meta_R}, /* Right meta */
            {AKEYCODE_ALT_LEFT, KEY_Alt_L},    /* Left alt */
            {AKEYCODE_ALT_RIGHT, KEY_Alt_R},   /* Right alt */
            //{ VK_LWIN, KEY_Super_L }, /* Left super */
            //{ VK_RWIN, KEY_Super_R } /* Right super */
            //KEY_Hyper_L = 0xFFED, /* Left hyper */
            //KEY_Hyper_R = 0xFFEE  /* Right hyper */
        };
}

vsg::Window::Result Android_Window::create(const vsg::Window::Traits& traits, bool debugLayer, bool apiDumpLayer, vsg::AllocationCallbacks* allocator)
{
    try
    {
        ref_ptr<Window> window(new Android_Window(traits, debugLayer, apiDumpLayer, allocator));
        return Result(window);
    }
    catch (vsg::Window::Result result)
    {
        return result;
    }
}

Android_Window::Android_Window(const vsg::Window::Traits& traits, bool debugLayer, bool apiDumpLayer, vsg::AllocationCallbacks* allocator)
{
    _keyboard = new KeyboardMap;

    /*if(!traits.nativeHandle.has_value())
    {
        return Result("Error: vsg::Android_Window::create(...) failed to create Window, Android requires a NativeWindow passed via traits.nativeHandle.", VK_ERROR_INVALID_EXTERNAL_HANDLE);
    }*/

    //ANativeWindow* nativeWindow = *std::any_cast<ANativeWindow*>(&traits.nativeHandle);
    ANativeWindow* nativeWindow = static_cast<ANativeWindow*>(traits.nativeWindow);

    if (nativeWindow == nullptr)
    {
        throw Result("Error: vsg::Android_Window::create(...) failed to create Window, traits.nativeHandle is not a valid ANativeWindow.", VK_ERROR_INVALID_EXTERNAL_HANDLE);
    }

    _window = nativeWindow;

    // we could get the width height from the window?
    uint32_t finalWidth = traits.width;
    uint32_t finalHeight = traits.height;

    vsg::ref_ptr<Android_Window> window;

    if (traits.shareWindow)
    {
        // create Android surface for the ANativeWindow
        vsg::ref_ptr<vsg::Surface> surface(new vsgAndroid::AndroidSurface(traits.shareWindow->instance(), nativeWindow, allocator));

        _instance = traits.shareWindow->instance();
        _surface = surface;
        _physicalDevice = traits.shareWindow->physicalDevice();
        _device = traits.shareWindow->device();
        _renderPass = traits.shareWindow->renderPass();
        _debugLayersEnabled = traits.shareWindow->debugLayersEnabled();

        // share the _instance, _physicalDevice and _device;
        window->share(*traits.shareWindow);

        // temporary hack to force vkGetPhysicalDeviceSurfaceSupportKHR to be called as the Vulkan
        // debug layer is complaining about vkGetPhysicalDeviceSurfaceSupportKHR not being called
        // for this _surface prior to swap chain creation
        vsg::ref_ptr<vsg::PhysicalDevice> physicalDevice = vsg::PhysicalDevice::create(traits.shareWindow->instance(), VK_QUEUE_GRAPHICS_BIT, surface);
    }
    else
    {
        vsg::Names instanceExtensions = vsgAndroid::getInstanceExtensions();

        vsg::Names requestedLayers;
        if (debugLayer)
        {
            instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            requestedLayers.push_back("VK_LAYER_LUNARG_standard_validation");
            if (apiDumpLayer) requestedLayers.push_back("VK_LAYER_LUNARG_api_dump");
        }

        vsg::Names validatedNames = vsg::validateInstancelayerNames(requestedLayers);

        vsg::Names deviceExtensions;
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vsg::ref_ptr<vsg::Instance> instance = vsg::Instance::create(instanceExtensions, validatedNames, allocator);
        if (!instance) throw Result("Error: vsg::Android_Window::create(...) failed to create Window, unable to create Vulkan instance.", VK_ERROR_INVALID_EXTERNAL_HANDLE);

        // use GLFW to create surface
        vsg::ref_ptr<vsg::Surface> surface(new vsgAndroid::AndroidSurface(instance, nativeWindow, allocator));
        if (!surface) throw Result("Error: vsg::Android_Window::create(...) failed to create Window, unable to create Win32Surface.", VK_ERROR_INVALID_EXTERNAL_HANDLE);

        // set up device
        vsg::ref_ptr<vsg::PhysicalDevice> physicalDevice = vsg::PhysicalDevice::create(instance, VK_QUEUE_GRAPHICS_BIT, surface);
        if (!physicalDevice) throw Result("Error: vsg::Android_Window::create(...) failed to create Window, no Vulkan PhysicalDevice supported.", VK_ERROR_INVALID_EXTERNAL_HANDLE);

        vsg::ref_ptr<vsg::Device> device = vsg::Device::create(physicalDevice, validatedNames, deviceExtensions, allocator);
        if (!device) throw Result("Error: vsg::Android_Window::create(...) failed to create Window, unable to create Vulkan logical Device.", VK_ERROR_INVALID_EXTERNAL_HANDLE);

        // set up renderpass with the imageFormat that the swap chain will use
        vsg::SwapChainSupportDetails supportDetails = vsg::querySwapChainSupport(*physicalDevice, *surface);
        VkSurfaceFormatKHR imageFormat = vsg::selectSwapSurfaceFormat(supportDetails);
        VkFormat depthFormat = VK_FORMAT_D24_UNORM_S8_UINT; //VK_FORMAT_D32_SFLOAT; // VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_SFLOAT_S8_UINT
        vsg::ref_ptr<vsg::RenderPass> renderPass = vsg::RenderPass::create(device, imageFormat.format, depthFormat, allocator);
        if (!renderPass) throw Result("Error: vsg::Android_Window::create(...) failed to create Window, unable to create Vulkan RenderPass.", VK_ERROR_INVALID_EXTERNAL_HANDLE);

        _instance = instance;
        _surface = surface;
        _physicalDevice = physicalDevice;
        _device = device;
        _renderPass = renderPass;
        _debugLayersEnabled = debugLayer;
    }

    buildSwapchain(finalWidth, finalHeight);

    _first_android_timestamp = now_ms();
    _first_android_time_point = vsg::clock::now();
}

Android_Window::~Android_Window()
{
    clear();

    if (_window != nullptr)
    {
        std::cout << "Calling DestroyWindow(_window);" << std::endl;
    }
}

bool Android_Window::pollEvents(vsg::Events& events)
{
    if (_bufferedEvents.size() > 0)
    {
        events.splice(events.end(), _bufferedEvents);
        _bufferedEvents.clear();
        return true;
    }

    return false;
}

bool Android_Window::resized() const
{
    // just hack resize for now
    int width = int(_extent2D.width);
    int height = int(_extent2D.height);

    return (width != int(_extent2D.width) || height != int(_extent2D.height));
}

void Android_Window::resize()
{
    int width = int(_extent2D.width);
    int height = int(_extent2D.height);

    buildSwapchain(width, height);
}

bool Android_Window::handleAndroidInputEvent(AInputEvent* anEvent)
{
    auto eventType = AInputEvent_getType(anEvent);

    if (eventType == AINPUT_EVENT_TYPE_MOTION)
    {
        auto action = AMotionEvent_getAction(anEvent);

        // first process the historical events (mutiple touch events may have occured since the last frame)
        auto historySize = AMotionEvent_getHistorySize(anEvent);
        auto pointerCount = AMotionEvent_getPointerCount(anEvent);
        for (auto h = 0; h < historySize; h++)
        {
            int64_t htime = AMotionEvent_getHistoricalEventTime(anEvent, h) / 1e-6;
            vsg::clock::time_point historical_event_time = _first_android_time_point + std::chrono::milliseconds(htime - _first_android_timestamp);

            for (int p = 0; p < pointerCount; p++)
            {
                uint32_t id = AMotionEvent_getPointerId(anEvent, p);
                float x = AMotionEvent_getHistoricalX(anEvent, p, h);
                float y = AMotionEvent_getHistoricalX(anEvent, p, h);

                switch (action)
                {
                case AMOTION_EVENT_ACTION_DOWN:
                    _bufferedEvents.emplace_back(new vsg::TouchDownEvent(this, historical_event_time, x, y, id));
                    break;
                case AMOTION_EVENT_ACTION_MOVE:
                    _bufferedEvents.emplace_back(new vsg::TouchMoveEvent(this, historical_event_time, x, y, id));
                    break;
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_CANCEL: // for now just treat cancel as up
                    _bufferedEvents.emplace_back(new vsg::TouchUpEvent(this, historical_event_time, x, y, id));
                    break;
                default: break;
                }
            }
        }

        // now process the latest event
        int64_t ctime = AMotionEvent_getEventTime(anEvent) / 1e-6;
        vsg::clock::time_point event_time = _first_android_time_point + std::chrono::milliseconds(ctime - _first_android_timestamp);

        for (int p = 0; p < pointerCount; p++)
        {
            uint32_t id = AMotionEvent_getPointerId(anEvent, p);
            float x = AMotionEvent_getX(anEvent, p);
            float y = AMotionEvent_getY(anEvent, p);

            LOG("touch xy: %f, %f", x, y);

            switch (action)
            {
            case AMOTION_EVENT_ACTION_DOWN:
                _bufferedEvents.emplace_back(new vsg::TouchDownEvent(this, event_time, x, y, id));
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                _bufferedEvents.emplace_back(new vsg::TouchMoveEvent(this, event_time, x, y, id));
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_CANCEL: // for now just treat cancel as up
                _bufferedEvents.emplace_back(new vsg::TouchUpEvent(this, event_time, x, y, id));
                break;
            default: break;
            }
        }
        return true;
    }
    else if (eventType == AINPUT_EVENT_TYPE_KEY)
    {
        auto action = AKeyEvent_getAction(anEvent);

        int64_t time = AKeyEvent_getEventTime(anEvent) / 1e-6;
        vsg::clock::time_point event_time = _first_android_time_point + std::chrono::milliseconds(time - _first_android_timestamp);

        int32_t keycode = AKeyEvent_getKeyCode(anEvent);
        int32_t metastate = AKeyEvent_getMetaState(anEvent);
        int32_t flags = AKeyEvent_getFlags(anEvent);
        int32_t scancode = AKeyEvent_getScanCode(anEvent);
        int32_t repeatcount = AKeyEvent_getRepeatCount(anEvent);

        vsg::KeySymbol keySymbol, modifiedKeySymbol;
        vsg::KeyModifier keyModifier;
        if (!_keyboard->getKeySymbol(keycode, metastate, keySymbol, modifiedKeySymbol, keyModifier))
            return false;

        switch (action)
        {
        case AKEY_EVENT_ACTION_DOWN:
            _bufferedEvents.emplace_back(new vsg::KeyPressEvent(this, event_time, keySymbol, modifiedKeySymbol, keyModifier));
            break;
        case AKEY_EVENT_ACTION_UP:
            _bufferedEvents.emplace_back(new vsg::KeyReleaseEvent(this, event_time, keySymbol, modifiedKeySymbol, keyModifier));
            break;
            //case AKEY_EVENT_ACTION_MULTIPLE:
            //   _bufferedEvents.emplace_back(new vsg::KeyPressEvent(this, event_time, keySymbol, modifiedKeySymbol, keyModifier);
            //   break;
        default: break;
        }
        return true;
    }
    return false;
}