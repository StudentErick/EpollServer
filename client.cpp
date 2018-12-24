#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 64

int main(int argc, char* argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number\n", argv[0]);
    return -1;
  }
  const char *ip = argv[1];
  int port = atoi(argv[2]);
  if (port <= 1024 || port >= 65535) {
    perror("port error");
    return -1;
  }

  struct sockaddr_in serv_addr;
  bzero(&serv_addr, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr.s_addr) < 0) {
    perror("inet_pton() error\n");
    return -1;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket() error\n");
    return -1;
  }

  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(sockaddr_in)) < 0) {
    perror("connect() error\n");
    close(sockfd);
    return -1;
  }

  pollfd fds[2];
  fds[0].fd = 0;       // 读取标准输入
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  fds[1].fd = sockfd;  // 处理socket事件
  fds[1].events = POLLIN | POLLRDHUP;
  fds[1].revents = 0;

  char read_buf[BUFFER_SIZE];
  int pipefd[2];

  int ret = pipe(pipefd);
  if (ret < 0) {
    perror("pipe() error\n");
    return -1;
  }

  // 简易自动机模型，收发数据
  while (1) {
    ret = poll(fds, 2, -1); // 这里永久性阻塞等待数据

    if (ret < 0) {
      perror("poll() error\n");
      return -1;
    }

    if (fds[1].revents & POLLRDHUP) {
      bzero(read_buf, sizeof(read_buf));
      printf("server close the connection\n");
      break;
    }

    if (fds[1].revents & POLLIN) {
      bzero(read_buf, sizeof(read_buf));
      recv(fds[1].fd, read_buf, BUFFER_SIZE - 1, 0);
      printf("%s\n", read_buf);
    }

    if(fds[0].revents & POLLIN) {
      // 使用splice把用户输入直接写到sockfd上，0 copy
      ret = splice(0, NULL, pipefd[1], NULL, 32768,
                      SPLICE_F_MORE | SPLICE_F_MOVE);
      if (ret < 0) {
        perror("splice() error\n");
      }
      ret = splice(pipefd[0], NULL, sockfd, NULL, 32768,
                      SPLICE_F_MORE | SPLICE_F_MOVE);
      if (ret < 0) {
        perror("splice() error\n");
      }
    }
  }

  close(sockfd);
  return 0;
}
