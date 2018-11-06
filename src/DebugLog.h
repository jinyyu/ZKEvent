#ifndef ZKCLIENT_DISTRIBUTION_LOG_H
#define ZKCLIENT_DISTRIBUTION_LOG_H
#include <string.h>
#include <stdio.h>
#include <string>

#ifdef DEBUG
#define LOG_DEBUG(format, ...) do { fprintf(stderr, "[DEBUG] [%s:%d] " format "\n", strrchr(__FILE__, '/') + 1, __LINE__, ##__VA_ARGS__); } while(0)
#else
#define LOG_DEBUG(format, ...)
#endif


#endif //ZKCLIENT_DISTRIBUTION_LOG_H
