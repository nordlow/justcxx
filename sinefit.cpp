#include "sinefit.hpp"

#if HAVE_CXX11_EXTERN_TEMPLATES
namespace arma {
template Mat<float> sinefit3par(const Col<float>& y,
                                const Col<float>& wt);
template Mat<double> sinefit3par(const Col<double>& y,
                                 const Col<double>& wt);
}
#endif
