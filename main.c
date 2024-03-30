#include <stdio.h>
#include "context.h"

void worker(coroutine_t *coroutine, void *user_data)
{
    printf("worker ID: %p\n", user_data);
    coroutine_yield(coroutine);
    printf("%p after yield\n", user_data);
}

int main()
{
    coroutine_t coroutine1, coroutine2;

    coroutine_spawn(&coroutine1, worker, (void*) 1);
    coroutine_spawn(&coroutine2, worker, (void*) 2);
    coroutine_enter(&coroutine2);
    coroutine_enter(&coroutine1);
    coroutine_enter(&coroutine2);
    return 0;
}
