#include <stdio.h>
#include <string>

#include <action.hpp>
#include <log_eval.hpp>
#include "config.hpp"

#include <ctime>
#include "hello_imgui/hello_imgui.h"
#include <sstream>

struct MyAppSettings
{
    char device_name[256] = "windows";
    bool counting = false;
};

#include "asio.hpp"
asio::io_service io_service;
struct AppState
{
    MyAppSettings myAppSettings;

    ImFont* TitleFont = nullptr;
    ImFont* ColorFont = nullptr;
    ImFont* EmojiFont = nullptr;
    ImFont* LargeIconFont = nullptr;
    ImFont* TimeFont = nullptr;
};

std::string MyAppSettingsToString(const MyAppSettings& myAppSettings)
{
    std::stringstream ss;
    ss << myAppSettings.device_name << "\n";
    ss << myAppSettings.counting << "\n";
    return ss.str();
}
MyAppSettings StringToMyAppSettings(const std::string& s)
{
    std::stringstream ss(s);
    MyAppSettings myAppSettings;
    ss >> myAppSettings.device_name;
    ss >> myAppSettings.counting;
    return myAppSettings;
}

// Note: LoadUserSettings() and SaveUserSettings() will be called in the callbacks `PostInit` and `BeforeExit`:
//     runnerParams.callbacks.PostInit = [&appState]   { LoadMyAppSettings(appState);};
//     runnerParams.callbacks.BeforeExit = [&appState] { SaveMyAppSettings(appState);};
void LoadMyAppSettings(AppState& appState) //
{
    appState.myAppSettings = StringToMyAppSettings(HelloImGui::LoadUserPref("MyAppSettings"));
}
void SaveMyAppSettings(const AppState& appState)
{
    HelloImGui::SaveUserPref("MyAppSettings", MyAppSettingsToString(appState.myAppSettings));
}

void LoadFonts(AppState& appState) // This is called by runnerParams.callbacks.LoadAdditionalFonts
{
    // First, load the default font (the default font should be loaded first)
    HelloImGui::ImGuiDefaultSettings::LoadDefaultFont_WithFontAwesomeIcons();
    // Then load the other fonts
    appState.TitleFont = HelloImGui::LoadFont("fonts/DroidSans.ttf", 18.f);

    appState.TimeFont = HelloImGui::LoadFont("fonts/Roboto/Roboto-BoldItalic.ttf", 80.f);

    HelloImGui::FontLoadingParams fontLoadingParamsEmoji;
    fontLoadingParamsEmoji.useFullGlyphRange = true;
    appState.EmojiFont = HelloImGui::LoadFont("fonts/NotoEmoji-Regular.ttf", 24.f, fontLoadingParamsEmoji);

    HelloImGui::FontLoadingParams fontLoadingParamsLargeIcon;
    fontLoadingParamsLargeIcon.useFullGlyphRange = true;
    appState.LargeIconFont = HelloImGui::LoadFont("fonts/fontawesome-webfont.ttf", 24.f, fontLoadingParamsLargeIcon);
#ifdef IMGUI_ENABLE_FREETYPE
    // Found at https://www.colorfonts.wtf/
    HelloImGui::FontLoadingParams fontLoadingParamsColor;
    fontLoadingParamsColor.loadColor = true;
    appState.ColorFont = HelloImGui::LoadFont("fonts/Playbox/Playbox-FREE.otf", 24.f, fontLoadingParamsColor);
#endif
}

void updateTime(const asio::error_code& ec, asio::steady_timer* t, char* time_str, size_t bufsize)
{
    if (ec)
    {
        std::cout << "timer error: " << ec.message() << "\n";
        return;
    }
    std::time_t now = std::time(0);
    std::strftime(time_str, 20, "%H:%M:%S", std::localtime(&now));
    t->expires_at(t->expiry() + asio::chrono::seconds(1));
    t->async_wait(std::bind(updateTime, std::placeholders::_1, t, time_str, bufsize));
}

void guiFunction(AppState& appState)
{
    static bool first_time = true;
    ImGui::PushFont(appState.TitleFont);
    auto device_name = appState.myAppSettings.device_name;
    ImGui::Text("device name");
    ImGui::SameLine();
    ImGui::InputText("##", device_name, sizeof(device_name));
    ImGui::PopFont();

    static char time_str[20] = "11:31:00";
    if (first_time)
    {
        std::time_t now = std::time(0);
        std::strftime(time_str, 20, "%H:%M:%S", std::localtime(&now));
        static asio::steady_timer t(io_service, asio::chrono::seconds(1));
        t.async_wait(std::bind(updateTime, std::placeholders::_1, &t, time_str, sizeof(time_str)));
    }
    ImGui::PushFont(appState.TimeFont);
    auto windowWidth = ImGui::GetWindowSize().x;
    static auto textWidth = ImGui::CalcTextSize("00:00:00").x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(time_str);
    ImGui::PopFont();

    first_time = false;
}

void AppPoll()
{
    io_service.poll();
}

int main(int , char *[]) {
    AppState appState;

    std::time_t start = std::time(0);
    std::time_t end = start + 10 * 60 * 60;
    create_event_log("test", "test event");
    create_goal_log("test", true, 10, start, end);
    create_goal_log("test", true, 99999, start, end);
    create_goal_log("test", false, 9999999, start, end);
    do_event_log(start, end, "test", "test");
    eval_log();

    HelloImGui::RunnerParams runnerParams;
    runnerParams.appWindowParams.windowTitle = "Life-controller";
    runnerParams.imGuiWindowParams.menuAppTitle = "Docking Demo";
    runnerParams.appWindowParams.windowGeometry.size = {1200, 1000};
    runnerParams.appWindowParams.restorePreviousGeometry = true;

    runnerParams.appWindowParams.borderless = true;
    runnerParams.appWindowParams.borderlessMovable = true;
    runnerParams.appWindowParams.borderlessResizable = true;
    runnerParams.appWindowParams.borderlessClosable = true;

    // Load additional font
    runnerParams.callbacks.LoadAdditionalFonts = [&appState]() { LoadFonts(appState); };

    //
    // Load user settings at `PostInit` and save them at `BeforeExit`
    //
    runnerParams.callbacks.PostInit = [&appState]   { LoadMyAppSettings(appState);};
    runnerParams.callbacks.BeforeExit = [&appState] { SaveMyAppSettings(appState);};
    runnerParams.callbacks.AfterSwap = AppPoll;

    //
    // Change style
    //
    // 1. Change theme
    auto& tweakedTheme = runnerParams.imGuiWindowParams.tweakedTheme;
    tweakedTheme.Theme = ImGuiTheme::ImGuiTheme_MaterialFlat;
    tweakedTheme.Tweaks.Rounding = 10.f;
    // 2. Customize ImGui style at startup
    runnerParams.callbacks.SetupImGuiStyle = []() {
        // Reduce spacing between items ((8, 4) by default)
        ImGui::GetStyle().ItemSpacing = ImVec2(6.f, 4.f);
    };

    runnerParams.callbacks.ShowGui =  [&] { guiFunction(appState); };

    runnerParams.iniFolderType = HelloImGui::IniFolderType::AppExecutableFolder;


    // HelloImGui::DeleteIniSettings(runnerParams);

    // Optional: choose the backend combination
    // ----------------------------------------
    //runnerParams.platformBackendType = HelloImGui::PlatformBackendType::Sdl;
    // runnerParams.rendererBackendType = HelloImGui::RendererBackendType::Vulkan;

    HelloImGui::Run(runnerParams); // Note: with ImGuiBundle, it is also possible to use ImmApp::Run(...)

    return 0;
}
