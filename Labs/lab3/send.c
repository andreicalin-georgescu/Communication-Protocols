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

void send_with_parity(char *payload, int size){
    msg_par m_par;
    msg m, r;

    memcpy(m_par.content, payload, size);
    m_par.parity = seq_parity(m_par.content, size);
    memcpy(m.payload, &m_par, sizeof(m_par));
    m.len = sizeof(int) + size;

    send_message(&m);
    recv_message(&r);

    while(r.len == NACK){
        send_message(&m);
        recv_message(&r);
    }
}

int main(int argc, char **argv)
{
    init(HOST, PORT);
    msg t;

    for (int i = 0; i < 10; i++)
    {
        send_with_parity("Ana", strlen("Ana") + 1);
    }
    return 0;
}
