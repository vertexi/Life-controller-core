#include "hello_imgui/hello_imgui.h"
#include "clip.h"
#include "hello_imgui/hello_imgui_font.h"
#include "hello_imgui/image_from_asset.h"
#include "hello_imgui/internal/image_abstract.h"
#include "imgui.h"
#include <iostream>
#include "stb_image_write.h"
#include "LXGWWenKaiMonoLite.hpp"

#ifdef HELLOIMGUI_HAS_OPENGL
#include "hello_imgui/hello_imgui_include_opengl.h"
#endif

#include <zmq.hpp>

// Demonstrate how to load additional fonts (fonts - part 1/3)
ImFont * gCustomFont = nullptr;
ImFont * chineseFont = nullptr;
void MyLoadFonts()
{

    HelloImGui::ImGuiDefaultSettings::LoadDefaultFont_WithFontAwesomeIcons(); // The font that is loaded first is the default font
    gCustomFont = HelloImGui::LoadFont("fonts/Akronim-Regular.ttf", 40.f); // will be loaded from the assets folder

    HelloImGui::FontLoadingParams fontLoadingParamsChinese;
    fontLoadingParamsChinese.mergeFontAwesome = true;
    fontLoadingParamsChinese.useFullGlyphRange = true;
    chineseFont = HelloImGui::LoadFont((void*)get_LXGWWenKaiMonoLite_Regular(),
                                                get_LXGWWenKaiMonoLite_RegularItalic_size(),
                                                30.f,
                                                fontLoadingParamsChinese);
}


#include <zmq.hpp>
#include <string>
#include <iostream>

void png_copy(void *socket_, void *data, int size) {
    zmq::message_t request (size);
    memcpy(request.data(), data, size);
    std::cout << "Sending Image " << std::endl;
    zmq::socket_t *socket = (zmq::socket_t *)socket_;
    socket->send (request, zmq::send_flags::none);
}

int main(int , char *[]) {
    HelloImGui::RunnerParams params;
    params.appWindowParams.windowGeometry.size = {1280, 720};
    params.appWindowParams.windowTitle = "Dear ImGui example with 'Hello ImGui'";
    params.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::NoDefaultWindow;

    // Fonts need to be loaded at the appropriate moment during initialization (fonts - part 2/3)
    params.callbacks.LoadAdditionalFonts = MyLoadFonts; // LoadAdditionalFonts is a callback that we set with our own font loading function

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, zmq::socket_type::req);

    std::cout << "Connecting to hello world server..." << std::endl;
    socket.connect ("tcp://localhost:8848");

    zmq_pollitem_t items[] = {
        {socket,  0,  ZMQ_POLLIN,     0},
    };

    params.callbacks.AfterSwap = [&]() {
        zmq_poll(items, IM_ARRAYSIZE(items), 0);
    };

    params.callbacks.ShowGui = [&]() {
        {
            ImGui::Begin("clip test");
            ImGui::Text("Hello, world!");

            ImGui::PushFont(chineseFont);
            ImGui::Text((char *)u8"你好，世界！");
            static std::string value;
            static clip::image image;
            static bool update_image = false;
            static bool wait_reply = false;
            if (ImGui::Button("Show clipboard"))
            {
                if (!clip::get_text(value))
                {
                    value = "Could not get clipboard text";
                    if (!clip::get_image(image)) {
                        value = "Could not get clipboard image";
                    } else {
                        update_image = true;
                        wait_reply = true;
                        auto imageDataSize = image.spec().required_data_size();
                        auto imageData = image.data();
                        IM_ASSERT(image.spec().bits_per_pixel / 8 == 4);
                        for (int i = 0; i < imageDataSize / 4; i++)
                        {
                            std::swap(imageData[i * 4 + 0], imageData[i * 4 + 2]);
                        }
                        stbi_write_png_to_func(png_copy, &socket, image.spec().width, image.spec().height, 4, (void *)imageData, image.spec().bytes_per_row);
                    }
                }
            }
            ImGui::Text("%s", value.c_str());
            static HelloImGui::ImageAbstractPtr concreteImage;
            static ImVec2 imageDispSize;
            if (update_image) {
#ifdef HELLOIMGUI_HAS_OPENGL
                std::tie(concreteImage, imageDispSize) = HelloImGui::ImageFromData((unsigned char *)image.data(), {(float)image.spec().width, (float)image.spec().height});
#endif
                update_image = false;
            }
            if (wait_reply && items[0].revents & ZMQ_POLLIN) {
                zmq::message_t reply;
                auto size = socket.recv (reply, zmq::recv_flags::none);
                if(size != -1)
                {
                    std::cout << "receive ok." << std::endl;
                    std::cout << reply.to_string() << std::endl;
                }
                wait_reply = false;
            }
            if (imageDispSize.x != 0)
            {
                ImGui::Image(concreteImage->TextureID(), imageDispSize);
            }
            if (ImGui::Button("Set clipboard"))
            {
                clip::set_text("Hello clipboard!");
            }
            ImGui::PopFont();
            ImGui::End();
        }

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
    };

    params.imGuiWindowParams.showMenuBar = true;
    params.imGuiWindowParams.showMenu_App = false;

    params.appWindowParams.borderless = true;
    params.appWindowParams.borderlessMovable = true;
    params.appWindowParams.borderlessResizable = true;
    params.appWindowParams.borderlessClosable = true;
    params.appWindowParams.borderlessHideable = true;
    params.fpsIdling.enableIdling = false;

    HelloImGui::Run(params);
    return 0;
}
