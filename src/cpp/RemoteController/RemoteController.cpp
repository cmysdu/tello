//
// Created by cenmmy on 20-2-24.
//
#include <RemoteController.h>
#include <sys/socket.h>
#include <libnet.h>
#include <libavutil/frame.h>

RemoteController::RemoteController (): stop_recv_stream(true), frame_ready(false), stop_recv_state(true){
    init();
}

void RemoteController::active() {
    send("command");
    this->stop_recv_state = false;
    t_recv_state = std::thread(&RemoteController::recvTelloState, this);
}

void RemoteController::streamon() {
    send("streamon");
    this->stop_recv_stream = false;
    t_decode_frame = std::thread(&RemoteController::decodeFrame, this);
}

cv::Mat RemoteController::getFrame() {
    while (!frame_ready) {

    }
    return this->frame;
    frame_ready = false;
}

void RemoteController::takeoff() {
    send("takeoff");
}

void RemoteController::landing() {
    send("land");
}

void RemoteController::forward(int distance) {
    send("forward " + std::to_string(distance));
}

void RemoteController::back(int distance) {
    send("back " + std::to_string(distance));
}

void RemoteController::left(int distance) {
    send("left " + std::to_string(distance));
}

void RemoteController::right(int distance) {
    send("right " + std::to_string(distance));
}

void RemoteController::up(int distance) {
    send("up " + std::to_string(distance));
}

void RemoteController::down(int distance) {
    send("down " + std::to_string(distance));
}

void RemoteController::disconnect() {
    stop_recv_stream = true;
    stop_recv_state = true;
    t_recv_state.join();
    t_decode_frame.join();
    send("streamoff");
    close(stream_server_sock_fd);
    close(state_server_sock_fd);
    close(local_server_sock_fd);
}

void RemoteController::send(const std::string& command) {
    int len = 0;
    if ( (len = sendto(local_server_sock_fd, command.c_str(), command.length(), 0, reinterpret_cast<sockaddr*>(&tello_server), sizeof(sockaddr))) < 0) {
        std::cout << "command send error!" << std::endl;
        throw RCError::COMMAND_SEND_ERROR;
    }
    u_char buffer[1024];
    sockaddr_in addr_tello;
    len = sizeof(sockaddr);
    int recv_num = recvfrom(local_server_sock_fd, buffer, 10000, 0, reinterpret_cast<sockaddr *>(&addr_tello),
                            reinterpret_cast<socklen_t *>(&len));
    char temp[recv_num];
    for (int i = 0; i < recv_num; i++) {
        std::cout << (char)buffer[i];
    }
    std::cout << std::endl;
}

void RemoteController::init () {
    createSocket();
    bindPort();
    setTelloServer();
}

void RemoteController::createSocket () {
    stream_server_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    state_server_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    local_server_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (stream_server_sock_fd < 0 || state_server_sock_fd < 0) {
        std::cout << "create socket error!" << std::endl;
        throw RCError::CREATE_SOCKET_ERROR;
    }
}

void RemoteController::bindPort () {
    setStreamServer();
    setStateServer();

    if(bind(stream_server_sock_fd, reinterpret_cast<struct sockaddr *>(&this->stream_server), sizeof(sockaddr)) < 0
            || bind(state_server_sock_fd, reinterpret_cast<struct sockaddr *>(&this->state_server), sizeof(sockaddr)) < 0) {
        std::cout << "bind port error!" << std::endl;
        throw RCError::BIND_PORT_ERROR;
    }
}

void RemoteController::setSockAddr (sockaddr_in &addr, const std::string& ip, int port) {
    bzero(&addr, sizeof(sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
}

void RemoteController::setStreamServer () {
    stream_server.sin_family = AF_INET;
    stream_server.sin_addr.s_addr = htonl(INADDR_ANY);
    stream_server.sin_port = htons(11111);
}

void RemoteController::setStateServer () {
    state_server.sin_family = AF_INET;
    state_server.sin_addr.s_addr = htonl(INADDR_ANY);
    state_server.sin_port = htons(8890);
}

void RemoteController::setTelloServer () {
    setSockAddr(this->tello_server, REMOTE_IP, REMOTE_PORT);
}

void RemoteController::decodeFrame () {
    u_char buffer[1024];
    int recv_num = 0;
    H264Decoder decoder {};
    sockaddr_in addr_tello;
    int len = sizeof(sockaddr);
    while (!stop_recv_stream) {
        recv_num = recvfrom(stream_server_sock_fd, buffer, 2048, 0, reinterpret_cast<sockaddr *>(&addr_tello),
                            reinterpret_cast<socklen_t *>(&len));
        int consumed_num = 0;
        while (true) {
            consumed_num += decoder.parse(buffer + consumed_num, recv_num - consumed_num);
            if (decoder.is_frame_available()) {
                try {
                    AVFrame avframe = decoder.decode_frame();
                    ConverterRGB24 converter {};
                    int out_rgb_size = converter.predict_size(avframe.width, avframe.height);
                    u_char out_rgb[out_rgb_size];
                    AVFrame rgbframe = converter.convert(avframe, out_rgb);
                    cv::Mat cvframe{avframe.height, avframe.width, CV_8UC3, out_rgb, size_t(rgbframe.linesize[0])};
                    cv::cvtColor(cvframe, this->frame, cv::COLOR_RGB2BGR);
                    frame_ready = true;
                } catch (...) {
                    // noting to do
                }
            }

            if (consumed_num == recv_num) {
                break;
            }
        }
    }
}

void RemoteController::recvTelloState () {
    u_char buffer[1024];
    sockaddr_in addr_tello;
    int len = sizeof(sockaddr);
    while (!stop_recv_state) {
        int recv_num = recvfrom(state_server_sock_fd, buffer, 2048, 0, reinterpret_cast<sockaddr *>(&addr_tello),
                                reinterpret_cast<socklen_t *>(&len));
        char temp[recv_num];
        for (int i = 0; i < recv_num; i++) {
            temp[i] = (char)buffer[i];
        }
        std::string state{temp};
    }
}