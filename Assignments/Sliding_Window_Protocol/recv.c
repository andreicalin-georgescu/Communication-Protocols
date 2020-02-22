#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc,char** argv){
  msg r;
  int fd, COUNT;
  init(HOST,PORT);

  // filename
  if (recv_message(&r) < 0){
    perror("Receive message");
    return -1;
  }

  char nume_fisier[100] = "recv_";

  strcat(nume_fisier, r.payload);

  // open output file for writing
  fd = open(nume_fisier, O_WRONLY | O_CREAT | O_TRUNC, 0600);

  if (fd < 0) {
    perror("send open");
    return -1;
  }

  // ready output file to receive message
  lseek(fd, 0, SEEK_SET);


  // receive input COUNT frames
  if (recv_message(&r) < 0){
    perror("Failed to receive count size. Exiting");
    return -1;
  }

  memcpy(&COUNT, r.payload, sizeof(int));

  printf("COUNT_recv: %d\n", COUNT);

  // wait for messages and send ACKs if everything all right
  for (int i = 0; i < COUNT; i++) {
    // wait for message
    if (recv_message(&r) < 0) {
      perror("[RECEIVER] Receive error. Exiting.\n");
      return -1;
    }

    write(fd, r.payload, r.len);
    memset(r.payload, 0, sizeof(r.payload));

    // send dummy ACK //
    if (send_message(&r) < 0) {
      perror("[RECEIVER] Send ACK error. Exiting.\n");
      return -1;
    }
  }

  // cleanup: close output file
  close(fd);
  return 0;
}
