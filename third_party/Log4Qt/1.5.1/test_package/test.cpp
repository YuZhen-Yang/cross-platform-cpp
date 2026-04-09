#include <log4qt/logger.h>
#include <log4qt/logmanager.h>
#include <iostream>

int main()
{
    Log4Qt::Logger *logger = Log4Qt::Logger::rootLogger();
    if (!logger) {
        std::cerr << "Failed to get Log4Qt root logger" << std::endl;
        return 1;
    }
    std::cout << "Log4Qt loaded successfully" << std::endl;
    return 0;
}
