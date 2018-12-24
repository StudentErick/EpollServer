//
// Created by erick on 12/24/18.
//

#include <strings.h>
#include <iostream>
#include "TcpServer.h"

TcpServer::TcpServer(int maxWaiter) {
    m_tcpSocket = std::make_shared<TcpSocket>(maxWaiter);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1024);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    m_listen_sockfd = m_tcpSocket->getSockFD();
    m_epfd = epoll_create1(0);
}

bool TcpServer::listen(int _port) {
    return m_tcpSocket->bindPort(_port) && m_tcpSocket->listenOn();
}

bool TcpServer::startService(int timeout) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));

    ev.data.fd = m_listen_sockfd;
    ev.events = EPOLLIN | EPOLLET;    // 新来的连接
    if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listen_sockfd, &ev) < 0) {
        return false;
    }

    // 服务器的监听循环
    while (true) {
        int nfds = epoll_wait(m_epfd, m_epollEvents, MAX_EVENTS, timeout);
        if (nfds < 0) {
            return false;
        }

        for (int i = 0; i < nfds; ++i) {
            if (m_epollEvents[i].data.fd == m_listen_sockfd) {
                // 处理新的连接
                newConnection();
            } else {
                // 处理已经存在的连接
                existConnection(i);
            }
        }
    }

    return true;
}

