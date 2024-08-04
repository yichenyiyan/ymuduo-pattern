#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

#include "Timestamp.h"
#include "AsyncLogging.h"


using namespace ymuduo;
off_t kRollSize = 500*1000*1000;

AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const std::string& msg) {
    g_asyncLog->append(msg);
}

void bench(bool longLog) {
    Logger& logger = Logger::GetInstance();
    logger.setOutPut(asyncOutput);

    int cnt = 0;
    const int kBatch = 1000;
    std::string empty = " ";
    std::string longStr(250, 'X');
    longStr += " ";

    for (int t = 0; t < 30; ++t) {
        Timestamp start = ymuduo::Timestamp::now();
        for (int i = 0; i < kBatch; ++i) {
            LOG_INFO("Hello yichen%s%d", longStr.c_str(), cnt);
            ++cnt;
        }
        Timestamp end = Timestamp::now();
        printf("%f\n", timeDifference(end, start) * 1000000 / kBatch);
        struct timespec ts = { 0, 500*1000*1000 };
        nanosleep(&ts, NULL);
    }
}

int main(int argc, char* argv[]) {
    {
        // set max virtual memory to 2GB.
        size_t kOneGB = 1000 * 1024 * 1024;
        rlimit rl = { 2*kOneGB, 2*kOneGB };
        setrlimit(RLIMIT_AS, &rl);
    }

    printf("pid = %d\n", getpid());

    char name[256] = { '\0' };
    strncpy(name, argv[0], sizeof name - 1);
    AsyncLogging log(::basename(name), kRollSize);
    log.start();
    g_asyncLog = &log;

    bool longLog = argc > 1;
    bench(longLog);

    return 0;
}
