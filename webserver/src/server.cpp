//
// Created by vladislav on 07.09.16.
//

#include "server.hpp"

Server::Server(const std::string &address, const std::string & port, const std::string &directory,
               size_t workersMin, size_t workersMax)
        : requestHandler(directory), threadPool(workersMin, workersMax), tcpAcceptor(ioService)  {
    bait::resolver resolver(ioService);
    bait::endpoint endpoint = *resolver.resolve({address, port});
    tcpAcceptor.open(endpoint.protocol());
    tcpAcceptor.set_option(bait::acceptor::reuse_address(true));
    tcpAcceptor.bind(endpoint);

    std::cout << "Highload WebServer successfully started" << std::endl;
    tcpAcceptor.listen();
}

Server::~Server() {
    stop();
}

void Server::listen() {
    for (;;) {
        socketPtr tcpSocket = std::make_unique<bait::socket>(ioService);
        boost::system::error_code ec;
        tcpAcceptor.accept(*tcpSocket, ec);

        if (ec) {
            if (ec.value() != boost::asio::error::interrupted)
                std::cout << "Acceptor ERROR: " << ec.message() << std::endl;
            return;
        }

        ConnectionPtr connection = std::make_shared<Connection>(std::move(tcpSocket), requestHandler,
                                                                [this](ConnectionPtr connectionPtr) {
                                                                    std::unique_lock<std::mutex> lock(disconnect_mutex);
                                                                    connectedClients.erase(connectionPtr);
                                                                });
        {
            std::unique_lock<std::mutex> lock(disconnect_mutex);
            connectedClients.insert(connection);
        }

        threadPool.enqueue(std::bind(&Connection::read, &*connection));
    }
}

void Server::stop() {
    std::unique_lock<std::mutex> lock(disconnect_mutex);
    tcpAcceptor.close();
    for (auto& cli: connectedClients)
        cli->close();
}
