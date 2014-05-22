#pragma once

#include <pthread.h>
#include <signal.h>

class Thread {
public:
    Thread() {}
//      ~Thread() {}
    int start(void);
    /* Suspends the execution of the caller until run terminates, either by
       calling exit or by being cancelled. */
    void * join(void) {
        void **thread_return;
        pthread_join(thread, thread_return);
        return *thread_return;
    }
    int kill(int signal_number) {
        return pthread_kill(thread, signal_number);
    }
protected:
    virtual void run(void) {}      // To be overloaded in sub-classes.
    static void * start_routine(void *);
private:
    Thread(const Thread& in) {} // Disable copy constructor.
    pthread_t thread;
};
