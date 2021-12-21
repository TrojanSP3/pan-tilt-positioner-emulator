#ifndef SOCKETSTATUS_H
#define SOCKETSTATUS_H


class SocketStatus
{
public:
    SocketStatus(int _socket);
    void Update();
    bool isReadable() const;
    bool isWritable() const;
    bool isClosed() const;
    bool isError() const;
    bool isValid() const;
    int AvailableBytes() const;
protected:
    void ByPoll();
    void ByIoctl();
    void ByGetsockopt();

private:
    int socket=0;
    bool closed = false;
    bool readable = false;
    bool writable = false;
    bool errors = false;
    int bytes = 0;
};

#endif // SOCKETSTATUS_H
