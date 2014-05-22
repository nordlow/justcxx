#pragma once

template<class T>
void unpack(const T & src,
           T & a, size_t anum,
           T & b, size_t bnum)
{
    size_t sbc = src.size() / (anum + bnum); // source block count

    a.resize(anum * sbc);
    b.resize(bnum * sbc);

    typename T::const_iterator si = begin(src); // source iterator
    auto ai = begin(a);
    auto bi = begin(b);

    for (size_t i = 0; i < sbc; i++)
    {
        for (size_t j = 0; j < anum; j++)
        {
            *(ai++) = *(si++);
        }
        for (size_t j = 0; j < bnum; j++)
        {
            *(bi++) = *(si++);
        }
    }

}
