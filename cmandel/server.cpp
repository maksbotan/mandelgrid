
#include <cstdio>
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <set>
#include <stdexcept>

#include "server.h"

bool operator<(const client& left, const client& right){
    return left.fd < right.fd;
}

typedef struct {
    unsigned int width, height, y0;
    double min_x, min_y, max_x, max_y;
    mandelbrot_type quality;
} job;

typedef struct {
    unsigned int height, y0;
} job_result;

MandelbrotServer::MandelbrotServer(unsigned int width_, unsigned int height_,
                                   double min_x_, double min_y_, double max_x_, double max_y_,
                                   mandelbrot_type quality_, mandelbrot_type *data_,
                                   int port_) :
    width(width_),
    height(height_),
    min_x(min_x_),
    min_y(min_y_),
    max_x(max_x_),
    max_y(max_y_),
    quality(quality_),
    data(data_),
    port(port_),
    stripsize(ceil(height/16.0)),
    y(0),
    done(0)
{
    step_x = (max_x - min_x) / width;
    step_y = (max_y - min_y) / height;

    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket == -1){
        perror("Cannot create socket");
        throw std::runtime_error("socket");
    }

    fcntl(master_socket, F_SETFL, O_NONBLOCK);
    int sockopt = 1;
    setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(master_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        perror("Cannot bind to requested port");
        throw std::runtime_error("bind");
    }

    listen(master_socket, 1);

    std::cout << "Listening on " << inet_ntoa(addr.sin_addr) << ":" << port << std::endl;
}

MandelbrotServer::~MandelbrotServer(){
    for (std::set<client>::iterator it = clients.begin(); it != clients.end(); it++)
        close(it->fd);
    clients.clear();
    close(master_socket);
}

void MandelbrotServer::run(){
    while (1){
        if (done >= height){
            std::cout << "Rendering complete" << std::endl;
            return;
        }

        timeval to;
        to.tv_sec = 1;
        to.tv_usec = 0;

        fd_set sockets;
        FD_ZERO(&sockets);
        FD_SET(master_socket, &sockets);
        for (std::set<client>::iterator it = clients.begin(); it != clients.end(); it++)
            FD_SET(it->fd, &sockets);

        if (select(FD_SETSIZE, &sockets, NULL, NULL, &to) == -1){
            perror("Error while select()'ing");
            throw std::runtime_error("select");
        }

        if (FD_ISSET(master_socket, &sockets)){
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int client_sock = accept(master_socket, (struct sockaddr*)&client_addr, &addr_len);
            if (client_sock < 0){
                perror("Error while accept()'ing");
                throw std::runtime_error("accept");
            }
            std::cout << "Client " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << " connected" << std::endl;

            client client_data = {Unknown, client_sock, client_addr};
            clients.insert(client_data);
        }
        for (std::set<client>::iterator it = clients.begin(); it != clients.end();){
            std::set<client>::iterator current = it++;
            if (FD_ISSET(current->fd, &sockets)){
                char header[4] = { '\0' };
                if (recv(current->fd, &header, 3, 0) <= 0){
                    std::cout << "Client " << inet_ntoa(current->addr.sin_addr) << ":" << ntohs(current->addr.sin_port) << " disconnected" << std::endl;
                    current->status = Closed;
                    close(current->fd);
                    clients.erase(current);
                    continue;
                }
                if (strcmp(header, "mnd")){
                    std::cerr << "Client " << inet_ntoa(current->addr.sin_addr) << ":" << ntohs(current->addr.sin_port) << " sent bad header" << std::endl;
                    close(current->fd);
                    clients.erase(current);
                    continue;
                }
                switch (current->status){
                case Unknown:
                    current->status = Waiting;
                    break;
                case Waiting:
                    std::cerr << "Client " << inet_ntoa(current->addr.sin_addr) << ":" << ntohs(current->addr.sin_port) << " sent extra data while waiting for task" << std::endl;
                    current->status = Closed;
                    close(current->fd);
                    clients.erase(current);
                    break;
                case Running:
                    load_job_result(*current);
                    current->status = Waiting;
                    break;
                case Closed:
                    break; //Impossible situation
                }
            }
            if (current->status == Waiting && y < height){
                send_next_job(*current);
                current->status = Running;
            }
        }
    }
}

void MandelbrotServer::send_next_job(client to){
    unsigned int y1 = (y + stripsize < height) ? (y + stripsize) : height;
    job j = { width, y1 - y, y, min_x, min_y + y*step_y, max_x, min_y + y1*step_y, quality};
    send(to.fd, "mnd", 3, 0);
    send(to.fd, &j, sizeof(j), 0);
    y = y1;
    std::cout << "Sent job to client " << inet_ntoa(to.addr.sin_addr) << ":" << ntohs(to.addr.sin_port) << std::endl;
}

void MandelbrotServer::load_job_result(client from){
    job_result res;
    if (recv(from.fd, &res, sizeof(res), 0) != sizeof(res)){
        std::cerr << "Bad result from client!" << std::endl;
        throw std::runtime_error("recv");
    }
    int data_len = width * res.height * sizeof(mandelbrot_type);
    void *t_data = (void*)(data+res.y0*width);
    int rcvd = 0;
    while (rcvd < data_len)
        rcvd += recv(from.fd, t_data + rcvd, data_len-rcvd, 0);
    done += res.height;
}
