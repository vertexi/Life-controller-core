#include <stdio.h>
#include <string>

#include <action.hpp>
#include <log_eval.hpp>
#include "config.hpp"
#include <common.hpp>

#include <ctime>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "hello_imgui/hello_imgui.h"
#include "imgui_internal.h"
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

struct MyAppSettings
{
    char device_name[256] = "windows";
    bool counting = false;
    std::time_t start_time = 0;
};

#include "asio.hpp"
asio::io_service io_service;

#include <tray.h>

#define TRAY_ICON1 "icon.png"
#define TRAY_ICON2 "icon.ico"
class tray_and_menu {
  public:
    tray_and_menu() {
        // Setup arrays with tray items
        if (tray_.menu != nullptr) {
            return;
        }

        tray_.menu = new tray_menu[8];
        tray_.menu[0] = {.text = (char*)"Hello", .cb = hello_cb, .submenu = nullptr};
        tray_.menu[1] = {.text = (char*)"Checked", .cb = toggle_cb, .submenu = nullptr};
        tray_.menu[2] = {.text = (char*)"Disabled", .disabled = 1, .submenu = nullptr};
        tray_.menu[3] = {.text = (char*)"-", .submenu = nullptr};
        tray_.menu[4] = {.text = (char*)"SubMenu", .submenu = new tray_menu[3]};
        tray_.menu[5] = {.text = (char*)"-", .submenu = nullptr};
        tray_.menu[6] = {.text = (char*)"Quit", .cb = quit_cb, .submenu = nullptr};
        tray_.menu[7] = {.text = nullptr, .submenu = nullptr};

        tray_.menu[4].submenu[0] = {.text = (char*)"FIRST",
                                    .checked = 1,
                                    .cb = submenu_cb,
                                    .submenu = nullptr};
        tray_.menu[4].submenu[1] = {.text = (char*)"SECOND",
                                    .submenu = new tray_menu[6]};
        tray_.menu[4].submenu[2] = {.text = nullptr, .submenu = nullptr};

        tray_.menu[4].submenu[1].submenu[0] = {.text = (char*)"THIRD",
                                               .submenu = new tray_menu[4]};
        tray_.menu[4].submenu[1].submenu[1] = {.text = (char*)"FOUR",
                                               .submenu = new tray_menu[3]};
        tray_.menu[4].submenu[1].submenu[2] = {.text = nullptr,
                                               .submenu = nullptr};

        tray_.menu[4].submenu[1].submenu[0].submenu[0] = {
            .text = (char*)"7", .cb = submenu_cb, .submenu = nullptr};
        tray_.menu[4].submenu[1].submenu[0].submenu[1] = {.text = (char*)"-",
                                                          .submenu = nullptr};
        tray_.menu[4].submenu[1].submenu[0].submenu[2] = {
            .text = (char*)"8", .cb = submenu_cb, .submenu = nullptr};
        tray_.menu[4].submenu[1].submenu[0].submenu[3] = {.text = nullptr,
                                                          .submenu = nullptr};

        tray_.menu[4].submenu[1].submenu[1].submenu[0] = {
            .text = (char*)"5", .cb = submenu_cb, .submenu = nullptr};
        tray_.menu[4].submenu[1].submenu[1].submenu[1] = {
            .text = (char*)"6", .cb = submenu_cb, .submenu = nullptr};
        tray_.menu[4].submenu[1].submenu[1].submenu[2] = {
            .text = nullptr, .cb = submenu_cb, .submenu = nullptr};

        if (tray_init(&tray_) < 0) {
            std::runtime_error("failed to create tray_");
        }
        tray_update(&tray_);
    }

    ~tray_and_menu() {
        delete[] tray_.menu[4].submenu[1].submenu[1].submenu;
        delete[] tray_.menu[4].submenu[1].submenu[0].submenu;
        delete[] tray_.menu[4].submenu[1].submenu;
        delete[] tray_.menu[4].submenu;
        delete[] tray_.menu[3].submenu;
        delete[] tray_.menu->submenu;
        tray_.menu->submenu = nullptr;
    }

    static void run_tray() {
        while (tray_loop(1) == 0) {
            std::cout << "Iteration" << std::endl;
        }
    }

    static int loop_tray()
    {
        return tray_loop(0);
    }

  private:
    static void toggle_cb(struct tray_menu *item) {
        std::cout << "toggle cb" << std::endl;
        item->checked = !item->checked;
        tray_update(&tray_);
    }

