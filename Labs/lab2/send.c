#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc, char **argv)
{
  init(HOST, PORT);
  msg t, r;
  int fd, count;

  char filename[MAX_LEN], buffer[MAX_LEN];

  fd = open("file.in", O_RDONLY);
  if (fd < 0)
  {
    perror("send open");
    exit(-1);
  }

  strcpy(t.payload, "file.out"); //sau cu memcpy daca sunteti hardcore
  t.len = strlen("file.out");
  send_message(&t);
  printf("Filename Sent!");
  // puts(r.payload)
  memset(t.payload, 0, sizeof(t.payload));
  t.len = read(fd, t.payload, sizeof(t.payload));
  printf("SENDER %d\n", t.len);
  while (t.len > 0)
  {
    send_message(&t);
    memset(t.payload, 0, sizeof(t.payload));
    t.len = read(fd, t.payload, sizeof(t.payload));
    printf("SENDER %d\n", t.len);
  }
  
  memset(t.payload, 0, sizeof(t.payload));
  t.len = 0;
  send_message(&t); //closing message
  close(fd);
  return 0;
}
