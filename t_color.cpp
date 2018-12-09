#include <stdio.h>
#include <iostream>
#include <limits>

#include "color.hpp"
#include "color_constants.hpp"
#include "saturate.hpp"
#include "parcmpn.h"
#include "stdio_x.h"
#include "rangerand.h"

using std::cout;
using std::cerr;
using std::endl;

// \todo USE GNU GMP mpz_t to verify results

template<typename T>
int test_sadd(bool show_flag = false)
{
    int ret = 0;                // status
    for (uint i = 0; i < 8 * sizeof(T); i++)
    {
        T a = (T)1 << i;        // 2-power i
        T y = sadd(a, a);
        cout << "a:" << static_cast<double>(a) << " "
             << "sadd(a,a):" << static_cast<double>(y) << " "
             << "(a+a):" << static_cast<double>(a+a) << " ";
        // 1. OK (MIN)
        // 2. OK (MAX)
        // 3. SATURATE
        // 4. FAILURE
        if (y >= a) { cout << "SUCCESS"; }
        else { cout << "FAILURE"; ret = -1; break; }
        if (y == pnw::max_of<T>()) { cout << "(MAX) "; }
        cout << endl;
    }
    endline();
    return ret;
}

template<typename T>
int test_ssub(bool show_flag = false)
{
    int ret = 0;                // status
    for (uint i = 1; i < 8 * sizeof(T); i++)
    {
        T a = pnw::max_of<T>() / 4;      // one fourth of maximum
        T b = (T)1 << i;        // 2-power i
        T y = ssub(a,b);
        cout << "a:" << a << " "
             << "b:" << b << " "
             << "ssub(a,b):" << y << " "
             << "a-b:" << (double)a-b << " ";
        if (y == pnw::max_of<T>()) { cout << "(MAX) "; }
        else if (y <= a) { cout << "SUCCESS\n"; }
        else { cout << "FAILURE\n"; ret = -1; break; }
    }
    endline();
    return ret;
}

