#pragma once
#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>
#include <JavaScriptCore/JavaScript.h>
#include <AppCore/Defines.h>

#include <vector>
#include <string>
#include <functional>
#include <map>

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

    virtual std::string JSStringToStdString(JSStringRef);

    JSValueRef OnButtonClick(JSContextRef, JSObjectRef, JSObjectRef, size_t,const JSValueRef[], JSValueRef*);

    virtual std::string GetRenderTitleJS(string);
    virtual std::string GetRenderSubtitleJS(string);
    // join method
    template <class T>
    std::string join(T& val, std::string delim)
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

    vector<string> input;
    vector<string> result;
    bool isInput = true;
protected:
    RefPtr<App> app_;
    RefPtr<Window> window_;
    RefPtr<Overlay> overlay_;
};
