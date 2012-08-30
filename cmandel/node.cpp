
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <inttypes.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "mandel.h"

typedef struct {
    unsigned int width, height, y0;
    double min_x, min_y, max_x, max_y;
    mandelbrot_type quality;
} job;

typedef struct {
    unsigned int height, y0;
} job_result;

void usage(){
    std::cout << "Mandelbrot set renderer network node. Available options:" << std::endl;
    std::cout << "-h\tMaster node address\tdefault: localhost" << std::endl;
    std::cout << "-p\tMaster node port\tdefault: 1573" << std::endl;
}

void error(){
    std::cerr << "Not enough command-line arguments" << std::endl;
    usage();
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1573);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    for (int i = 1; i < argc; i++){
        if (!strcmp(argv[i], "--help")){
            usage();
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[i], "-h")){
            if (i != argc-1){
                if (!inet_aton(argv[++i], &(addr.sin_addr))){
                    std::cerr << "Bad address" << std::endl;
                    usage();
                    exit(EXIT_FAILURE);
                }
            }
            else
                error();
        } else if (!strcmp(argv[i], "-p")){
            if (i != argc-1){
                short port;
                sscanf(argv[++i], "%hu" , &port);
                addr.sin_port = htons(port);
            }
            else
                error();
        }
    }

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        perror("socket");
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        perror("connect");

    std::cout << "Connected to " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;

    send(s, "mnd", 3, 0);
    char header[4] = { '\0' };

    while ((recv(s, &header, 3, 0) > 0) && !strcmp(header, "mnd")){
        job j;
        recv(s, &j, sizeof(j), 0);

        mandelbrot_type *data = new mandelbrot_type[j.width*j.height];

        MandelbrotRenderer renderer(
            j.width, j.height,
            j.min_x, j.min_y, j.max_x, j.max_y,
            j.quality,
            data
        );
        renderer.render();
        send(s, "mnd", 3, 0);
        job_result r = { j.height, j.y0 };
        send(s, &r, sizeof(r), 0);
        int sent = 0;
        int data_len = j.width*j.height*sizeof(mandelbrot_type);
        while (sent < data_len)
            sent += send(s, (void*)data + sent, data_len - sent, 0);
        delete []data;
    }
    std::cout << "Server closed connection" << std::endl;
    close(s);
}
