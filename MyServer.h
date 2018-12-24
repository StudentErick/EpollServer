//
// Created by erick on 12/24/18.
//

#ifndef SERVER_MYSERVER_H
#define SERVER_MYSERVER_H

#include "TcpServer.h"
#include "ThreadPool.h"

class MyServer : public TcpServer {
public:
    explicit MyServer(int maxWaiter = 5);

    void newConnection() override;

    void existConnection(int fd) override;

    int setnonblocking(int fd);

    inline void setTimer(unsigned long n);
    // 每隔一段时间写一次日志
    static void writeLog(int n);

    static unsigned long m_alarmSeconds;  // 定时器，间隔一定时间记录一次日志

private:
    std::shared_ptr<ThreadPool> threadPool;  // 测试用，暂时8个线程
};


#endif //SERVER_MYSERVER_H
