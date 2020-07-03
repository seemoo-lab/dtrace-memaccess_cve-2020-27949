extern "C" {
#include <unistd.h>
}

#include "shared.h"


int main(int argc, char **argv) {
    char password[256] = {};

    LOG("This is the victim application.");
    LOG("We have PID: " << getpid());
    LOG("password buffer is at " << std::hex << reinterpret_cast<void *>(password) << std::dec);

    LOG("Type in a password:");
    std::cin >> password;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    LOG("Password contents:")
    DumpHex(password, 256);

    LOG("Start 'memaccess' and wait for instructions to resume the target.");
    waitForEnter();

    LOG("Password contents after probe install:")
    DumpHex(password, 256);

    LOG("Please stop the DTrace script now, then continue this application.");
    waitForEnter();

    LOG("Password contents after probe uninstall:")
    DumpHex(password, 256);
    return 0;
}
