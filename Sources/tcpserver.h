#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "TcpSocket/tcpserversocket.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <string>
#include <queue>

class TCPServer
{
public:
	TCPServer();
    ~TCPServer();
    void Start(const uint16_t port);
    void Stop();
    bool IsOpen();
    void Loop();
    void Send(const std::string msg);
    std::string Recv();
    int Port() const;
    bool IsDataAvailable() const;
    bool IsClientPresent() const;
private:
    void LoopForClients();
    void ReceiveDataFromClients();
    void SendDataToClients();
    void CloseAllClients();
    void AddToInputBuffer(std::string msg);
    std::string GetFromInputBuffer();
    void AddToOutputBuffer(std::string msg);
    std::string GetFromOutputBuffer();

    uint16_t port;
    std::queue<std::string> inputStringBuffer;
    std::mutex inputStringBufferMutex;
    std::queue<std::string> outputStringBuffer;
    std::mutex outputStringBufferMutex;
    std::vector<std::shared_ptr<TcpClientSocket>> tcpClients;
    std::mutex tcpClientsMutex;

    TcpServerSocket serverSocket;
    std::atomic<bool> isServerReadyToListen;

    static void server_thread_procedure(TCPServer* obj);
    std::atomic<bool> server_thread_flag_stop;
    std::thread* server_thread;

    static void clients_thread_procedure(TCPServer* obj);
    std::thread* clients_thread;
};

#endif // TCPSERVER_H
