#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

void receive_with_parity() {
  msg_par m_par;
  msg m;

  while(1) {

    recv_message(&m);
    memcpy(&m_par, m.payload, m.len);

    if(seq_parity((unsigned char *)(m_par.content), m.len - sizeof(int)) == m_par.parity) {
      m.len = ACK;
      puts(m_par.content);
      send_message(&m);
      break;
    } else {
      m.len = NACK;
      send_message(&m);
    }
  }
}


int main(int argc,char** argv){
  init(HOST,PORT);

    for (int i = 0; i < 10; i++)
    {
        receive_with_parity();
    }


  return 0;
}
