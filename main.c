#include <stdio.h>
#include "context.h"
#include "ring.h"
#include "utils.h"

typedef struct task_s {
    coroutine_t *coroutine;
    int result;
} task_t;

void ring_coroutine_cb(void *data, int result)
{
    ring_handle_t *handle = (ring_handle_t*) data;

    task_t *task = (task_t*) handle->user_data;

    task->result = result;

    coroutine_enter(task->coroutine);
}

int ring_listener_accept_await(ring_listener_t *listener)
{
    task_t task = {
            .coroutine = coroutine_current_context(),
            .result = 0
    };

    listener->handle.user_data = (void*) &task;

    ring_listener_start(listener, (listener_cb) ring_coroutine_cb);
    coroutine_yield(coroutine_current_context());

    ring_listener_stop(listener);

    return task.result;
}

int ring_tcp_send_await(ring_tcp_t *handle, const char *buffer, size_t size)
{
    task_t task = {
            .coroutine = coroutine_current_context(),
            .result = 0
    };

    handle->handle.user_data = (void*) &task;

    ring_tcp_send(handle, buffer, size, (tcp_cb) ring_coroutine_cb);
    coroutine_yield(coroutine_current_context());

    return task.result;
}

int ring_tcp_receive_await(ring_tcp_t *handle, char *buffer, size_t size)
{
    task_t task = {
            .coroutine = coroutine_current_context(),
            .result = 0
    };

    handle->handle.user_data = (void*) &task;

    ring_tcp_receive(handle, buffer, size, (tcp_cb) ring_coroutine_cb);
    coroutine_yield(task.coroutine);

    return task.result;
}

void worker(coroutine_t *context, void *args)
{
    ring_tcp_t *stream = (ring_tcp_t*) args;
    char buffer[512];

    int received = ring_tcp_receive_await(stream, buffer, 512);

    printf("%d received!\n", received);
}

void server_main(coroutine_t *context, void *args)
{
    int server_fd = make_server(8090);

    ring_listener_t listener;

    ring_listener_init(&listener, args, server_fd);

    for (;;) {
        int client_fd = ring_listener_accept_await(&listener);

        coroutine_t *new_coroutine = (coroutine_t*) malloc(sizeof(coroutine_t));
        ring_tcp_t *tcp = (ring_tcp_t*) malloc(sizeof(ring_tcp_t));
        ring_tcp_init(tcp, args, client_fd);

        coroutine_spawn(new_coroutine, worker, tcp);
    }
}

int main()
{
    ring_loop_t loop;

    ring_loop_init(&loop, 4096);

    coroutine_t server;

    coroutine_spawn(&server, server_main, &loop);
    coroutine_enter(&server);

    ring_loop_submit(&loop);
    ring_loop_run(&loop);

    return 0;
}
