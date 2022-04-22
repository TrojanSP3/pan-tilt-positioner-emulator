#include "tcpserver.h"

#include "log.h"
#include "config.h"
#include "utils.h"

const std::string LOGMODULE="TCPServer";

const int TIMEOUT_MS=5000;
const int TIMEOUT_INTERVAL=100;
const int TIMEOUT_ITERATIONS=TIMEOUT_MS/TIMEOUT_INTERVAL;

TCPServer::~TCPServer()
{
    Stop();


}

void TCPServer::Start(const uint16_t port)
{
    Stop();
    this->port=port;
    server_thread_flag_stop.store(false);
    isServerReadyToListen.store(false);

    server_thread=new std::thread(TCPServer::server_thread_procedure, this);
    clients_thread = new std::thread(clients_thread_procedure, this);

    int cnt=0;
    while(!isServerReadyToListen.load())
    {
        ++cnt;
        if(cnt>TIMEOUT_ITERATIONS)
            break;
        Utils::Sleep(TIMEOUT_INTERVAL);
    }

    if(!isServerReadyToListen.load())
    {
        throw std::runtime_error("Start failed");
    }
}

void TCPServer::Stop()
{
    server_thread_flag_stop.store(true);
    serverSocket.Stop();
    if(server_thread!=nullptr)
    {
        if(server_thread->joinable())
            server_thread->join();
        delete server_thread; server_thread=nullptr;
    }
    if(clients_thread!=nullptr)
    {
        if(clients_thread->joinable())
            clients_thread->join();
        delete clients_thread; clients_thread=nullptr;
    }
    port=0;
}

bool TCPServer::IsOpen()
{
    return isServerReadyToListen.load();
}

void TCPServer::server_thread_procedure(TCPServer* obj)
{
    const std::string LOGNAME="server_thread_procedure";
    if(obj==nullptr)
        return;
    try
    { 
        Utils::SetThreadName("tcp_server");
        obj->Loop();
    }
    catch(std::exception& ex)
    {
        std::string msg = std::string("TcpServerThreadException: ")+ex.what();
        LOG.WriteCrit(LOGMODULE,LOGNAME,msg);
    }
    catch(...)
    {
        std::string msg = "TcpServerThreadException";
        LOG.WriteCrit(LOGMODULE,LOGNAME,msg);
    }
}

void TCPServer::Loop()
{
    const std::string LOGNAME="Loop";
    LOG.WriteInfo(LOGMODULE, LOGNAME, "Starting server");

    try
    {
        serverSocket.Start(this->port);
    }
    catch(TcpSocketException& ex)
    {
        std::string msg = std::string("Error on starting server: ")+ex.what();
        LOG.WriteError(LOGMODULE, LOGNAME, msg);
        return;
    }

    isServerReadyToListen.store(true);

    while(!server_thread_flag_stop.load())
    {
        if(!serverSocket.IsListening())
        {
            LOG.WriteError(LOGMODULE, LOGNAME, "Server closed");
            break;
        }

        if(serverSocket.IsClientAvailable())
        {
            std::shared_ptr<TcpClientSocket> possible_client = serverSocket.AcceptClient();
            if( (possible_client!=nullptr) && (possible_client->IsOpen()))
            {
                tcpClientsMutex.lock();
                tcpClients.push_back(possible_client);
                tcpClientsMutex.unlock();

                LOG.WriteInfo(LOGMODULE, LOGNAME,
                              "New client ["+std::to_string(possible_client->Id())+"]");
            }
        }
    }
    isServerReadyToListen.store(false);

    serverSocket.Stop();
    LOG.WriteInfo(LOGMODULE, LOGNAME, "Successfully ended");
}

void TCPServer::clients_thread_procedure(TCPServer* obj)
{
    const std::string LOGNAME="client_thread_procedure";

    if( obj==nullptr )
        return;

    try
    {
        Utils::SetThreadName("tcp_clients");
        obj->LoopForClients();
    }
    catch(std::exception& ex)
    {
        std::string msg = std::string("TcpClientThreadException: ")+ex.what();
        LOG.WriteCrit(LOGMODULE,LOGNAME,msg);
    }
    catch(...)
    {
        std::string msg = "TcpClientThreadException";
        LOG.WriteCrit(LOGMODULE,LOGNAME,msg);
    }

}

