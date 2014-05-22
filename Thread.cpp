#include "Thread.hpp"

int Thread::start(void) {
    pthread_attr_t * attr = NULL; // Use default attributes.
    int code = pthread_create(& thread, attr, start_routine, this);
    return code;
}

void * Thread::start_routine(void * pthis) {
   (static_cast<Thread *> (pthis))->run(); // \todo DANgerous?
    return 0;
}
