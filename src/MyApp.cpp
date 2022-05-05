#include <array>
#include "MyApp.h"

#define WINDOW_WIDTH  660
#define WINDOW_HEIGHT 820

vector<string> input;
std::string result;
bool isEdit = true;

MyApp::MyApp() {
    ///
    /// Create our main App instance.
    ///
    app_ = App::Create();
    ///
    /// Create a resizable window by passing by OR'ing our window flags with
    /// kWindowFlags_Resizable.
    ///
    window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
                             false, kWindowFlags_Titled | kWindowFlags_Resizable);

    ///
    /// Create our HTML overlay-- we don't care about its initial size and
    /// position because it'll be calculated when we call OnResize() below.
    ///
    overlay_ = Overlay::Create(window_, 1, 1, 0, 0);

    ///
    /// Force a call to OnResize to perform size/layout of our overlay.
    ///
    OnResize(window_.get(), window_->width(), window_->height());

    ///
    /// Load a page into our overlay's View
    ///
    overlay_->view()->LoadURL("file:///app.html");

    ///
    /// Register our MyApp instance as an AppListener so we can handle the
    /// App's OnUpdate event below.
    ///
    app_->set_listener(this);

    ///
    /// Register our MyApp instance as a WindowListener so we can handle the
    /// Window's OnResize event below.
    ///
    window_->set_listener(this);

    ///
    /// Register our MyApp instance as a LoadListener so we can handle the
    /// View's OnFinishLoading and OnDOMReady events below.
    ///
    overlay_->view()->set_load_listener(this);

    ///
    /// Register our MyApp instance as a ViewListener so we can handle the
    /// View's OnChangeCursor and OnChangeTitle events below.
    ///
    overlay_->view()->set_view_listener(this);
}

MyApp::~MyApp() {
}

void MyApp::Run() {
    app_->Run();
}

void MyApp::OnUpdate() {
    ///
    /// This is called repeatedly from the application's update loop.
    ///
    /// You should update any app logic here.
    ///
}

void MyApp::OnClose(ultralight::Window* window) {
    app_->Quit();
}

void MyApp::OnResize(ultralight::Window* window, uint32_t width, uint32_t height) {
    ///
    /// This is called whenever the window changes size (values in pixels).
    ///
    /// We resize our overlay here to take up the entire window.
    ///
    overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View* caller,
                            uint64_t frame_id,
                            bool is_main_frame,
                            const String& url) {
    ///
    /// This is called when a frame finishes loading on the page.
    ///
}
bool MyApp::IsNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isdigit(*it) || *it == '-' || *it == '(' || *it == '.')) ++it;
    return !s.empty() && it == s.end();
}

std::string MyApp::JSStringToStdString(JSStringRef jsString) {
    size_t maxBufferSize = JSStringGetMaximumUTF8CStringSize(jsString);
    char* utf8Buffer = new char[maxBufferSize];
    size_t bytesWritten = JSStringGetUTF8CString(jsString, utf8Buffer, maxBufferSize);
    std::string utf_string = std::string (utf8Buffer, bytesWritten -1); // the last byte is a null \0 which std::string doesn't need.
    delete [] utf8Buffer;
    return utf_string;
}

void MyApp::SetText(JSContextRef ctx, string selector, string content) {
    string runScript = "document.querySelector('" + selector + "').innerText = '" + content + "';";
    // Create our string of JavaScript
    JSStringRef script = JSStringCreateWithUTF8CString(runScript.c_str());

    // Execute it with JSEvaluateScript, ignoring other parameters for now
    JSEvaluateScript(ctx, script, 0, 0, 0, 0);

    // Release our string (we only Release what we Create)
    JSStringRelease(script);
}

