#pragma once

#define RESPONSE_LINE_200 "HTTP/1.0 200 OK\r\n"
#define RESPONSE_LINE_404 "HTTP/1.0 404 OK\r\n"

#define RESPONSE_HEADERS "Server: webserver-c\r\n"  \
                        "Content-type: text/html\r\n"
#define RESPONSE_FILE_HEADERS "Server: webserver-c\r\n"  \
                        "Content-type: application/octet-stream\r\n"
#define CONTENT_LENGTH "Content-Length: "

#define HTML_PART1 "<html>"     \
    "<head>"                    \
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" \
    "<title>Directory listing for "

#define HTML_PART2 "</title>"   \
    "</head><body><h1>"

#define HTML_PART3 "</h1><hr><ul>"
#define HTML_PART4 "</ul><hr></body></html>"

#define LINK_PART1 "<li><a href=\"http://127.0.0.1:"
#define LINK_PART2 "\">"
#define LINK_PART3 "</a></li>"

#define FILE_NOT_FOUND  "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">"    \
                        "<title>Error response</title></head><body><h1>Error response</h1>"                     \
                        "<p>Error code: 404</p><p>Message: File not found.</p>"                                 \
                        "<p>Error code explanation: HTTPStatus.NOT_FOUND - Nothing matches the given URI.</p>"  \
                        "</body></html>"

void handleRequest(int sock);
