#include "LineReader.hpp"
#include <cstdlib>

bool LineReader::into(std::string & line) const
{
    char * l = ::readline(prompt);

    if (l)
    {
        line = l;
        std::free(l);
    }
    else
    {
        line = "";
        std::cout << std::endl;
    }

    return l > 0;
}

void LineReader::print_history(std::ostream & os) const
{
    ::HIST_ENTRY **l = ::history_list();

    if (l)
    {
        for (unsigned int i = 0; l[i]; i++)
        {
            os << i + ::history_base << "  " << l[i]->line << std::endl;
        }
    }
}
