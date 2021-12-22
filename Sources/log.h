#ifndef LOG_H
#define LOG_H

#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <fstream>

class Log;
extern Log LOG;

class Log
{
    class LogFile;
public:
    ~Log();
    void Start(std::string filename, ssize_t max_file_size=10*1024*1014);
    void Stop();
    void WriteDebug(std::string module, std::string method, std::string message);
    void WriteInfo(std::string module, std::string method, std::string message);
    void WriteError(std::string module, std::string method, std::string message);
    void WriteCrit(std::string module, std::string method, std::string message);
    void Loop();

private:
    enum TYPE {Unknown, Debug, Info, Error, Crit};
    void AddToQueue(TYPE type, std::string module, std::string method, std::string message);
    void FlushQueueToFile();

    static void log_thread_procedure(Log* obj);
    std::thread* log_thread=nullptr;
    std::atomic<bool> log_thread_flag_stop;
    std::queue<std::string> log_queue;
    std::mutex log_queue_mutex;
private:
    class LogFile
    {
    public:
        ~LogFile();
        void SetFileName(std::string name);
        void SetFileSize(ssize_t size);
        void Open();
        void Close();
        bool isOpen() const;
        void Write(std::string msg);
        void Flush();
        bool isExist();
        bool isValidFileSize();
        void CutFileProcedure();
    protected:
        void OpenFile();
        void CloseFile();
    private:
        std::fstream file;
        std::string file_name="";
        ssize_t max_file_size;
    } logFile;
};




#endif // LOG_H
