#pragma once
#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>
#include <JavaScriptCore/JavaScript.h>
#include <AppCore/Defines.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>
#include <functional>
#include <map>
#include <array>
#include <stdio.h>
#include "Blamath.h"

using namespace std;
using namespace ultralight;

class MyApp : public AppListener,
              public WindowListener,
              public LoadListener,
              public ViewListener {
public:
    MyApp();

    virtual ~MyApp();

    // Start the run loop.
    virtual void Run();

    // This is called continuously from the app's main loop.
    virtual void OnUpdate() override;

    // This is called when the window is closing.
    virtual void OnClose(ultralight::Window* window) override;

    // This is called whenever the window resizes.
    virtual void OnResize(ultralight::Window* window, uint32_t width, uint32_t height) override;

    // This is called when the page finishes a load in one of its frames.
    virtual void OnFinishLoading(ultralight::View* caller,
                                 uint64_t frame_id,
                                 bool is_main_frame,
                                 const String& url) override;

    // This is called when the DOM has loaded in one of its frames.
    virtual void OnDOMReady(ultralight::View* caller,
                            uint64_t frame_id,
                            bool is_main_frame,
                            const String& url) override;

    // This is called when the page requests to change the Cursor.
    virtual void OnChangeCursor(ultralight::View* caller,
                                Cursor cursor) override;

    virtual void OnChangeTitle(ultralight::View* caller,
                               const String& title) override;

    static std::string JSStringToStdString(JSStringRef);

    static JSValueRef OnButtonClick(JSContextRef, JSObjectRef, JSObjectRef, size_t,const JSValueRef[], JSValueRef*);

    static bool IsNumber(const std::string&);
    static void SetText(JSContextRef, string, string);

    // join method
    template <class T>
    std::string static join(T& val, std::string delim)
    {
        std::string str;
        typename T::iterator it;
        const typename T::iterator itlast = val.end()-1;
        for (it = val.begin(); it != val.end(); it++)
        {
            str += *it;
            if (it != itlast)
            {
                str += delim;
            }
        }
        return str;
    }


protected:
    RefPtr<App> app_;
    RefPtr<Window> window_;
    RefPtr<Overlay> overlay_;
};
