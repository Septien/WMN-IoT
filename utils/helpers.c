#include <string.h>

#include "helpers.h"
#include "printbinary.h"

int _mutex_lock(mutex_t *mtx)
{
#ifdef __LINUX__
    return (pthread_mutex_trylock(mtx) == 0 ? 1 : 0);
#endif
#ifdef __RIOT__
    return mutex_trylock(mtx);
#endif
}

void _mutex_unlock(mutex_t *mtx)
{
#ifdef __LINUX__
    pthread_mutex_unlock(mtx);
#endif
#ifdef __RIOT__
    mutex_unlock(mtx);
#endif
}

void *execute_rema(void *arg)
{
    (void) arg;
    REMA_t rema, *prema;
    uint64_t nid[2] = {1234567890, 9876543210};
    memcpy(rema._node_id, nid, 2 * sizeof(uint64_t));

    prema = &rema;
    rema_init(&prema);

    int end = 0;
    while (!end) {
        /* Handle requests from main thread */
        if (_mutex_lock(&mtx_req)) {
            switch (request)
            {
            case GET_NODEID:
                _mutex_lock(&mtx_data);
                memcpy(rema_data._node_id, rema._node_id, 2*sizeof(uint64_t));
                _mutex_unlock(&mtx_data);
                request = NONE;
                break;
            case STOP:
                end = 1;
                break;
            default:
                break;
            }
            _mutex_unlock(&mtx_req);
        }

        // Run the REMA protocol
        rema_dummy(prema);
#ifdef __RIOT__
        thread_yield();
        thread_sleep();
#endif
    }

    rema_terminate(&prema);

    return NULL;
}
