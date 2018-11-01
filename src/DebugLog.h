#ifndef ZKCLIENT_DISTRIBUTION_LOG_H
#define ZKCLIENT_DISTRIBUTION_LOG_H
#include <string.h>
#include <stdio.h>
#include <string>

#define LOG_DEBUG(format, ...) do { \
    char buffer[64];     \
    fprintf(stdout, "[DEBUG] [%s:%d] " format "\n", buffer, strrchr(__FILE__, '/') + 1, __LINE__, ##__VA_ARGS__); } while(0)


#endif //ZKCLIENT_DISTRIBUTION_LOG_H
