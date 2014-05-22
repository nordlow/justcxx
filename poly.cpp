#include "poly.hpp"

#if HAVE_CXX11_EXTERN_TEMPLATES
namespace arma {
template pure Col<float> polyfit<float>(const Col<float>& y,
                                        const Col<float>& x,
                                        size_t m,
                                        const Col<float>& w = Col<float>(),
                                        bool slow = false);
template pure Col<double> polyfit<double>(const Col<double>& y,
                                          const Col<double>& x,
                                          size_t m,
                                          const Col<double>& w = Col<double>(),
                                          bool slow = false);
template pure Col<float> linear_polyfit<float>(const Col<float>& y,
                                               const Col<float>& x);
template pure Col<double> linear_polyfit<double>(const Col<double>& y,
                                                 const Col<double>& x);
}
#endif
