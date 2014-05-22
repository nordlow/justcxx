#include <vector>
#include <iostream>

// ============================================================================

template<typename T>
class Map
{
public:
    Map(unsigned int m, unsigned int n);
    const T & operator () (unsigned int i, unsigned int j) const;
    T & operator () (unsigned int i, unsigned int j);
    void print(std::ostream & os) const;
private:
    unsigned int m, n;
    std::vector<T> gmap;
};

template<typename T>
Map<T>::Map(unsigned int m, unsigned int n)
    : m(m), n(n), gmap(m*n)
{
}

template<typename T>
inline const T & Map<T>::operator() (unsigned int i, unsigned int j) const
{
    return gmap[i * n + j];
}

template<typename T>
inline T & Map<T>::operator() (unsigned int i, unsigned int j)
{
    return gmap[i * n + j];
}

template<typename T>
void Map<T>::print(std::ostream & os) const
{
    for (unsigned int i = 0; i < m; i++)
    {
        for (unsigned int j = 0; j < n; j++)
        {
            os << (*this)(i, j) << " ";
        }
        os << std::endl;
    }
}

template<typename T>
std::ostream & operator << (std::ostream & os, const Map<T> & a)
{
    a.print(os);
    return os;
}

// ============================================================================

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    Map<PixRGB24> m(4,4);
    m(0,0) += PixRGB24(3);
    m(0,0) += PixRGB24(2e9);
    m(0,0) += PixRGB24(2e9);
    m(0,0) += PixRGB24(2e9);
    cout << m << endl;
    return 0;
}
