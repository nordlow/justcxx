#include "layout.hpp"
#include "cc_features.h"

#if HAVE_CXX11_EXTERN_TEMPLATES
template pure std::vector<box<float,2> > uhg_layout(const box<float,2>& b, size_t n, float p = 0.0);
template pure std::vector<box<float,2> > uvg_layout(const box<float,2>& b, size_t n, float p = 0.0);
template pure std::vector<box<float,2> > urg_layout(const box<float,2>& b, size_t n, size_t n0, size_t n1, float p = 0.0);
template pure std::vector<box<float,2> > usg_layout(const box<float,2>& b, size_t n, float p = 0.0);

template pure std::vector<box<double,2> > uhg_layout(const box<double,2>& b, size_t n, double p = 0.0);
template pure std::vector<box<double,2> > uvg_layout(const box<double,2>& b, size_t n, double p = 0.0);
template pure std::vector<box<double,2> > urg_layout(const box<double,2>& b, size_t n, size_t n0, size_t n1, double p = 0.0);
template pure std::vector<box<double,2> > usg_layout(const box<double,2>& b, size_t n, double p = 0.0);

template pure std::vector<box<float,3> > uhg_layout(const box<float,3>& b, size_t n, float p = 0.0);
template pure std::vector<box<float,3> > uvg_layout(const box<float,3>& b, size_t n, float p = 0.0);
template pure std::vector<box<float,3> > urg_layout(const box<float,3>& b, size_t n, size_t n0, size_t n1, float p = 0.0);
template pure std::vector<box<float,3> > usg_layout(const box<float,3>& b, size_t n, float p = 0.0);

template pure std::vector<box<double,3> > uhg_layout(const box<double,3>& b, size_t n, double p = 0.0);
template pure std::vector<box<double,3> > uvg_layout(const box<double,3>& b, size_t n, double p = 0.0);
template pure std::vector<box<double,3> > urg_layout(const box<double,3>& b, size_t n, size_t n0, size_t n1, double p = 0.0);
template pure std::vector<box<double,3> > usg_layout(const box<double,3>& b, size_t n, double p = 0.0);
#endif
