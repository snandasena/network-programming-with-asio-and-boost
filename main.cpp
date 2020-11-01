#include <iostream>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> vBuffer(1 * 1024);

void GrabSomeData(asio::ip::tcp::socket &socket) {
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
                           [&](std::error_code ec, std::size_t length) {
                               if (!ec) {
                                   std::cout << "\n\nRead " << length << " bytes\n\n";

                                   for (int i = 0; i < length; ++i) {
                                       std::cout << vBuffer[i];
                                   }

                                   GrabSomeData(socket);
                               }
                           });
}

int main() {
    asio::error_code ec;

    // create a "context" - essentially the platform specific interface
    asio::io_context context;

    // give some fake tasks to asio so the context doesn't finish
    asio::io_context::work idlework(context);

    // start the context
    std::thread thrContext = std::thread([&]() { context.run(); });
    // get the address of somewhere we wish to connect to
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

    // create a socket, the context will deliver the implementation
    asio::ip::tcp::socket socket(context);

    // tell socket to try and connect
    socket.connect(endpoint, ec);
    if (!ec) {
        std::cout << "Connected!\n";
    } else {
        std::cout << "Failed to connect to the address:\n" << ec.message() << "\n";
    }

    if (socket.is_open()) {

        GrabSomeData(socket);

        std::string sRequest =
                "GET /index.html HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Connection: close\r\n\r\n";
        socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);
        // bruteforce
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(1000ms);

//        socket.wait(socket.wait_read);
//        size_t bytes = socket.available();
//        std::cout << "Bytes Available: " << bytes << "\n";
//        if (bytes > 0) {
//            std::vector<char> vBuffer(bytes);
//            socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);
//
//            for (auto c: vBuffer) {
//                std::cout << c;
//            }
//        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(20000ms);

        context.stop();
        if (thrContext.joinable()) thrContext.join();
    }
    return 0;
}