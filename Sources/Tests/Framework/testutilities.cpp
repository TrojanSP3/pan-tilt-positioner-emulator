#include "testutilities.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

using namespace UnitTests;

uint16_t TestUtilities::TCP_GetFreePort()
{
    int socket_value = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket_value<=0)
    {
        throw TestException("Error on opening socket: "+std::to_string(errno),__FILE__,__LINE__);
    }

    struct sockaddr_in bind_address;
    bind_address.sin_family = AF_INET;
    bind_address.sin_port = htons( 0 );
    bind_address.sin_addr.s_addr = INADDR_ANY;
    struct sockaddr *bind_address_ptr = reinterpret_cast<struct sockaddr *>(&bind_address);
    socklen_t bind_address_len = sizeof(bind_address);
    int not_binded = bind(socket_value, bind_address_ptr, bind_address_len);
    if(not_binded)
    {
        close(socket_value);
        throw TestException("Error on binding socket: "+std::to_string(errno),__FILE__,__LINE__);
    }

    struct sockaddr_in storage_address;
    struct sockaddr *storage_address_ptr = reinterpret_cast<struct sockaddr *>(&storage_address);
    socklen_t storage_address_len = sizeof(storage_address);
    int not_named = getsockname(socket_value, storage_address_ptr, &storage_address_len);
    if(not_named)
    {
        close(socket_value);
        throw TestException("Error on get socket: "+std::to_string(errno),__FILE__,__LINE__);
    }

    uint16_t result = ntohs(storage_address.sin_port);
    close(socket_value);

    return result;
}