    static void hello_cb(struct tray_menu *item) {
        (void)item;
        std::cout << "hello cb" << std::endl;
        // NOLINTNEXTLINE(bugprone-branch-clone)
        if (strcmp(tray_.icon, TRAY_ICON1) == 0) {
            tray_.icon = (char *)TRAY_ICON2;
        } else {
            tray_.icon = (char *)TRAY_ICON1;
        }
        tray_update(&tray_);
    }

    static void quit_cb(struct tray_menu *item) {
        (void)item;
        std::cout << "quit cb" << std::endl;
        tray_exit();
    }

    static void submenu_cb(struct tray_menu *item) {
        (void)item;
        std::cout << "submenu: clicked on " << item->text << std::endl;
        tray_update(&tray_);
    }

  private /* members */:
    // Tray with pointers to menu
    static tray tray_;
};

tray tray_and_menu::tray_{.icon = (char *)TRAY_ICON2, .menu = nullptr};

struct AppState
{
    MyAppSettings myAppSettings;

    ImFont* TitleFont = nullptr;
    ImFont* ColorFont = nullptr;
    ImFont* EmojiFont = nullptr;
    ImFont* LargeIconFont = nullptr;
    ImFont* TimeFont = nullptr;
    ImFont* TimeFontSmall = nullptr;
};

std::string MyAppSettingsToString(const MyAppSettings& myAppSettings)
{
    rapidjson::Document d;
    d.SetObject();
    auto& allocator = d.GetAllocator();
    rapidjson::Value deviceName(myAppSettings.device_name, allocator);
    rapidjson::Value counting(myAppSettings.counting);
    rapidjson::Value startTime((int64_t)myAppSettings.start_time);

    d.AddMember("deviceName", deviceName, allocator);
    d.AddMember("counting", counting, allocator);
    d.AddMember("startTime", startTime, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    return buffer.GetString();
}
MyAppSettings StringToMyAppSettings(const std::string& s)
{
    MyAppSettings myAppSettings;
    rapidjson::Document d;
    d.Parse(s.c_str());
    if (d.IsObject())
    {
        if (d.HasMember("deviceName"))
        {
            strcpy_s(myAppSettings.device_name, d["deviceName"].GetString());
        }
        if (d.HasMember("counting"))
        {
            myAppSettings.counting = d["counting"].GetBool();
        }
        if (d.HasMember("startTime"))
        {
            myAppSettings.start_time = d["startTime"].GetInt64();
        }
    }
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

    appState.TimeFontSmall = HelloImGui::LoadFont("fonts/Roboto/Roboto-BoldItalic.ttf", 30.f);

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

void updateTimer(const asio::error_code& ec, asio::steady_timer* t, std::time_t start_time, char* time_str, size_t bufsize)
{
    if (ec)
    {
        std::cout << "timer error: " << ec.message() << "\n";
        return;
    }
    auto duaration_str = get_duration_str(start_time, std::time(0));
    strcpy(time_str, duaration_str.c_str());
    printf("timer:%s\n", time_str);
    t->expires_at(t->expiry() + asio::chrono::seconds(1));
    t->async_wait(std::bind(updateTimer, std::placeholders::_1, t, start_time, time_str, bufsize));
}

bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label);
}

void TextCenteredOnLine(const char* label, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    ImGui::Text(label);
    return;
}

