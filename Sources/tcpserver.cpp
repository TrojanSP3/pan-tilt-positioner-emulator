#include "tcpserver.h"

#include "log.h"
#include "config.h"
#include "utils.h"

const std::string LOGMODULE="TCPServer";

const int TIMEOUT_MS=5000;
const int TIMEOUT_INTERVAL=100;
const int TIMEOUT_ITERATIONS=TIMEOUT_MS/TIMEOUT_INTERVAL;

void TCPServer::Start(const int port)
{
    Stop();
    this->port=port;
    server_thread_flag_stop.store(false);
    isServerReadyToListen.store(false);

    server_thread=new std::thread(TCPServer::server_thread_procedure, this);

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
    if(server_thread!=nullptr)
    {
        if(server_thread->joinable())
            server_thread->join();
        delete server_thread; server_thread=nullptr;
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

    TcpServerSocket server;
    TcpClientSocket* client=nullptr;

    try
    {
        server.Start(this->port);
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
        if(!server.IsListening())
        {
            LOG.WriteError(LOGMODULE, LOGNAME, "Server closed");
            break;
        }

        if( (client!=nullptr) && (!client->IsOpen()) )
        {
            isClientPresent.store(false);
            delete client;
            client=nullptr;
        }

        if(server.IsClientAvailable())
        {
            TcpClientSocket* possible_client = server.AcceptClient();

            if( (possible_client!=nullptr) && (possible_client->IsOpen()))
            {
                if(isClientPresent.load())
                {
                    LOG.WriteInfo(LOGMODULE, LOGNAME, "Dropping client...");
                    possible_client->Close();
                    delete possible_client;
                    possible_client=nullptr;
                }
                else
                {
                    LOG.WriteInfo(LOGMODULE, LOGNAME, "Accepting client...");
                    if( client!=nullptr )
                    {
                        client->Close();
                        delete client;
                    }
                    client=possible_client;
                    if(client_thread!=nullptr)
                    {
                        client_thread->join();
                        delete client_thread;
                        client_thread=nullptr;
                    }

                    client_thread = new std::thread(client_thread_procedure, this, client);
                    isClientPresent.store(true);
                }
            }
        }
        Utils::Sleep(100);
    }
    isServerReadyToListen.store(false);

    if( client!=nullptr )
    {
        client->Close();
        delete client;
        isClientPresent.store(false);
    }

    server.Stop();
    LOG.WriteInfo(LOGMODULE, LOGNAME, "Successfully ended");
}

void TCPServer::client_thread_procedure(TCPServer* obj, TcpClientSocket* client)
{
    const std::string LOGNAME="client_thread_procedure";

    if( obj==nullptr || client==nullptr )
        return;

    try
    {
        Utils::SetThreadName("tcp_client");
        obj->ProcessConnection(client);
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

void TCPServer::ProcessConnection(TcpClientSocket* client)
{
    const std::string LOGNAME="ProcessConnection";
    int id_client=client->Id();
    std::string log_client="["+std::to_string(id_client)+"] ";
    LOG.WriteInfo(LOGMODULE,LOGNAME,log_client+"Connected");

    while(!server_thread_flag_stop.load())
    {

        if(!client->IsOpen())
        {
            LOG.WriteInfo(LOGMODULE,LOGNAME,log_client+"Client disconnected");
            break;
        }

        while(client->BytesAvailable())
        {
            std::string data=client->ReadLine();
            LOG.WriteDebug(LOGMODULE,LOGNAME,log_client+"Data from client: "+data);
            TCPServer::AddToInputBuffer(data);
            Utils::Sleep(10);
        }

        while(TCPServer::outputStringBuffer.size())
        {
            std::string data=TCPServer::GetFromOutputBuffer();
            if(!data.empty())
            {
                LOG.WriteDebug(LOGMODULE,LOGNAME,log_client+"Data from server: "+data);
                client->WriteLine(data);
            }
            Utils::Sleep(10);
        }

        Utils::Sleep(10);
    }
    client->Close();
    LOG.WriteInfo(LOGMODULE,LOGNAME,log_client+"Ending");

}

void TCPServer::Send(const std::string msg)
{
    if(isClientPresent.load())
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
    if(outputStringBuffer.size())
    {
        result=outputStringBuffer.front();
        outputStringBuffer.pop();
    }
    outputStringBufferMutex.unlock();
    return result;
}

bool TCPServer::IsClientPresent() const
{
    return isClientPresent.load();
}
