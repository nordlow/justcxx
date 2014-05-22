/*!
 * \file utf8test.cpp
 *
 * Any C++ compiler made in the last 5 or so years should be able to build this.
 *
 * The program when compiled, accepts a filename as command line
 * argument. This file is read in as UTF-8, and stored internally
 * as UCS-4 (using std::wstring which uses wchar_t.)
 * Any UTF-8 compatible file may be used. During development the
 * author used the following file:
 * http:*www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-demo.txt
 *
 * \see http://stackoverflow.com/questions/7889032/utf-8-compliant-iostreams
 */

#include <iostream>
#include <locale>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cerrno>
#include <cstring>
#include <algorithm>
#include <algorithm_x.hpp>

namespace
{

std::vector<std::wstring> all_lines;

std::locale sv_SE("sv_SE.utf8");  // Any UTF-8 locale would do I guess
typedef std::codecvt<wchar_t, char, std::mbstate_t> utf8_codecvt_t;
const utf8_codecvt_t &utf8_codecvt = std::use_facet<utf8_codecvt_t>(sv_SE);
std::locale utf8_locale(sv_SE, &utf8_codecvt);

// Lines may not be longer than 512 bytes
// (which can be less than 512 characters)
const int max_input_line_length = 512;

/* Read a file in UTF-8, and convert for internal storage in UCS-4 (wchar_t). */
void read_file(const char *file_name, std::vector<std::wstring> &input)
{
    size_t total_length = 0;

    std::wifstream ifs(file_name);
    if (ifs.is_open())
    {
        // This line is the magic that converts UTF-8 to UCS-4
        ifs.imbue(utf8_locale);

        wchar_t line[max_input_line_length];

        // -1 to have place for string terminator
        while (ifs.getline(line, max_input_line_length - 1))
        {
            input.push_back(line);
            total_length += std::wcslen(line);
        }

        if (not ifs.eof())
            std::cout << "Error reading input file: " << std::strerror(errno) << "\n";

        ifs.close();
    }
    else
        std::cout << "Could not open input file: " << std::strerror(errno) << "\n";

    std::cout << "Read total " << input.size() << " lines, totaling "
              << total_length << " characters ("
              << total_length * sizeof(std::wstring::value_type) << " bytes)\n";
}

/*! Convert a Wide-Character String (made of wchar_t) into an UTF-8 string. */
void wstring_to_utf8(const std::wstring &input, std::string &output)
{
    const auto inlen = input.length() + 1;  // +1 for terminating '\0'
    const auto in = input.c_str();
    auto outlen = inlen;
    char *out = nullptr;
    std::mbstate_t mbstate = { 0 };

    output.clear();

    for (;;)
    {
        if (out != nullptr)
            delete [] out;

        if ((out = new char[outlen]) == 0)
            break;

        const wchar_t *wp = in;
        char *p = out;
        utf8_codecvt_t::result res;

        // This is the
        res = utf8_codecvt.out(mbstate,
                               in, in + inlen, wp,
                               out, out + outlen, p);

        if (res == std::codecvt<wchar_t, char, mbstate_t>::ok)
            break;  // All ok
        else if (res == std::codecvt<wchar_t, char, mbstate_t>::noconv)
            break;  // No conversion made, straight copy
        else if (res == std::codecvt<wchar_t, char, mbstate_t>::partial)
            outlen *= 2;  // Not all converted, increase size and try again
        else
        {
            // Error
            if (out != nullptr)
            {
                delete [] out;
                out = nullptr;
            }
            break;
        }
    }

    if (out)
    {
        output = out;
        delete [] out;
    }
}

class file_output_writer
{
public:
    file_output_writer(std::ostream &os) : m_output(os) { }
    void operator()(const std::wstring &str)
    {
        if (str.length() > 0)
        {
            std::string outstr;
            wstring_to_utf8(str, outstr);
            m_output << outstr << "\n";
        }
        else
            m_output << "\n";
    }
private:
    std::ostream &m_output;
};

void write_file(std::ostream &os, const std::vector<std::wstring> &output)
{
    std::for_each(output, file_output_writer(os));
}
}

int main(int argc, char *argv[])
{
#ifdef __STDC_ISO_10646__
	std::cout << "__STDC_ISO_10646__ is defined to " <<__STDC_ISO_10646__ << "\n";
#else
	std::cout << "__STDC_ISO_10646__ is not defined\n";
#endif

	if (argc < 1+1)
	{
		std::cout << "Usage: " << argv[0] << " <filename>\n";
		return 1;
	}

	std::cout << "\n"
			  << "Test checking reading and writing UTF-8 files,\n"
			  << "while storing in memory as UCS-4 (wchar_t).\n"
			  << "----------------------------------------------\n";

	read_file(argv[1], all_lines);
	write_file(std::cout, all_lines);
}
