#include "webinterface.h"

#include <cstdlib>
#include <cstring>
#include <string>

namespace ddgen {

// This function will be called by civetweb on every new request.
int WebInterface::begin_request_handler(struct lh_ctx_t* ctx, struct lh_con_t* conn)
{
    const lh_rqi_t* request_info = httplib_get_request_info(conn);

    std::string local_uri(request_info->local_uri);

    if (("/healthz" == local_uri) || ("/health" == local_uri) || ("/readyz" == local_uri) || ("/ready" == local_uri)) {
        char content[1000];

        // Prepare the message we're going to send
        int content_length = snprintf(content,
                                      sizeof(content),
                                      "Everything will be ok! Remote port: %d \n and request uri: %s \n and loacu uri: %s",
                                      request_info->remote_port,
                                      request_info->request_uri,
                                      local_uri.c_str());

        // Send HTTP reply to the client
        httplib_printf(ctx,
                       conn,
                       "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Content-Length: %d\r\n" // Always set Content-Length
                       "\r\n"
                       "%s",
                       content_length,
                       content);

        // Returning non-zero tells civetweb that our function has replied to
        // the client, and civetweb should not send client any more data.
        return 1;
    } else if ("/image" == local_uri) {
        char content[1000];

        // Prepare the message we're going to send
        int content_length = snprintf(content, sizeof(content), "%s", std::getenv("IMAGE"));

        // Send HTTP reply to the client
        httplib_printf(ctx,
                       conn,
                       "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Content-Length: %d\r\n" // Always set Content-Length
                       "\r\n"
                       "%s",
                       content_length,
                       content);

        // Returning non-zero tells civetweb that our function has replied to
        // the client, and civetweb should not send client any more data.
        return 1;
    } else {
        return 0;
    }
}

WebInterface::WebInterface(bool shouldUseSecureWebInterface)
{
    static const std::string port = "8080";
    static const std::string secure_port = "443s";

    lh_clb_t callbacks;

    std::string listening_ports = port;
    const char* ssl_certificate = nullptr;
    if (shouldUseSecureWebInterface) {
        listening_ports += ", " + secure_port;
        ssl_certificate = "ssl_certificate";
    }

    // List of options. Last element must be NULL.
    lh_opt_t options[] = { { "error_log_file", "web_interface_error_logs.txt" },
                           { "listening_ports", listening_ports.c_str() },
                           { ssl_certificate, "ddgen-cert.pem" },
                           { nullptr, nullptr } };

    // Prepare callbacks structure. We have only one callback, the rest are NULL.
    std::memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = begin_request_handler;

    // Start the web server.
    ctx = httplib_start(&callbacks, NULL, options);
}

WebInterface::~WebInterface()
{
    // Stop the server.
    httplib_stop(ctx);
}

} // namespace ddgen
