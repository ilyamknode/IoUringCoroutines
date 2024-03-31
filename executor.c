//
// Created by user on 3/31/24.
//

#include "executor.h"

void executor_init(executor_t *executor)
{
    memset(executor, 0, sizeof(executor_t));
    TAILQ_INIT(&executor->pending);
    //TAILQ_INIT(&executor->active);
}

void executor_spawn(executor_t *executor, coroutine_entry_point entry, void *args)
{
    executor_task_t *task = (executor_task_t*) malloc(sizeof(executor_task_t));

    memset(task, 0, sizeof(executor_task_t));

    coroutine_spawn(&task->context, entry, args);

    TAILQ_INSERT_TAIL(&executor->pending, task, entries);
}

void executor_run(executor_t *executor)
{
    executor_task_t *task = executor->pending.tqh_first;

    while (task != NULL) {
        coroutine_enter(&task->context);

        TAILQ_REMOVE(&executor->pending, executor->pending.tqh_first, entries);

        task = executor->pending.tqh_first;
    }
}