#include <iostream>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

int main() {
    asio::error_code ec;

    // create a "context" - essentially the platform specific interface
    asio::io_context context;

    // get the address of somewhere we wish to connect to
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

    // create a socket, the context will deliver the implementation
    asio::ip::tcp::socket socket(context);

    // tell socket to try and connect
    socket.connect(endpoint, ec);
    if (!ec) {
        std::cout << "Connected!\n";
    } else {
        std::cout << "Failed to connect to the address:\n" << ec.message() << "\n";
    }
    return 0;
}