template<typename T>
int test_smul(bool show_flag = false)
{
    int ret = 0;                // status
    for (uint i = 0; i < 8 * sizeof(T); i++)
    {
        T a = (T)1 << i;        // 2-power i
        T y = smul(a, a);
        cout << "a:" << a << " "
             << "smul(a,a):" << y << " "
             << "a*a:" << (double)a*a << " ";
        if (y == pnw::max_of<T>()) { cout << "(MAX) "; }
        else if (y >= a) { cout << "SUCCESS\n"; }
        else { cout << "FAILURE\n"; ret = -1; break; }
    }
    endline();
    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

int test_parcmp8(bool show_flag = false)
{
    int ret = 0;                // status
    int8_t x[8], y[8];
    for (int i = 0; i < 8; ++i)
    {
        x[i] = int8_rand();
        y[i] = int8_rand();
    }
    int8_parcmp8(x, y);

    cout << "int8_parcmp8: ";
    for (int i = 0; i < 8; ++i) {
        if (x[i] < y[i]) {
            cout << x[i] << "," << y[i] << " ";
            cout << "SUCCESS ";
        } else {
            cout << "FAILURE "; ret -= 1;
        }
    }
    endline();

    return ret;
}

int test_parcmp4(bool show_flag = false)
{
    int ret = 0;                // status
    int16_t x[4], y[4];
    for (int i = 0; i < 4; ++i)
    {
        x[i] = int16_rand();
        y[i] = int16_rand();
    }
    int16_parcmp4(x, y);

    cout << "int16_parcmp4: ";
    for (int i = 0; i < 4; ++i) {
        if (x[i] < y[i]) {
            cout << x[i] << "," << y[i] << " ";
            cout << "SUCCESS ";
        } else {
            cout << "FAILURE "; ret -= 1;
        }
    }
    endline();

    return ret;
}

int test_parcmp2(bool show_flag = false)
{
    int ret = 0;                // status
    int32_t x[2], y[2];
    for (int i = 0; i < 2; ++i)
    {
        x[i] = int32_rand();
        y[i] = int32_rand();
    }
    int32_parcmp2(x, y);

    cout << "int32_parcmp2: ";
    for (int i = 0; i < 2; ++i) {
        if (x[i] < y[i]) {
            cout << x[i] << "," << y[i] << " ";
            cout << "SUCCESS ";
        } else {
            cout << "FAILURE "; ret -= 1;
        }
    }
    endline();

    return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

void
test_color_float_convert_HSV2RGB(void)
{
#define N_COLORS 6

    const char *colorNames[N_COLORS] =
        { "red", "green", "blue", "cyan", "magenta", "yellow" };

    float hsv[N_COLORS][3] = {
        {0.000, 1.00, 1.00},
        {0.333, 1.00, 1.00},
        {0.676, 1.00, 1.00},
        {0.500, 1.00, 1.00},
        {0.833, 1.00, 1.00},
        {0.167, 1.00, 1.00}
    };

    float r, g, b;

    for (int i = 0; i < N_COLORS; i++) {
        float_convert_HSV2RGB(hsv[i][0], hsv[i][1], hsv[i][2], &r, &g, &b);

        printf("%s HSV: (%3.2f, %3.2f, %3.2f)"
               "\nRGB: (%3.2f %3.2f %3.2f)\n",
               colorNames[i], hsv[i][0], hsv[i][1], hsv[i][2], r, g, b);
    }
}

void
test_color_float_convert_RGB2HSV(void)
{
#define N_COLORS 6

    const char *colorNames[N_COLORS] =
        { "red", "green", "blue", "cyan", "magenta", "yellow" };

    float rgb[3*N_COLORS][3] = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
        {0.0, 1.0, 1.0},
        {1.0, 0.0, 1.0},
        {1.0, 1.0, 0.0},

        {0.75, 0.00, 0.00},
        {0.00, 0.75, 0.00},
        {0.00, 0.00, 0.75},
        {0.00, 0.75, 0.75},
        {0.75, 0.00, 0.75},
        {0.75, 0.75, 0.00},

        {1.00, 0.30, 0.30},
        {0.30, 1.00, 0.30},
        {0.30, 0.30, 1.00},
        {0.30, 1.00, 1.00},
        {1.00, 0.30, 1.00},
        {1.00, 1.00, 0.30}
    };
    float h, s, v;
    int i, j;

    for (i = 0; i < N_COLORS; i++) {
        float_convert_RGB2HSV(rgb[i][0], rgb[i][1], rgb[i][2], &h, &s, &v);
        printf("%s  RGB: (%3.2f, %3.2f, %3.2f)  "
               "HSV: (%3.2f %3.2f %3.2f)\n",
               colorNames[i], rgb[i][0], rgb[i][1], rgb[i][2], h, s, v);
    }

    printf("\ndarker versions of the colors -- \n");
    for (i = N_COLORS, j = 0; i < 2 * N_COLORS; i++, j++) {
        float_convert_RGB2HSV(rgb[i][0], rgb[i][1], rgb[i][2], &h, &s, &v);
        printf("%s  RGB: (%3.2f, %3.2f, %3.2f)  "
               "HSV: (%3.2f %3.2f %3.2f)\n",
               colorNames[j], rgb[i][0], rgb[i][1], rgb[i][2], h, s, v);
    }

    printf("\npaler versions of the colors -- \n");
    for (i = 2 * N_COLORS, j = 0; i < 3 * N_COLORS; i++, j++) {
        float_convert_RGB2HSV(rgb[i][0], rgb[i][1], rgb[i][2], &h, &s, &v);
        printf("%s  RGB: (%3.2f, %3.2f, %3.2f)  "
               "HSV: (%3.2f %3.2f %3.2f)\n",
               colorNames[j], rgb[i][0], rgb[i][1], rgb[i][2], h, s, v);
    }
}

/* ---------------------------- Group Separator ---------------------------- */

int
main(int argc, char *argv[])
{
    int ret = 0;                // status
    int ecnt = 0;               // error counter

    test_color_float_convert_HSV2RGB();
    test_color_float_convert_RGB2HSV();

    cout << "### sadd" << endl;
    cout << "####  8-bit unsigned" << endl; ecnt += (test_sadd<uint8_t >() < 0);
    cout << "#### 16-bit unsigned" << endl; ecnt += (test_sadd<uint16_t>() < 0);
    cout << "#### 32-bit unsigned" << endl; ecnt += (test_sadd<uint32_t>() < 0);
    cout << "#### 64-bit unsigned" << endl; ecnt += (test_sadd<uint64_t>() < 0);
    cout << "####  8-bit signed" << endl; ecnt += (test_sadd<int8_t >() < 0);
    cout << "#### 16-bit signed" << endl; ecnt += (test_sadd<int16_t>() < 0);
    cout << "#### 32-bit signed" << endl; ecnt += (test_sadd<int32_t>() < 0);
    cout << "#### 64-bit signed" << endl; ecnt += (test_sadd<int64_t>() < 0);

    cout << "### ssub" << endl;
    cout << "####  8-bit unsigned" << endl; ecnt += (test_ssub<uint8_t >() < 0);
    cout << "#### 16-bit unsigned" << endl; ecnt += (test_ssub<uint16_t>() < 0);
    cout << "#### 32-bit unsigned" << endl; ecnt += (test_ssub<uint32_t>() < 0);
    cout << "#### 64-bit unsigned" << endl; ecnt += (test_ssub<uint64_t>() < 0);
    cout << "####  8-bit signed" << endl; ecnt += (test_ssub<int8_t >() < 0);
    cout << "#### 16-bit signed" << endl; ecnt += (test_ssub<int16_t>() < 0);
    cout << "#### 32-bit signed" << endl; ecnt += (test_ssub<int32_t>() < 0);
    cout << "#### 64-bit signed" << endl; ecnt += (test_ssub<int64_t>() < 0);

    cout << "### smul" << endl;
    cout << "####  8-bit unsigned" << endl; ecnt += (test_smul<uint8_t >() < 0);
    cout << "#### 16-bit unsigned" << endl; ecnt += (test_smul<uint16_t>() < 0);
    cout << "#### 32-bit unsigned" << endl; ecnt += (test_smul<uint32_t>() < 0);
    cout << "#### 64-bit unsigned" << endl; ecnt += (test_smul<uint64_t>() < 0);
    cout << "####  8-bit signed" << endl; ecnt += (test_smul<int8_t >() < 0);
    cout << "#### 16-bit signed" << endl; ecnt += (test_smul<int16_t>() < 0);
    cout << "#### 32-bit signed" << endl; ecnt += (test_smul<int32_t>() < 0);
    cout << "#### 64-bit signed" << endl; ecnt += (test_smul<int64_t>() < 0);

    ecnt += (test_parcmp8() < 0);
    ecnt += (test_parcmp4() < 0);
    ecnt += (test_parcmp2() < 0);

    if (ecnt > 0) {
        cout << ecnt << " number of tests failed:" << endl;
        ret = -1;
    }

    return ret;
}
