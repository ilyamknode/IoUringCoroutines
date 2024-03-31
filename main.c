#include <stdio.h>
#include "context.h"
#include "executor.h"
#include "ring.h"
#include "utils.h"
#include "async.h"

#define SERVER_PORT 8080
#define IORING_QUEUE_SIZE 4096

executor_t g_executor = {0};

void main_worker(coroutine_t *context, void *args)
{
    ring_tcp_t *tcp = (ring_tcp_t*)args;

    char buffer[512];

    int result = ring_tcp_receive_await(tcp, buffer, 128);

    printf("result: %d\n", result);

    result = ring_tcp_receive_await(tcp, buffer, 128);

    printf("result: %d\n", result);
}

void new_connection_cb(ring_listener_t *listener, int result)
{
    printf("New connection: %d\n", result);

    ring_tcp_t *tcp = (ring_tcp_t*) malloc(sizeof(ring_tcp_t));
    ring_tcp_init(tcp, listener->handle.loop, result);

    executor_spawn(&g_executor, main_worker, tcp);
}

void ring_prepare_executor()
{
    executor_run(&g_executor);
}

int main()
{
    executor_init(&g_executor);

    ring_loop_t loop;
    ring_loop_init(&loop, IORING_QUEUE_SIZE);

    int server_fd = make_server(SERVER_PORT);
    ring_listener_t listener;
    ring_listener_init(&listener, &loop, server_fd);
    ring_listener_start(&listener, new_connection_cb);

    ring_loop_run_with_prepare(&loop, ring_prepare_executor);
    return 0;
}