void AlignForWidth(float width, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

enum SCREEN_STATE { Time_S, Timer_S, Setting_S, Timer_End_S, Event_Edit_S };

void guiFunction(AppState& appState)
{
    static tray_and_menu t;
    if (tray_and_menu::loop_tray() == -1)
    {
        HelloImGui::GetRunnerParams()->appShallExit = true;
    }
    static bool first_time = true;
    static enum SCREEN_STATE current_screen = Time_S;

    static char time_str[20] = "11:31:00";
    if (first_time)
    {
        std::time_t now = std::time(0);
        std::strftime(time_str, 20, "%H:%M:%S", std::localtime(&now));
        static asio::steady_timer t(io_service, asio::chrono::seconds(1));
        t.async_wait(std::bind(updateTime, std::placeholders::_1, &t, time_str, sizeof(time_str)));
    }

    ImGui::NewLine();

    if (ImGui::BeginPopupContextItem("ContextMenu"))
    {
        ImGui::PushFont(appState.TitleFont);
        if (ImGui::Selectable("Settings"))
        {
            current_screen = Setting_S;
        }
        ImGui::PopFont();
        ImGui::EndPopup();
    }

    switch (current_screen)
    {
    case Time_S:
        {
            ImVec2 dragSize(ImGui::GetMainViewport()->Size.x, ImGui::GetFontSize() * 1.5f);
            ImRect dragArea(ImGui::GetMainViewport()->Pos, ImGui::GetMainViewport()->Pos + dragSize);
            ImVec2 mousePos = ImGui::GetMousePos();

            if (dragArea.Contains(mousePos) && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("ContextMenu");
            }
            ImGui::PushFont(appState.TimeFont);
            auto windowWidth = ImGui::GetWindowSize().x;
            static auto textWidth = ImGui::CalcTextSize("00:00:00").x;
            ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
            ImGui::Text(time_str);
            ImGui::PopFont();
            if (ButtonCenteredOnLine("Start"))
            {
                appState.myAppSettings.counting = true;
                current_screen = Timer_S;
                std::time_t current = std::time(0);
                appState.myAppSettings.start_time = current;
                printf("Counting started!\n");
                printf("Start time: %s\n", std::ctime(&current));
            }
        }
        break;
    case Timer_S:
        {
            static bool first_time = true;
            static char timer_str[20] = "00:00:00";
            static asio::steady_timer t(io_service, asio::chrono::seconds(1));
            if (first_time)
            {
                std::time_t start_time = std::time(0);
                t.expires_from_now(asio::chrono::seconds(1));
                t.async_wait(std::bind(updateTimer, std::placeholders::_1, &t, start_time, timer_str, sizeof(timer_str)));
            }
            first_time = false;

            ImGui::PushFont(appState.TimeFontSmall);
            auto windowWidth = ImGui::GetWindowSize().x;
            static auto textWidthSmall = ImGui::CalcTextSize("00:00:00").x;
            ImGui::SetCursorPosX((windowWidth - textWidthSmall) * 0.5f);
            ImGui::Text(time_str);
            ImGui::PopFont();
            ImGui::PushFont(appState.TimeFont);
            static auto textWidth = ImGui::CalcTextSize("00:00:00").x;
            ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
            ImGui::Text(timer_str);
            ImGui::PopFont();
            if (ButtonCenteredOnLine("Stop"))
            {
                strcpy(timer_str, "00:00:00");
                appState.myAppSettings.counting = false;
                current_screen = Timer_End_S;
                printf("Counting stopped!\n");
                printf("Start time: %s\n", std::ctime(&appState.myAppSettings.start_time));
                t.cancel();
                first_time = true;
            }
        }
        break;
    case Setting_S:
        {
            static bool first_time = true;
            ImGui::PushFont(appState.TitleFont);
            static char device_name[256] = "";
            if (first_time)
            {
                strcpy(device_name, appState.myAppSettings.device_name);
            }
            first_time = false;
            ImGui::Text("device name");
            ImGui::SameLine();
            ImGui::InputText("##", device_name, sizeof(device_name));

            ImGuiStyle& style = ImGui::GetStyle();
            float width = 0.0f;
            width += HelloImGui::EmSize(5.0f);
            width += style.ItemSpacing.x;
            width += HelloImGui::EmSize(5.0f);
            width += style.ItemSpacing.x;
            width += HelloImGui::EmSize(5.0f);
            AlignForWidth(width);

            if (ImGui::Button("OK", HelloImGui::EmToVec2(5.0f, 0.0f)))
            {
                strcpy(appState.myAppSettings.device_name, device_name);
                current_screen = Time_S;
                first_time = true;
            }
            ImGui::SameLine(0, HelloImGui::EmSize(5.0f));
            if (ImGui::Button("CANCEL", HelloImGui::EmToVec2(5.0f, 0.0f)))
            {
                current_screen = Time_S;
                first_time = true;
            }
            ImGui::PopFont();
        }
        break;
    case Timer_End_S:
        {
            ImGui::PushFont(appState.TitleFont);
            TextCenteredOnLine("Timer ended!");

            ImGuiStyle& style = ImGui::GetStyle();
            float width = 0.0f;
            width += HelloImGui::EmSize(5.0f);
            width += style.ItemSpacing.x;
            width += HelloImGui::EmSize(5.0f);
            width += style.ItemSpacing.x;
            width += HelloImGui::EmSize(5.0f);
            AlignForWidth(width);

            if (ImGui::Button("STORE", HelloImGui::EmToVec2(5.0f, 0.0f)))
            {
                current_screen = Time_S;
                first_time = true;
            }
            ImGui::SameLine(0, HelloImGui::EmSize(5.0f));
            if (ImGui::Button("CANCEL", HelloImGui::EmToVec2(5.0f, 0.0f)))
            {
                current_screen = Time_S;
                first_time = true;
            }

            ImGui::PopFont();
        }
        break;
    default:
        break;
    }

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
