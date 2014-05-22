/*!
 * \file mex_x.hpp
 * \brief Matlab Mex Extensions and Utilities.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-01-23 09:47
 */

#pragma once

#include <mex.h>
#include <algorithm>

// C++11 for different GCC Versions
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6) || !defined(__GXX_EXPERIMENTAL_CXX0X__)
#define NO_CONSTEXPR
#define NO_NULLPTR
#endif

#ifdef NO_NULLPTR
#define nullptr (NULL)
#endif

// TODO: Convert operators from double to int
// TODO: Convert printers t show functions.
// #include <show.hpp>

#if 0
static
void myprint(const char * print)
{
#ifdef _WIN32
    mexPrintf("\nThe initialized data in object:\n");
#else
    std::cout << "\nThe initialized data in object:\n";
#endif
    std::flush(std::cout);
}
#endif

/*! Get Structure C Type Name. */
inline const char * mxArrayGetTypeName(const mxArray* pa)
{
    const char * type = nullptr;
    if      (mxIsCell(pa))           { type = "Cell"; }
    else if (mxIsNumeric(pa))        { type = "Numeric"; }
    else if (mxIsLogical(pa))        { type = "Logical"; }
    else if (mxIsChar(pa))           { type = "Char"; }
    else if (mxIsStruct(pa))         { type = "Struct"; }
    else if (mxIsOpaque(pa))         { type = "Opaque"; }
    else if (mxIsFunctionHandle(pa)) { type = "Function"; }
    return type;
}

/*! Get Precision C Type Name. */
inline const char * mxArrayGetPrecisionName(const mxArray* pa)
{
    const char * prec = nullptr;
    if      (mxIsSingle(pa)) { prec = "float"; }
    else if (mxIsDouble(pa)) { prec = "double"; }
    else if (mxIsInt8(pa))   { prec = "int8_t"; }
    else if (mxIsInt16(pa))  { prec = "int16_t"; }
    else if (mxIsInt32(pa))  { prec = "int32_t"; }
    else if (mxIsInt64(pa))  { prec = "int64_t"; }
    else if (mxIsUint8(pa))  { prec = "uint8_t"; }
    else if (mxIsUint16(pa)) { prec = "uint16_t"; }
    else if (mxIsUint32(pa)) { prec = "uint32_t"; }
    else if (mxIsUint64(pa)) { prec = "uint64_t"; }
    return prec;
}

/*! Print Array \pa named as \p name. */
inline void mxArrayPrint(const mxArray* pa, const char * name = nullptr,
                         mwIndex this_row = -1,
                         mwIndex this_col = -1,
                         int depth = 0)
{
    if (pa) {
        const mwSize Nd = mxGetNumberOfDimensions(pa);
        const mwSize R = mxGetM(pa); // number of rows
        const mwSize C = mxGetN(pa); // number of columns
        const mwSize N = R * C; // number of elements
//const void * d = mxGetData(pa); // data pointer
        auto type = mxArrayGetTypeName(pa);
        auto prec = mxArrayGetPrecisionName(pa);
        if (depth) {
            mexPrintf("|");
        }
        for (int i = 0; i < depth; i++) {
            mexPrintf("-");
        }
        if (name) {
            mexPrintf("%-5s: ", name);
        }
        if (this_row >= 0 &&
            this_col >= 0) {
            mexPrintf("(%d,%d) ", this_row+1,this_col+1);
        }
        mexPrintf("%dD-%-9s %dx%d", Nd, type, R, C);
        if (R >= 2 && C >= 2) {
            mexPrintf(" (=%d)", N);
        }

        mexPrintf(" of");
        if (prec) {
            mexPrintf(" %-8s",
                      prec);
        }
        mexPrintf("\n");
        if (mxIsCell(pa)) {
            for (mwSize c = 0; c < C; c++) { // for each column
                for (mwSize r = 0; r < R; r++) { // for each row
                    mxArrayPrint(mxGetCell(pa, c*R + r), nullptr, r,c, depth+1);
                }
            }
        }
    } else {
        mexPrintf("%s: empty!\n", name ? name : "unknown");
    }
}

/*! Check if \p pa i a \em scalar.
 * \ref MATLAB's' \c isscalar()
 */
inline bool mxIsScalar(const mxArray* pa)
{
    const auto ndim = mxGetNumberOfDimensions(pa);
    const auto dims = mxGetDimensions(pa);
    return ndim and std::all_of(dims, dims+ndim, [](mwSize dim) { return dim == 1; });
    // const mwSize Nd = mxGetNumberOfDimensions(pa);
    // const mwSize R = mxGetM(pa); // number of rows
    // const mwSize C = mxGetN(pa); // number of columns
    // return (Nd == 2 &&
    //         R == 1 &&
    //         C == 1);
}

/*! Check if \p pa i a \em vector.
 * \ref MATLAB's' \c isscalar()
 */
inline bool mxIsVector(const mxArray* pa)
{
    const auto ndim = mxGetNumberOfDimensions(pa);
    const auto dims = mxGetDimensions(pa);
    return ndim and std::all_of(dims, dims+ndim, [](mwSize dim) { return dim == 1; });
    // const mwSize Nd = mxGetNumberOfDimensions(pa);
    // const mwSize R = mxGetM(pa); // number of rows
    // const mwSize C = mxGetN(pa); // number of columns
    // return (Nd == 2 &&
    //         R == 1 &&
    //         C == 1);
}

/*! Get \em Length (\em Largest Dimension) of \p pa, or 0 of empty array.
 * \ref MATLAB's' \c length()
 */
inline mwSize mxGetLength(const mxArray *pa)
{
    const auto ndim = mxGetNumberOfDimensions(pa);
    const auto dims = mxGetDimensions(pa);
    return ndim ? *std::max_element(dims, dims+ndim) : 0;
}

