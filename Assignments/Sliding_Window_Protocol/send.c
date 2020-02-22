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

int main(int argc,char** argv){
  init(HOST,PORT);
  msg t;
  int fd, fsize, COUNT;

  // opening input file
  fd = open("fileX", O_RDONLY);

  if (fd < 0) {
    perror("[SENDER] Error opening input file. Exiting");
    return -1;
  }

  // calculating file size
  fsize = lseek(fd, 0, SEEK_END);

  // resetting file pointer to begin reading data
  lseek(fd, 0, SEEK_SET);

  // calculating window_size
  int window_size = atol(argv[2]) * atol(argv[3]) * 1000/ (sizeof(msg) * 8);

  // computing number of frames according to file and payload size
  COUNT = fsize / MSGSIZE;

  if (fsize % MSGSIZE != 0) {
    COUNT++;
  }

  if(window_size > COUNT){
    window_size = COUNT;
  }

  // filename
  sprintf(t.payload, argv[1]);
  t.len = strlen(t.payload);
  send_message(&t);

  printf("Win size:%d\n", window_size);

  // send file size to receiver
  memcpy(t.payload, &COUNT, sizeof(int));
  t.len = COUNT;
  send_message(&t);

  memset(t.payload, 0, sizeof(t.payload));

  // sending nr of window frames for the receiver
  for(int i = 0; i < window_size; i++) {

    t.len = read(fd, t.payload, sizeof(t.payload));

    if (send_message(&t) > 0) {
      printf("Sent ok\n");
    } else {
      perror("send error");
    }
  }

  // follow-up COUNT - window_size messages where we wait for ACK

  for (int i = 0; i < COUNT - window_size; i++) {

    // wait for ACK
    printf("Received ACK, sending again\n");
    if (recv_message(&t) < 0) {
      perror("[SENDER] Receive error. Exiting.\n");
      return -1;
    }

    memset(t.payload, 0, sizeof(t.payload));
    t.len = read(fd, t.payload, sizeof(t.payload));

    // send msg
    if (send_message(&t) < 0) {
      perror("[SENDER] Send error. Exiting.\n");
      return -1;
    }


  }

  // wait for the final window_size ACKs

  for (int i = 0; i < window_size; ++i) {

    // wait for ACK
    printf("Waiting for last window_size ACKs\n");
    if (recv_message(&t) < 0) {
      perror("[SENDER] Receive error. Exiting.\n");
      return -1;
    }
  }

  printf("[SENDER] Job done, all sent.\n");

  // cleanup: closing input file
  close(fd);

  return 0;
}