void TCPServer::LoopForClients()
{
    const std::string LOGNAME="LoopForClients";

    while(!server_thread_flag_stop.load())
    {
        ReceiveDataFromClients();
        SendDataToClients();
        Utils::Sleep(10);
    }

    CloseAllClients();
}

void TCPServer::ReceiveDataFromClients()
{
    const std::string LOGNAME="ReceiveDataFromClients";
    if(!tcpClients.empty())
    {
        const std::lock_guard<std::mutex> lock(tcpClientsMutex);
        for(size_t i=0; i<tcpClients.size(); ++i)
        {
            std::shared_ptr<TcpClientSocket> client = tcpClients[i];

            const std::string logClientId=
                    (client==nullptr? "null" : std::to_string(client->Id()));
            const std::string logClientPrefix = "["+logClientId+"] ";

            if(client==nullptr || !client->IsOpen())
            {
                LOG.WriteDebug(LOGMODULE,LOGNAME,
                               logClientPrefix+"Client disconnected");
                tcpClients.erase(tcpClients.begin()+static_cast<std::ptrdiff_t>(i));
                --i;
            }
            else
            {
                while(client->BytesAvailable())
                {
                    std::string data=client->ReadLine();
                    LOG.WriteDebug(LOGMODULE,LOGNAME,logClientPrefix+"Data: "+data);
                    TCPServer::AddToInputBuffer(data);
                }
            }
        }
    }
}

void TCPServer::SendDataToClients()
{
    const std::string LOGNAME="SendDataToClients";
    while(TCPServer::outputStringBuffer.size())
    {
        std::string data=TCPServer::GetFromOutputBuffer();
        if(!data.empty())
        {
            LOG.WriteDebug(LOGMODULE,LOGNAME,"Data: "+data);
            const std::lock_guard<std::mutex> lock(tcpClientsMutex);
            for(size_t i=0; i<tcpClients.size(); ++i)
            {
                std::shared_ptr<TcpClientSocket> client = tcpClients[i];
                if(client!=nullptr && client->IsOpen())
                    client->WriteLine(data);
            }
        }
        Utils::Sleep(10);
    }
}

void TCPServer::CloseAllClients()
{
    const std::lock_guard<std::mutex> lock(tcpClientsMutex);
    for(size_t i=0; i<tcpClients.size(); ++i)
    {
        if(tcpClients[i]!=nullptr)
            tcpClients[i]->Close();
    }
    tcpClients.clear();
}

void TCPServer::Send(const std::string msg)
{
    if(IsClientPresent())
        TCPServer::AddToOutputBuffer(msg);
    else
    {
        outputStringBufferMutex.lock();
        while(outputStringBuffer.size()>0)
            outputStringBuffer.pop();
        outputStringBufferMutex.unlock();
    }
}

std::string TCPServer::Recv()
{
    return GetFromInputBuffer();
}

int TCPServer::Port() const
{
    return port;
}

bool TCPServer::IsDataAvailable() const
{
   return inputStringBuffer.size()>0;
}

void TCPServer::AddToInputBuffer(std::string msg)
{
    inputStringBufferMutex.lock();
    while(inputStringBuffer.size()>=CONFIG::TCP::MAXBUFFERSIZE.Get())
    {
        inputStringBuffer.pop();
    }
    inputStringBuffer.push(msg);
    inputStringBufferMutex.unlock();
}

std::string TCPServer::GetFromInputBuffer()
{
    inputStringBufferMutex.lock();
    std::string result = "";
    if(inputStringBuffer.size())
    {
        result=inputStringBuffer.front();
        inputStringBuffer.pop();
    }
    inputStringBufferMutex.unlock();
    return result;
}

void TCPServer::AddToOutputBuffer(std::string msg)
{
    outputStringBufferMutex.lock();
    while(outputStringBuffer.size()>=CONFIG::TCP::MAXBUFFERSIZE.Get())
    {
        outputStringBuffer.pop();
    }
    outputStringBuffer.push(msg);
    outputStringBufferMutex.unlock();
}

std::string TCPServer::GetFromOutputBuffer()
{
    outputStringBufferMutex.lock();
    std::string result = "";
    if(outputStringBuffer.size()>0)
    {
        result=outputStringBuffer.front();
        outputStringBuffer.pop();
    }
    outputStringBufferMutex.unlock();
    return result;
}

bool TCPServer::IsClientPresent() const
{
    return tcpClients.size()>0;
}
