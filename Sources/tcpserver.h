#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpSocket/tcpserversocket.h"

#include <string>
#include <queue>
#include <atomic>
#include <thread>

class TCPServer
{
public:
    ~TCPServer();
    void Start(const int port);
    void Stop();
    bool IsOpen();
    void Loop();
    void Send(const std::string msg);
    std::string Recv();
    int Port() const;
    bool IsDataAvailable() const;
    bool IsClientPresent() const;
private:
    void ProcessConnection(TcpClientSocket* client);
    void AddToInputBuffer(std::string msg);
    std::string GetFromInputBuffer();
    void AddToOutputBuffer(std::string msg);
    std::string GetFromOutputBuffer();

    int port=0;
    std::queue<std::string> inputStringBuffer;
    std::mutex inputStringBufferMutex;
    std::queue<std::string> outputStringBuffer;
    std::mutex outputStringBufferMutex;

    std::atomic<bool> isClientPresent = {false};
    std::atomic<bool> isServerReadyToListen = {false};

    static void server_thread_procedure(TCPServer* obj);
    std::atomic<bool> server_thread_flag_stop;
    std::thread* server_thread=nullptr;

    static void client_thread_procedure(TCPServer* obj, TcpClientSocket* client);
    std::thread* client_thread=nullptr;
};

#endif // TCPSERVER_H
