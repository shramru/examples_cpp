//
// Created by vladislav on 07.09.16.
//

#include "connection.hpp"

Connection::Connection(socketPtr tcpSocket, const RequestHandler& requestHandler,
                       std::function<void (ConnectionPtr)> abortedCallback)
        : requestHandler(requestHandler), tcpSocket(std::move(tcpSocket)),
          abortedCallback(abortedCallback), buffer(BUFFER_SIZE), messageSize(0) {}

void Connection::read() {
    for (;;) {
        boost::system::error_code ec;
        size_t bytesRead = tcpSocket->read_some(boost::asio::buffer(buffer), ec);

        if (ec) {
            if (ec != boost::asio::error::eof)
                std::cout << "Connection ERROR: " << ec.message() << std::endl;

            close();
            return;
        }

        if (messageSize == 0) {
            messageSize = tcpSocket->available();
            message.clear();
            message.insert(message.end(), buffer.begin(), buffer.begin() + bytesRead);
        } else {
            messageSize -= bytesRead;
        }

        if (messageSize == 0) {
            requestHandler.handle_request(std::string(message.begin(), message.end()),
                                          std::bind(&Connection::write, this, std::placeholders::_1));
            close();
            return;
        }
    }
}

void Connection::write(const std::string& message) {
    tcpSocket->write_some(boost::asio::buffer(message));
}

void Connection::close() {
    if (tcpSocket->is_open()) tcpSocket->close();
    abortedCallback(shared_from_this());
}