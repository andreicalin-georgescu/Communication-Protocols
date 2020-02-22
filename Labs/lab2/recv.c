
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc, char **argv)
{

  init(HOST, PORT);
  msg t, r;
  int fd, count;

  if (recv_message(&r) < 0)
  {
    perror("[RECEIVER] Recv error");
    return -1;
  }
  fd = open(r.payload, O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (fd < 0)
  {
    perror("send open");
    exit(-1);
  }
  puts("Got filename!");
  memset(r.payload, 0, sizeof(r.payload));
  if (recv_message(&r) < 0)
  {
    perror("[RECEIVER] Recv error");
    return -1;
  }
  printf("Receiver %d\n", r.len);
  while (r.len > 0)
  {
    write(fd, r.payload, r.len);
    memset(r.payload, 0, sizeof(r.payload));
    if (recv_message(&r) < 0)
    {
      perror("[RECEIVER] Recv error");
      return -1;
    }
    printf("RECEIVER %d\n", r.len);
  }
  close(fd);
  return 0;
}
