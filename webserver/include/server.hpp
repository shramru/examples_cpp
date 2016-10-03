//
// Created by vladislav on 07.09.16.
//

#ifndef WEBSERVER_SERVER_HPP
#define WEBSERVER_SERVER_HPP

#include <thread>
#include <set>
#include "connection.hpp"
#include "threadpool.hpp"

class Server {
    using bait = boost::asio::ip::tcp;

    RequestHandler requestHandler;
    ThreadPool threadPool;
    std::mutex disconnect_mutex;

    boost::asio::io_service ioService;
    bait::acceptor tcpAcceptor;

    std::set<ConnectionPtr> connectedClients;

public:
    void listen();
    void stop();

    Server(const std::string& address, const std::string & port, const std::string& directory,
           size_t workersMin, size_t workersMax);
    ~Server();
};


#endif //WEBSERVER_SERVER_HPP
