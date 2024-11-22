#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 5050
#define BUF_SIZE 1024

volatile sig_atomic_t wasSigHup = 0;
int client_socket = -1;

void sigHupHandler(int r){
  if(r == SIGHUP){
    wasSigHup = 1;
  }
}

int main(){
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigHupHandler;
  sa.sa_flags = SA_RESTART;
  sigaction(SIGHUP, &sa, NULL);

  int serv_socket;
  struct sockaddr_in server_addr;
  socklen_t client_len = sizeof(struct sockaddr_in);

  serv_socket = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  bind(serv_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

  printf("Сервер запущен и слушает порт %d\n", PORT);

  fd_set fds;
  sigset_t blockedMask, origMask;
  sigemptyset(&blockedMask);
  sigaddset(&blockedMask, SIGHUP);
  sigprocmask(SIG_BLOCK, &blockedMask, &origMask);

  while(1){
    if(wasSigHup){
      printf("Получен сигнал SIGHUP.\n");
      if(client_socket == -1){
        close(client_socket);
        client_socket = -1
        printf("Сокет клиента закрыт.\n");
      }
      wasSigHup = 0;
      continue;
    }

    FD_ZERO(&fds);
    FD_SET(serv_socket, &fds);
    if(client_socket != -1){
      FD_SET(client_socket, &fds);
    }

    int max_fd = (client_socket > serv_socket) ? client_socket : serv_socket;

    int res = pselect(max_fd + 1, &fds, NULL, NULL, NULL, &origMask);
    if(res == -1){
      if(errno == EINTR){
        printf("pselect был прерван сигналом.\n");
        continue;
      }
    }

    if(FD_ISSET(serv_socket, &fds)){
      int new_client_socket = accept(serv_socket, NULL, &client_len);
      printf("Принято новое соединение.\n");
      if(client_socket == -1){
        client_socket = new_client_socket;
      }
      else{
        close(new_client_socket);
        printf("Закрыто лишнее соединение.\n");
      }
    }

    if(client_socket != -1 && FD_ISSET(client_socket, &fds)){
      char buf[BUF_SIZE];
      ssize_t size_data = read(client_socket, buf, BUF_SIZE);
      if(size_data > 0){
        printf("Получено %ld байтов данных.\n", size_data);
      }
      else if(size_data == 0){
        printf("Клиент закрыл соединение.\n");
        close(client_socket);
        client_socket = -1;
      }
      else
        perror("Ошибка получения данных.\n");
    }
  }

  close(serv_socket);
  if(client_socket != -1){
    close(client_socket);
  }
  printf("Сокет сервера закрыт.\n");
  return 1;
}
