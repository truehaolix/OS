#include <linklist.h>
#include "callback.h"
#include "mm.h"

struct callback_t *cb_list;

int register_callback(enum cb_type type,
        struct time_t *delay, cb_func_t *callback)
{
    struct callback_t *cb =
        (struct callback_t *) kalloc(sizeof(struct callback_t));
    if (!cb)
        return -1;

    cb->reg_time = get_cur_milis();
    cb->delay = time_to_milis(delay);
    cb->callback = callback;
    cb->type = type;

    /* create a new linklist if none exist */
    if (!cb_list)
    {
        llist_init(cb, ll);
        cb_list = cb;
    }
    else
        llist_add_before(cb_list, cb, ll);

    return 0;
}

/*
 * Removes a registered callback.
 * Returns -1 if provided callback doesn't exist. 0 otherwise.
 */
int remove_callback(struct callback_t *cb)
{
    if (!llist_is_in_list(cb, ll))
        return -1;

    llist_delete(cb, ll);
    free(cb);
    return 0;
}

/*
 * Checks callbacks and executes the ones which is time to trigger.
 * Depending on the type of callback, after the callback function
 * returns, the entry is removed from entry or rescheduled for repeat.
 */
void check_callbacks()
{
    struct callback_t tmp;
    struct callback_t *cb = &tmp;
    size_t idx;
    milis_t cur_milis;

    if (!cb_list)
        return;

    cur_milis = get_cur_milis();

    llist_foreach(cb_list, cb, idx, ll)
    {
        if (cb->reg_time + cb->delay <= cur_milis)
        {
            cb->callback(NULL);
            if (cb->type == CALLBACK_REPEAT)
                cb->reg_time = cur_milis;
            else
                llist_delete(cb, ll);
        }
    }
}