// This callback will be bound to 'OnButtonClick()' on the page.
JSValueRef MyApp::OnButtonClick(JSContextRef ctx, JSObjectRef function,
                                JSObjectRef thisObject, size_t argumentCount,
                                const JSValueRef arguments[], JSValueRef* exception) {

    for (int index = 0; index < argumentCount; index ++) {
        JSStringRef thisArg = JSValueToStringCopy(ctx,arguments[index],NULL);
        string thisArgStr = JSStringToStdString(thisArg);
        if (thisArgStr == "reset") {
            isEdit = true;
            result = "";
            input.clear();
        }else if(thisArgStr == "-") {
            if (input.size() > 0) {
                if (IsNumber(input[input.size() - 1])) {
                    input[input.size() - 1] = to_string((stold(input[input.size() - 1]) * -1));
                }
            }
        }else if (thisArgStr == "=" && isEdit) {
            isEdit = false;
        }else if (thisArgStr == "<") {
            isEdit = true;
            result = "";
            if (input.size() >= 1) {
                input.pop_back();
            }
        }else if (thisArgStr == "! ") {
            if (input.size() >= 1) {
                if (IsNumber(input[input.size() - 1])) {
                    input.push_back(thisArgStr);
                }
            }
        }else if (thisArgStr == "edit") {
            isEdit = true;
            result = "";
        }else {
            isEdit = true;
            if (input.size() >= 1) {
                if (thisArgStr[0] != ' ' && IsNumber(thisArgStr) && IsNumber(input[input.size() - 1])) {
                    input[input.size() - 1] += thisArgStr;
                }else {
                    input.push_back(thisArgStr);
                }
            }else {
                input.push_back(thisArgStr);
            }
        }
    }
    if (isEdit) {
        if (input.size() <= 0) {
            SetText(ctx, ".main-number", "0");
        }else {
            SetText(ctx, ".main-number", join(input, ""));
        }
        SetText(ctx, ".sub-number", result);
    }else {
        // TODO: call Blamath program & fetch result
//        std::string command("whoami");
//        array<char, 128> buffer;
//
//        FILE* pipe = popen(command.c_str(), "r");
//        if (!pipe)
//        {
//            return 0;
//        }
//        while (fgets(buffer.data(), 128, pipe) != NULL) {
//            result += buffer.data();
//            SetText(ctx, ".main-number", result);
//        }
//        cout << result << endl;
////        result = "1231";
//        // replace
//        for (int index = 0; index < result.size(); index ++) {
//            if (result[index] == '\n') {
//                result[index] = ' ';
//            }
//        }
        string task = join(input, "");

        Blamath bla(task);
        SetText(ctx, ".sub-number", join(input, ""));
        SetText(ctx, ".main-number", bla.getValue());
    }
    return JSValueMakeNull(ctx);
}

void MyApp::OnDOMReady(ultralight::View* caller,
                       uint64_t frame_id,
                       bool is_main_frame,
                       const String& url) {
    ///
    /// This is called when a frame's DOM has finished loading on the page.
    ///
    /// This is the best time to setup any JavaScript bindings.
    ///

    // Acquire the JS execution context for the current page.
    //
    // This call will lock the execution context for the current
    // thread as long as the Ref<> is alive.
    //
    RefPtr<JSContext> context = caller->LockJSContext();

    // Get the underlying JSContextRef for use with the
    // JavaScriptCore C API.
    JSContextRef ctx = *context.get();

    // init display
    input.clear();
    result = "";
    SetText(ctx, ".main-number", "0");
    SetText(ctx, ".sub-number", "");

    // Create a JavaScript String containing the name of our callback.
    JSStringRef name = JSStringCreateWithUTF8CString("OnButtonClick");

    // Create a garbage-collected JavaScript function that is bound to our
    // native C callback 'OnButtonClick()'.
    JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, OnButtonClick);

    // Get the global JavaScript object (aka 'window')
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // Store our function in the page's global JavaScript object so that it
    // accessible from the page as 'OnButtonClick()'.
    JSObjectSetProperty(ctx, globalObj, name, func, 0, 0);

    // Release the JavaScript String we created earlier.
    JSStringRelease(name);
}

void MyApp::OnChangeCursor(ultralight::View* caller,
                           Cursor cursor) {
    ///
    /// This is called whenever the page requests to change the cursor.
    ///
    /// We update the main window's cursor here.
    ///
    window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View* caller,
                          const String& title) {
    ///
    /// This is called whenever the page requests to change the title.
    ///
    /// We update the main window's title here.
    ///
    window_->SetTitle(title.utf8().data());
}