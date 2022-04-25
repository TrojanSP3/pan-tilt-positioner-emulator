#ifndef CROSSPLATFORM_H
#define CROSSPLATFORM_H

#ifdef _WIN32
	#define WINDOWS_PLATFORM
#else
	#define LINUX_PLATFORM
#endif

void PlatformInitialization();

#ifdef WINDOWS_PLATFORM
	#pragma comment(lib,"Ws2_32.lib")
	#include <stdint.h>
	#define NOEXCEPT
	typedef int64_t ssize_t;
	typedef unsigned long in_addr_t;
	typedef int socklen_t;
	#define SHUT_RDWR SD_BOTH
	#define FILE_PATH_SEPARATOR_CHAR ('\\')
    #define ERROR_NO_ENDPOINT WSAENOTCONN
	#define CHANGE_DIRECTORY_FUNCTION _chdir
	#define STD_ISSPACE_FUNCTION isspace
#else
	#define NOEXCEPT noexcept
	#define FILE_PATH_SEPARATOR_CHAR ('/')
    #define ERROR_NO_ENDPOINT 107
	#define CHANGE_DIRECTORY_FUNCTION chdir
	#define STD_ISSPACE_FUNCTION std::isspace
#endif

#endif //CROSSPLATFORM_H
