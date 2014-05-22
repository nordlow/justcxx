#include <limits>
#include <iostream>

void test_limits()
{
    using namespace std;

    cout

        << "char min = "
        << static_cast<int>(numeric_limits<char>::min())
        << endl
        << "char max = "
        << static_cast<int>(numeric_limits<char>::max())
        << endl
        << endl

        << "uchar min = "
        << static_cast<uint>(numeric_limits<uchar>::min())
        << endl
        << "uchar max = "
        << static_cast<uint>(numeric_limits<uchar>::max())
        << endl
        << endl

        << "short min = " << numeric_limits<short>::min()
        << endl
        << "short max =  " << numeric_limits<short>::max()
        << endl
        << endl

        << "ushort min = " << numeric_limits<ushort>::min()
        << endl
        << "ushort max =  " << numeric_limits<ushort>::max()
        << endl
        << endl

        << "int min = " << numeric_limits<int>::min()
        << endl
        << "int max =  " << numeric_limits<int>::max()
        << endl
        << endl

        << "uint min = " << numeric_limits<uint>::min()
        << endl
        << "uint max =  " << numeric_limits<uint>::max()
        << endl
        << endl

        << "long min = " << numeric_limits<long>::min()
        << endl
        << "long max =  " << numeric_limits<long>::max()
        << endl
        << endl

        << "ulong min = " << numeric_limits<ulong>::min()
        << endl
        << "ulong max =  " << numeric_limits<ulong>::max()
        << endl
        << endl

        << "long long min = " << numeric_limits<long long>::min()
        << endl
        << "long long max =  " << numeric_limits<long long>::max()
        << endl
        << endl

        << "unsigned long long min = "
        << numeric_limits<unsigned long long>::min() << endl
        << "unsigned long long max = "
        << numeric_limits<unsigned long long>::max() << endl
        << endl

        << "float min = "
        << numeric_limits<float>::min() << endl
        << "float max = "
        << numeric_limits<float>::max() << endl
        << endl

        << "double min = "
        << numeric_limits<double>::min() << endl
        << "double max = "
        << numeric_limits<double>::max() << endl
        << endl

        ;

    cout

        << "char sign = " << numeric_limits<char>::is_signed << endl
        << "uchar sign = "
        << numeric_limits<uchar>::is_signed << endl
        << endl;
}

int main(int argc, char *argv[])
{
    test_limits();
    return 0;
}
