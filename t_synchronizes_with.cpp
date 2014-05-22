/*! \file t_synchronizes_with.cpp
 * \brief
 * \see http://preshing.com/20130823/the-synchronizes-with-relation
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <iostream>
#include <string>
#include <atomic>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

struct Message {
    clock_t     tick;
    const char* str;
    void*       param;
};

std::atomic<int> g_guard(0);

void SendTestMessage(void* param) {
    // Copy to shared memory using non-atomic stores.
    g_payload.tick  = clock();
    g_payload.str   = "TestMessage";
    g_payload.param = param;

    // Perform an atomic write-release to indicate that the message is ready.
    g_guard.store(1, std::memory_order_release);
}

/*! Perform an atomic read-acquire to check whether the message is ready. */
bool TryReceiveMessage(Message& result) {
    int ready = g_guard.load(std::memory_order_acquire);
    if (ready != 0) {
        // Yes. Copy from shared memory using non-atomic loads.
        result.tick  = g_payload.tick;
        result.str   = g_msg_str;
        result.param = g_payload.param;

        return true;
    }
    return false;               // no
}

int main(int argc, const char * argv[], const char * envp[])
{

    return 0;
}
