#include <zmq.hpp>
#include <iostream>
#include <sstream>

#ifdef WIN32
#include <Windows.h>
#endif

int main (int argc, char *argv[])
{
#ifdef WIN32
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif
    zmq::context_t context (1);

    //  Socket to talk to server
    std::cout << "Collecting updates from python server...\n" << std::endl;
    zmq::socket_t subscriber (context, zmq::socket_type::sub);
    subscriber.connect("ws://localhost:8848");

    while (true)
    {
        //  Subscribe to zipcode, default is NYC, 10001
        const char *filter = (argc > 1)? argv [1]: "10001 ";
        subscriber.set(zmq::sockopt::subscribe, "");

        zmq::message_t update;

        std::string activeWindowTitle;

        subscriber.recv(update, zmq::recv_flags::none);
        activeWindowTitle = update.to_string();
        std::cout << activeWindowTitle << std::endl;
    }

    return 0;
}
