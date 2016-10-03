//
// Created by vladislav on 07.09.16.
//

#include <fstream>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sys/stat.h>
#include "request_handler.hpp"

RequestHandler::RequestHandler(const std::string &dir)
        : root_dir(dir) {}

void RequestHandler::handle_request(const std::string &request, std::function<void (const std::string&)> writeCallback) const {
    std::istringstream iss(request);
    std::string method, url, protocol = "HTTP/1.1";
    iss >> method >> url >> protocol;
    url = url_decode(url);
    size_t pos;

    while ((pos = url.find("/../")) != std::string::npos) {
        url.erase(pos, 4);
    }

    if ((pos = url.find('?')) != std::string::npos) {
        url = url.substr(0, pos);
    }

    if (method == "GET") {
        GET(url, protocol, writeCallback);
    } else if (method == "HEAD") {
        HEAD(url, protocol, writeCallback);
    } else if (method == "POST" || method == "PUT" || method == "PATCH" ||
               method == "DELETE" || method == "TRACE" || method == "CONNECT" ||
               method == "OPTIONS") {
        NotAllowed(protocol, writeCallback);
    } else {
        NotImplemented(protocol, writeCallback);
    }
}

void RequestHandler::GET(const std::string &url, const std::string &protocol,
                         std::function<void (const std::string&)> writeCallback) const {
    std::string path = root_dir + url;
    int code;
    bool dir;
    if ((dir = is_directory(path))) {
        if (path.back() != '/') path += '/';
        path += "index.html";
    }

    std::string headers(message_headers());

    if (file_exists(path)) {
        code = 200;
        std::string ext = dir ? "html" : get_extension(url);
        headers += file_headers(file_size(path), ext);
        writeCallback(build_headers(protocol, get_code(code), headers));
        std::ifstream in(path);

        char buffer[BUFFER_SIZE];
        while (size_t count = (size_t)in.readsome(buffer, BUFFER_SIZE)) {
            writeCallback(std::string(buffer, count));
        }
    } else {
        code = (dir) ? 403 : 404;
        headers += file_headers(strlen((dir) ? forbidden : not_found), "html");
        writeCallback(build_headers(protocol, get_code(code), headers));
        writeCallback((dir) ? forbidden : not_found);
    }
}

void RequestHandler::HEAD(const std::string &url, const std::string &protocol,
                          std::function<void(const std::string &)> writeCallback) const {
    std::string path = root_dir + url;
    int code;
    bool dir;
    if ((dir = is_directory(path))) {
        if (path.back() != '/') path += '/';
        path += "index.html";
    }

    std::string headers(message_headers());
    if (file_exists(path)) {
        code = 200;
        std::string ext = dir ? "html" : get_extension(url);
        headers += file_headers(file_size(path), ext);
        writeCallback(build_headers(protocol, get_code(code), headers));
    } else {
        code = (dir) ? 403 : 404;
        headers += file_headers(strlen((dir) ? forbidden : not_found), "html");
        writeCallback(build_headers(protocol, get_code(code), headers));
        writeCallback((dir) ? forbidden : not_found);
    }
}

void RequestHandler::NotAllowed(const std::string &protocol,
                                std::function<void(const std::string &)> writeCallback) const {
    const int code = 405;
    std::string headers(message_headers());
    headers += file_headers(strlen(not_allowed), "html");
    writeCallback(build_headers(protocol, get_code(code), headers));
    writeCallback(not_allowed);
}

void RequestHandler::NotImplemented(const std::string &protocol,
                                std::function<void(const std::string &)> writeCallback) const {
    const int code = 501;
    std::string headers(message_headers());
    headers += file_headers(strlen(not_implemented), "html");
    writeCallback(build_headers(protocol, get_code(code), headers));
    writeCallback(not_implemented);
}

std::string RequestHandler::url_decode(const std::string &url) const {
    std::string res;

    for (size_t i = 0; i < url.length(); ++i) {
        if (url[i] == '%') {
            int val;
            sscanf(url.substr(i + 1, 2).c_str(), "%x", &val);
            res += (char)val;
            i += 2;
        } else if (url[i] == '+') {
            res += ' ';
        } else {
            res += url[i];
        }
    }
    return res;
}

bool RequestHandler::is_directory(const std::string& path) const {
    struct stat s;
    return (stat(path.c_str(), &s) == 0) && (s.st_mode & S_IFDIR);
}

std::string RequestHandler::message_headers() const {
    std::ostringstream headers;
    headers.imbue(std::locale(headers.getloc(), new boost::posix_time::time_facet("%a, %d %b %Y %H:%M:%S GMT")));
    headers << "Server: HigloadWebServer/1.0 (Linux)\r\n"
            << "Date: " << boost::posix_time::second_clock::universal_time() << "\r\n"
            << "Connection: close\r\n";
    return headers.str();
}

std::string RequestHandler::file_headers(size_t length, const std::string& ext) const {
    std::ostringstream headers;
    headers << "Content-Length: " << length << "\r\n"
            << "Content-Type: " << ext_to_mime[ext] << "\r\n";
    return headers.str();
}

size_t RequestHandler::file_size(const std::string& path) const {
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    return (size_t)in.tellg();
}

bool RequestHandler::file_exists(const std::string &path) const {
    std::ifstream in(path);
    return in.good();
}

std::string RequestHandler::get_extension(const std::string &url) const {
    size_t pos = url.find_last_of('.');
    return (pos != std::string::npos) ? url.substr(pos + 1) : "txt";
}

std::string RequestHandler::get_code(int code) const {
    switch (code) {
        case 200:
            return "200 OK";
        case 403:
            return "403 Forbidden";
        case 404:
            return "404 Not Found";
        case 405:
            return "405 Method Not Allowed";
        case 501:
            return "501 Not Implemented";
        default:
            return "";
    }
}

std::string RequestHandler::build_headers(const std::string &protocol, const std::string &code,
                                          const std::string &headers) const {
    std::stringstream response;
    response << protocol << ' ' << code << "\r\n" << headers << "\r\n";
    return response.str();
}
