#ifndef YMUDUO_CURRENTTHREAD
#define YMUDUO_CURRENTTHREAD

#include <unistd.h>
#include <sys/syscall.h>

namespace ymuduo {

namespace CurrentThread {
    extern __thread int t_cachedTid;

    void cacheTid();

    inline int tid() {
        if (__builtin_expect(t_cachedTid == 0, 0))
            cacheTid();
        
        return t_cachedTid;
    }
}

}

#endif 