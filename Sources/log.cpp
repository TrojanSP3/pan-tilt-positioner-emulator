#include "log.h"

#include <iostream>
#include <sstream>
#include <list>
#include <sys/stat.h>

#include "utils.h"

Log LOG;

const std::string TYPENAME []= {"Unknown","Debug","Info","Error","Critical"};

void Log::log_thread_procedure(Log* obj)
{
    if(obj == nullptr)
        return;
    try
    {
        Utils::SetThreadName("log");
        obj->Loop();
    }
    catch(std::exception& ex)
    {
        std::string msg = std::string("LogThreadException: ")+ex.what();
        std::cerr<<msg<<std::endl<<std::flush;
    }
    catch(...)
    {
        std::string msg = "LogThreadError";
        std::cerr<<msg<<std::endl<<std::flush;
    }
}

void Log::Start(std::string filename, ssize_t max_file_size)
{
    Stop();
    logFile.SetFileName(filename);
    logFile.SetFileSize(max_file_size);
    log_thread_flag_stop.store(false);
    log_thread = new std::thread(Log::log_thread_procedure, this);
}

void Log::Stop()
{
    log_thread_flag_stop.store(true);
    if(log_thread!=nullptr)
    {
        if(log_thread->joinable())
            log_thread->join();
        delete log_thread; log_thread=nullptr;
    }
}

Log::~Log()
{
    Stop();
}

void Log::WriteDebug(std::string module, std::string method, std::string message)
{
    Log::AddToQueue(Log::TYPE::Debug,module,method,message);
}
void Log::WriteInfo(std::string module, std::string method, std::string message)
{
    Log::AddToQueue(Log::TYPE::Info,module,method,message);
}
void Log::WriteError(std::string module, std::string method, std::string message)
{
    Log::AddToQueue(Log::TYPE::Error,module,method,message);
}
void Log::WriteCrit(std::string module, std::string method, std::string message)
{
    Log::AddToQueue(Log::TYPE::Crit,module,method,message);
}

void Log::AddToQueue(Log::TYPE type, std::string module, std::string method, std::string message)
{
    const std::string UNKNOWN = "UNKNOWN";
    std::string formatted_datetime=Utils::GetCurrentTime();
    std::string formatted_type=TYPENAME[type];
    std::string formatted_module=module.empty() ? UNKNOWN : module;
    std::string formatted_method=method.empty() ? UNKNOWN : method;
    std::string formatted_message=message.empty() ? "" : (": "+message);

    std::stringstream msg;
    msg<<formatted_datetime<<" | ["<<formatted_module<<"."<<formatted_method<<"] "
       <<formatted_type<<formatted_message<<std::endl<<std::endl;

    log_queue_mutex.lock();
    log_queue.push(msg.str());
    log_queue_mutex.unlock();
}

void Log::Loop()
{

    while(!log_thread_flag_stop.load())
    {
        if(!logFile.isOpen() || !logFile.isExist())
        {
            logFile.Open();
        }

        if(log_queue.size()>0)
        {
            FlushQueueToFile();
            if(!logFile.isValidFileSize())
                logFile.CutFileProcedure();
        }
        Utils::Sleep(10);
    }
    FlushQueueToFile();
    logFile.Close();
}



void Log::FlushQueueToFile()
{
    while(log_queue.size()>0)
    {
        std::string message = log_queue.front();
        log_queue.pop();

        if(logFile.isOpen())
        {
            logFile.Write(message);
        }
        else
        {
            std::cout<<message<<std::endl<<std::flush;
        }
    }
    logFile.Flush();
}




Log::LogFile::~LogFile()
{
    Close();
}

void Log::LogFile::SetFileName(std::string name)
{
    file_name=name;
}

void Log::LogFile::SetFileSize(ssize_t size)
{
    max_file_size=size;
}

void Log::LogFile::Open()
{
    Close();
    OpenFile();
}

void Log::LogFile::Close()
{
    if(isOpen())
    {
        CloseFile();
    }
}

bool Log::LogFile::isOpen() const
{
    return file.is_open();
}

void Log::LogFile::Write(std::string msg)
{
    file<<msg;
}

void Log::LogFile::Flush()
{
    Log::LogFile::file.flush();
}

bool Log::LogFile::isExist()
{
    struct stat buffer;
    return (stat (file_name.c_str(), &buffer) == 0);
}

bool Log::LogFile::isValidFileSize()
{
    ssize_t size = file.tellp();
    bool result = (size<=max_file_size?true:false);
    return result;
}

void Log::LogFile::OpenFile()
{
    if(!file_name.empty())
    {
        std::ios_base::openmode flags_work = std::ios_base::out | std::ios_base::app;
        file.open(file_name.c_str(), flags_work);
    }
}

void Log::LogFile::CloseFile()
{
    file.flush();
    file.close();
}

void Log::LogFile::CutFileProcedure()
{
    if(isOpen())
    {
        CloseFile();

        std::ios_base::openmode flags_read = std::ios_base::in;

        file.open(file_name.c_str(), flags_read);
        std::list<std::string> data;
        file.seekg(0,std::ios_base::beg);
        while(!file.eof())
        {
            std::string value="";
            std::getline(file, value);
            if(!value.empty())
            {
                data.push_back(value);
            }
        }
        auto tmp_it = data.begin();
        for(size_t i=0;i<data.size()/2;++i)
            ++tmp_it;
        data.erase(data.begin(),tmp_it);

        file.close();
        std::ios_base::openmode flags_clear = std::ios_base::out | std::ios_base::trunc;
        file.open(file_name.c_str(),flags_clear);
        file.close();
        OpenFile();

        for(auto it = data.begin(); it!=data.end(); it++)
        {
            file<<*it<<"\n\n";
        }
        file<<"LogFile cleared!\n\n";
        file.flush();
    }
}
