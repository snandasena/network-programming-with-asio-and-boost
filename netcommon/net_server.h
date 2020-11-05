#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace olc {

    namespace net {
        template<typename T>
        class server_interface {
        public:
            server_interface(uint16_t port) : m_asioAcceptor(m_asioContext,
                                                             asio::ip::tcp::endpoint(asio::tp::tcp::v4(), port)) {

            }

            virtual ~server_interface() {
                Stop();
            }

            bool Start() {
                try {
                    WaitForClientConnection();
                    m_threadContext = std::thread([this]() { m_asioContext.run(); });
                } catch (std::exception &e) {
                    std::cerr << "[SERVER] Exception: " << e.what() << "\n";
                    return false;
                }
                std::cout << "[SERVER] Started\n";
                return true;

            }

            bool Stop() {
                // request the context to close
                m_asioContext.stop();

                // tidy up the context thread
                if (m_threadContext.joinable()) m_threadContext.join();

                // inform someone, anybody, if they care...
                std::cout << "[SERVER] Stopped!\n";
            }

            // ASYNC - instruct asio wait for connection
            void WaitForClientConnection() {
                m_asioAcceptor.async_accept(
                        [this](std::error_code ec, asio::ip::tcp::socket socket) {
                            if (!ec) {
                                std::cout << "[SERVER] New connection: " << socket.remote_endpoint() << "\n";
                                std::shared_ptr<connection<T>> newConn =
                                        std::make_shared<connection<T>>(connection<T>::owner::server, m_asioContext,
                                                                        std::move(socket), m_qMessagesIn);

                                // give the user server a chance to deny connection
                                if (OnClientConnect(newConn)) {
                                    m_deqConnections.emplace_back(std::move(newConn));
                                    m_deqConnections.back()->ConnectToClient(nIDCounter++);

                                    std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";

                                } else {
                                    std::cout << "[-----] Connection denied\n";
                                }
                            } else {
                                std::cout << "[SERVER] New connection error: " << ec.message() << "\n";
                            }

                            WaitForClientConnection();
                        }
                );
            }

            // send a message to a specific client
            void MessageClient(std::shared_ptr<connection<T>> client, const message<T> &msg) {

            }

            // send message to all clients
            void MessageAllClients(const message<T> &msg, std::shared_ptr<connection<T>> pIgnoreClient) {

            }

        protected:
            // called when a client connects, you can veto the connection by returning false
            virtual bool OnClientConnect(std::shared_ptr<connection<T>> client) {
                return false;
            }

            // called when a client appears to have disconnected
            virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client) {

            }

            // called when a message arrives
            virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T> &msg) {

            }

        protected:
            // thread safe queue for incoming message packets
            tsqueue<owned_message<T>> m_qMessagesIn;

            // container of active validated connections
            std::deque<std::shared_ptr<connection<T>>> m_deqConnections;

            // order of declaration is important - it is also the order of initialisation
            asio::io_context m_asioContext;
            std::thread m_threadContext;

            // these things need an context
            asio::ip::tcp::acceptor m_asioAcceptor;

            // clients will be identified in the "wider system" via an ID
            uint32_t nIDCounter = 10000;

        };
    }
}