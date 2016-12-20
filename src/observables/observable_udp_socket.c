#include "observable_udp_socket.h"

/**
 * @brief udp socket (UDP_Socket) observable
 */
typedef struct {
    Observable base; /**< Parent observable structure */
    uv_udp_t udp;    /**< Libuv udp handle */
} UDP_Socket;

static void udp_socket_destroy_cb(Observable *observable) {
    observable_deinit(observable);

    UDP_Socket *udp_socket = (UDP_Socket *) observable;
    uv_udp_recv_stop(&udp_socket->udp);

    free(udp_socket);
}

static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = xmalloc(suggested_size);
    buf->len = suggested_size;
}

static void on_receive(uv_udp_t *handle, ssize_t nread,
        const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
    Observable *observable = (Observable *)handle->data;

    if (nread == 0) {
        if (addr == NULL) {
            // End of packet
            observable->callback(observable, end_of_data());
        }
    } else if (nread < 0) { // Some error occured
        log_error("Failed to receive data from socket because of %s.", uv_strerror(nread));
    } else { // Normal data received
        // BEWARE: put a (char *) to user. May be dangerous,
        // because it should not be a null-terminated C string.
        void *data = observable->callback(observable, (void *)buf->base);
        observable_broadcast(observable, data);
    }

    if (buf->base) {
        free(buf->base);
    }
}

Observable *observable_udp_socket_create(Loop *loop, unsigned short port, observable_cb callback) {
    struct sockaddr_in addr;

    CHECK_NULL_RETURN(loop, NULL);

    UDP_Socket *udp_socket = xmalloc(sizeof(UDP_Socket));

    observable_init(&udp_socket->base);
    udp_socket->base.destroy_cb = udp_socket_destroy_cb;
    udp_socket->base.callback = callback;

    uv_ip4_addr("0.0.0.0", port, &addr);

    uv_udp_init(loop->loop, &udp_socket->udp);
    udp_socket->udp.data = udp_socket;
    uv_udp_bind(&udp_socket->udp, (const struct sockaddr *)&addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&udp_socket->udp, alloc_buffer, on_receive);

    return (Observable *)udp_socket;
}
