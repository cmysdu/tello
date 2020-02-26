//
// Created by cenmmy on 20-2-24.
//

#ifndef TELLO_REMOTECONTROLLER_H
#define TELLO_REMOTECONTROLLER_H

#include <string>
#include <netinet/in.h>
#include <opencv2/opencv.hpp>
#include <h264decoder.hpp>
#include <thread>
#include <atomic>

#define HOST_IP     "0.0.0.0"
#define HOST_PORT 1111
#define REMOTE_IP "192.168.10.1"
#define REMOTE_PORT 8889

enum RCError {
    CREATE_SOCKET_ERROR,
    BIND_PORT_ERROR,
    COMMAND_SEND_ERROR
};

class RemoteController {

private:
    sockaddr_in tello_server;
    sockaddr_in stream_server;
    sockaddr_in state_server;
    int state_server_sock_fd;
    int stream_server_sock_fd;
    int local_server_sock_fd;
    cv::Mat frame;
    bool stop_recv_stream;
    bool stop_recv_state;
    std::thread t_decode_frame;
    std::thread t_recv_state;
    std::atomic<bool> frame_ready;

public:
    RemoteController();
    void active();
    void streamon();
    cv::Mat getFrame();
    void takeoff();
    void landing();
    void forward(int distance = 20);
    void back(int distance = 20);
    void left(int distance = 20);
    void right(int distance = 20);
    void up(int distance = 20);
    void down(int distance = 20);
    void disconnect();

private:
    void send(const std::string& message);
    void init();
    void createSocket();
    void bindPort();
    void setSockAddr(sockaddr_in& addr, const std::string& ip, int port);
    void setStreamServer();
    void setStateServer();
    void setTelloServer();
    void decodeFrame();
    void recvTelloState();
};

#endif //TELLO_REMOTECONTROLLER_H
