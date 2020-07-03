#ifndef DTRACE_MEMACCESS_SHARED_H
#define DTRACE_MEMACCESS_SHARED_H

#include <cstdlib>
#include <iostream>

void DumpHex(const void* data, size_t size);
void waitForEnter();

#define LOG_ERR(x) std::cerr << x << std::endl;
#define LOG(x) std::cout << x << std::endl;
#define STRERRNO() errno << " (" << strerror(errno) << ")"

#endif //DTRACE_MEMACCESS_SHARED_H
