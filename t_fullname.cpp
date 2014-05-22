/*! \file t_fullname.cpp
 * \brief
 */

#include <iostream>
#include <string>
#include "fullname.hpp"

using std::cout;
using std::endl;

int main(int argc, const char * argv[], const char * envp[])
{
    int i;
    int ret;
    char fullname[1024];
    for (i = 1; i < argc; ++i) {
        ret = get_full_user_name(argv[i], fullname, sizeof fullname);
        if (ret == -1)
            printf("ERROR: %s: %s\n", argv[i], strerror(errno));
        else if (ret == 0)
            printf("UNKONWN: %s\n", argv[i]);
        else
            printf("%s: %s\n", argv[i], fullname);
    }
    return 0;
}
