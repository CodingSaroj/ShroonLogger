#define SLOG_IMPLEMENTATION
#include "Shroon/Logger/Logger.h"

enum MyLogLevels
{
    INFO,
    WARN,
    ERR,
    FERR
};

#define MY_LOG(...) \
    {\
        char * fPrefix = SLOGFormat("%=cynbxInfo:%=cynxx"), *fMsg = SLOGFormat(__VA_ARGS__);\
        SLOGLog(INFO, fPrefix, fMsg);\
        SUTLStringFree(fMsg), SUTLStringFree(fPrefix);\
    }

#define MY_WARN(...) \
    {\
        char * fPrefix = SLOGFormat("%=ylwbxWarning:%=ylwxx"), *fMsg = SLOGFormat(__VA_ARGS__);\
        SLOGLog(WARN, fPrefix, fMsg);\
        SUTLStringFree(fMsg), SUTLStringFree(fPrefix);\
    }

#define MY_ERR(...) \
    {\
        char * fPrefix = SLOGFormat("%=redbxError:%=redxx"), *fMsg = SLOGFormat(__VA_ARGS__);\
        SLOGLog(ERR, fPrefix, fMsg);\
        SUTLStringFree(fMsg), SUTLStringFree(fPrefix);\
    }

#define MY_FATAL(...) \
    {\
        char * fPrefix = SLOGFormat("%=redbxFatalError:%=redxx"), *fMsg = SLOGFormat(__VA_ARGS__);\
        SLOGLog(FERR, fPrefix, fMsg);\
        SUTLStringFree(fMsg), SUTLStringFree(fPrefix);\
    }

int main()
{
    SLOGInit();

    SLOGSetOutputFile(stderr);

    MY_LOG("Log 1.\n");
    MY_WARN("Warn 1.\n");
    MY_LOG("Log 2.\n");
    MY_ERR("Err 1.\n");
    MY_LOG("Log 3.\n");
    MY_WARN("Warn 2.\n");
    MY_LOG("Log 4.\n");
    MY_LOG("Log 5.\n");
    MY_FATAL("Fatal 1.\n");
}
