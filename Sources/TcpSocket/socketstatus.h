#ifndef SOCKETSTATUS_H
#define SOCKETSTATUS_H

class SocketStatus
{
public:
    SocketStatus(int socket);
    bool isReadable() const;
    bool isWritable() const;
    bool isClosed() const;
    bool isError() const;
    bool isValid() const;
    int AvailableBytes() const;
protected:
    void Update();
    void ByPoll();
    void ByIoctl();
    void ByGetsockopt();
private:
    int socket;
    bool closed;
    bool readable;
    bool writable;
    bool errors;
    int bytes;
};

#endif // SOCKETSTATUS_H