inline mwSize mxNumEl(const mxArray *pa) { return mxGetNumberOfElements(pa); }
inline mxArray *mxCreateDoubleRowVector(mwSize n, mxComplexity flag) { return mxCreateDoubleMatrix(1,n,flag); }
inline mxArray *mxCreateDoubleColumnVector(mwSize m, mxComplexity flag) { return mxCreateDoubleMatrix(m,1,flag); }

inline mxArray *mxCreateSingleMatrix(mwSize m, mwSize n, mxComplexity flag)
{
    const mwSize dims[2] = {m, n};
    return mxCreateNumericArray(2,
                                dims,
                                mxSINGLE_CLASS,
                                flag);
}

/*! Get real data pointer for numeric array in single precision float. */
inline float *mxGetPrf(const mxArray *pa)		/* pointer to array */
{
    return (float*)mxGetData(pa);
}

namespace matlab {

inline mwSize resize_rows(const mxArray *pa) { return mxGetM(pa); }
inline mwSize resize_cols(const mxArray *pa) { return mxGetN(pa); }

inline void resize_rows(mxArray *pa, mwSize m) { mxSetM(pa, m); }
inline void resize_cols(mxArray *pa, mwSize n) { mxSetN(pa, n); }

inline mwSize length(const mxArray *pa) { return mxGetLength(pa); }
inline mwSize numel(const mxArray *pa) { return mxGetNumberOfElements(pa); }
inline mwSize ndims(const mxArray *pa) { return mxGetNumberOfDimensions(pa); }
inline int eval(const char *str) { return mexEvalString(str); } /* matlab command string */
inline mxArray* eval_with_trap(const char *str) { return mexEvalStringWithTrap(str); } /* matlab command string */
inline bool isglobal(const mxArray *pA) { return mexIsGlobal(pA); }

inline int set(double handle, const char *property, mxArray *value) { return mexSet(handle, property, value); }
inline const mxArray *get(double handle, const char *property) { return mexGet(handle, property); }

inline bool empty(const mxArray *pa) { return mxIsEmpty(pa); }
inline bool isempty(const mxArray *pa) { return mxIsEmpty(pa); }

inline bool issparse(const mxArray *pa) { return mxIsSparse(pa); }

// inline bool isimag(const mxArray *pa) { return mxIsImag(pa); }
inline bool iscomplex(const mxArray *pa) { return mxIsComplex(pa); }
inline bool isreal(const mxArray *pa) { return not iscomplex(pa); }

inline bool isscalar(const mxArray *pa) { return mxIsScalar(pa); }
inline bool isvector(const mxArray *pa) { return mxIsVector(pa); }
// inline bool isrow(const mxArray *pa) { return mxIsRow(pa); }
// inline bool iscolumn(const mxArray *pa) { return mxIsColumn(pa); }
// inline bool ismatrix(const mxArray *pa) { return mxIsMatrix(pa); }

inline bool isobject(const mxArray *pa) { return mxIsObject(pa); }
inline bool isstruct(const mxArray *pa) { return mxIsStruct(pa); }
inline bool iscell(const mxArray *pa) { return mxIsCell(pa); }
inline bool isnumeric(const mxArray *pa) { return mxIsNumeric(pa); }
inline bool islogical(const mxArray *pa) { return mxIsLogical(pa); }
inline bool ischar(const mxArray *pa) { return mxIsChar(pa); }
inline bool issingle(const mxArray *pa) { return mxIsSingle(pa); }
inline bool isdouble(const mxArray *pa) { return mxIsDouble(pa); }
inline bool isfunctionhandle(const mxArray *pa) { return mxIsFunctionHandle(pa); }
inline bool isfloat(const mxArray *pa) { return issingle(pa) or isdouble(pa); }

inline bool isint8(const mxArray *pa) { return mxIsInt8(pa); }
inline bool isint16(const mxArray *pa) { return mxIsInt16(pa); }
inline bool isint32(const mxArray *pa) { return mxIsInt32(pa); }
inline bool isint64(const mxArray *pa) { return mxIsInt64(pa); }

inline bool isuint8(const mxArray *pa) { return mxIsUint8(pa); }
inline bool isuint16(const mxArray *pa) { return mxIsUint16(pa); }
inline bool isuint32(const mxArray *pa) { return mxIsUint32(pa); }
inline bool isuint64(const mxArray *pa) { return mxIsUint64(pa); }
inline bool isinteger(const mxArray *pa) { return (isint8(pa) or isint16(pa) or isint32(pa) or isint64(pa) or
                                                   isuint8(pa) or isuint16(pa) or isuint32(pa) or isuint64(pa)); }

// inline bool isfinite(const mxArray *pa) { return mxIsFinite(pa); }
// inline bool isinf(const mxArray *pa) { return mxIsInf(pa); }
// inline bool isnan(const mxArray *pa) { return mxIsNan(pa); }

// inline bool size(const mxArray *pa) { return mxSize(pa); }

inline mxArray *clone(const mxArray *in) { return mxDuplicateArray(in); }

/*! Place a copy of the array value of \p parray into the specified \p workspace
 *  with the specified \p name.
 */
inline int put_variable(const char *workspace,
                        const char *name,
                        const mxArray *parray) { /* matrix to copy */
    return mexPutVariable(workspace, name, parray);
}

inline int call(int		nlhs,			/* number of expected outputs */
                mxArray	*plhs[],		/* pointer array to outputs */
                int		nrhs,			/* number of inputs */
                mxArray	*prhs[],		/* pointer array to inputs */
                const char	*fcn_name   /* name of function to execute */
                ) {
    return mexCallMATLAB(nlhs, plhs, nrhs, prhs, fcn_name);
}

}
