
#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <set>

#include "mandel.h"

typedef enum {
    Unknown,
    Waiting,
    Running,
    Closed
} ClientStatus;

typedef struct {
    mutable ClientStatus status;
    int fd;
    struct sockaddr_in addr;
} client;

class MandelbrotServer {
public:
    MandelbrotServer(unsigned int width_, unsigned int height_,
                     double min_x_, double min_y_, double max_x_, double max_y_,
                     mandelbrot_type quality_, mandelbrot_type *data_,
                     int port_ = 1573);
    ~MandelbrotServer();
    void run();
private:
    void send_next_job(client to);
    void load_job_result(client from);
    unsigned int width, height;
    double min_x, min_y, max_x, max_y;
    mandelbrot_type quality;
    mandelbrot_type *data;
    double step_x, step_y;
    int port;
    int master_socket;
    unsigned int stripsize, y, done;
    std::set<client> clients;
};
