//
// Created by vladislav on 07.09.16.
//

#ifndef WEBSERVER_REQUEST_HANDLER_HPP
#define WEBSERVER_REQUEST_HANDLER_HPP

#include <string>
#include <unordered_map>

class RequestHandler {
    static const unsigned BUFFER_SIZE = 8192;

    std::string root_dir;
    mutable std::unordered_map<std::string, std::string> ext_to_mime =
            {
                    { "txt", "text/plain" },
                    { "html", "text/html" },
                    { "css", "text/css" },
                    { "js", "application/javascript" },
                    { "jpg", "image/jpeg" },
                    { "jpeg", "image/jpeg" },
                    { "png", "image/png" },
                    { "gif", "image/gif" },
                    { "swf", "application/x-shockwave-flash" }
            };

    const char* not_found =
            "<html>"
            "<head><title>Not Found</title></head>"
            "<body><h1>404 Not Found</h1></body>"
            "</html>";
    const char* forbidden =
            "<html>"
            "<head><title>Forbidden</title></head>"
            "<body><h1>403 Forbidden</h1></body>"
            "</html>";
    const char* not_allowed =
            "<html>"
            "<head><title>Not Allowed</title></head>"
            "<body><h1>404 Method Not Allowed</h1></body>"
            "</html>";
    const char* not_implemented =
            "<html>"
            "<head><title>Not Implemented</title></head>"
            "<body><h1>501 Not Implemented</h1></body>"
            "</html>";

    void GET(const std::string &url, const std::string &protocol,
             std::function<void (const std::string&)> writeCallback) const;
    void HEAD(const std::string &url, const std::string &protocol,
             std::function<void (const std::string&)> writeCallback) const;
    void NotAllowed(const std::string &protocol,
                    std::function<void (const std::string&)> writeCallback) const;
    void NotImplemented(const std::string &protocol,
                    std::function<void (const std::string&)> writeCallback) const;

    std::string url_decode(const std::string& url) const;
    std::string message_headers() const;
    bool file_exists(const std::string& path) const;
    size_t file_size(const std::string& path) const;
    std::string file_headers(size_t length, const std::string& ext) const;
    std::string get_extension(const std::string& url) const;
    bool is_directory(const std::string& path) const;
    std::string get_code(int code) const;
    std::string build_headers(const std::string& protocol, const std::string& code,
                               const std::string& headers) const;

public:
    void handle_request(const std::string& request, std::function<void (const std::string&)> writeCallback) const;

    RequestHandler(const std::string& dir);
};


#endif //WEBSERVER_REQUEST_HANDLER_HPP
