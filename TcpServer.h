//
// Created by erick on 12/24/18.
//

#ifndef SERVER_TCPSERVER_H
#define SERVER_TCPSERVER_H

#include "TcpSocket.h"
#include "user_data.h"

#include <sys/epoll.h>
#include <netinet/ip.h>
#include <memory>
#include <vector>

/*
 *  封装一个TcpServer类型
 */

class TcpServer {
public:
    explicit TcpServer(int maxWaiter = 5);

    bool listen(int _port = 8001);

    // 处理新的连接函数
    virtual void newConnection() = 0;

    // 处理已经存在的连接的函数
    virtual void existConnection(int sockfd) = 0;

    // 在这里进入事件的主循环，可以自行设置时间
    bool startService(int timeout = -1);

protected:
    struct sockaddr_in serv_addr; // 服务器信息
    std::shared_ptr<TcpSocket> m_tcpSocket;

    epoll_event m_epollEvents[MAX_EVENTS];  // epoll事件队列
    int m_epfd;   // epoll的fd
    int m_listen_sockfd;
};


#endif //SERVER_TCPSERVER_H
