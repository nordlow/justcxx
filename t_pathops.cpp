#include <cstring>
#include <cstdlib>
#include <iostream>

#include "utils.h"
#include "timing.h"
#include "pathops.h"
#include "csc.hpp"
#include "pathops.hpp"
#include "find_exec.hpp"
#include "show.hpp"
#include "enforce.hpp"

static int nloops  = 1000000;

static const char * g_home = "/home";
static const char * g_homeS = "/home/";
static const char * g_sub = "peeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeer";
static const char * g_Ssub = "/peeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeer";
static const char * g_SsubShome = "/home/peeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeer";
static const char * g_subS = "peeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeer/";
static const char * g_bashrc = ".bashrc";
static const char * g_full = "/home/peeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeer/.bashrc";

int test_cstr_pathapp(bool canon_flag = true)
{
    int ret = 0;
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
    for (int i = 0; i < nloops; i++) {
        char * p = cstr_pathapp(strdup(g_home), g_sub);
        if (i == 0 and strcmp(p, g_SsubShome) != 0) { printf("NOT OK: %s\n", p); }
        free(p);
    }
    ptimer_toc(&tmr); ptimer_print_sec_usec9(tmr); printf("\n");
    return ret;
}

int test_cstr_pathadd(bool canon_flag = true)
{
    int ret = 0;
    const char * cdir = g_homeS;
    const char * sfil = g_Ssub;
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
    for (int i = 0; i < nloops; i++) {
        char * p = cstr_pathadd(cdir, sfil);
        if (i == 0 and strcmp(p, g_SsubShome) != 0) { printf("NOT OK: %s\n", p); }
        free(p);
    }
    ptimer_toc(&tmr); ptimer_print_sec_usec9(tmr); printf("\n");
    return ret;
}

int test_cbuf_pathadd(bool canon_flag = true)
{
    int ret = 0;
    const char * cdir = g_homeS;
    const char * sfil = g_Ssub;
    size_t clen = strlen(cdir);
    size_t flen = strlen(sfil);
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
    for (int i = 0; i < nloops; i++) {
        char * p = cbuf_pathadd(cdir, clen, sfil, flen, NULL);
        if (i == 0 and strcmp(p, g_SsubShome) != 0) { printf("NOT OK: %s\n", p); }
        free(p);
    }
    ptimer_toc(&tmr); ptimer_print_sec_usec9(tmr); printf("\n");
    return ret;
}

int test_path_add(bool canon_flag = true)
{
    int ret = 0;
    const csc cdir(g_homeS);
    const csc sfil(g_Ssub);
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
    for (int i = 0; i < nloops; i++) {
        const csc p = path_add(cdir, sfil, canon_flag);
        if (i == 0 and compare2(p, g_SsubShome) != 0) { printf("NOT OK: %s\n", p.c_str()); }
    }
    ptimer_toc(&tmr); ptimer_print_sec_usec9(tmr); printf("\n");
    return ret;
}

int test_cstr_pathadd3(bool canon_flag = true)
{
    int ret = 0;
    const char * cdir = g_homeS;
    const char * sub = g_subS;
    const char * sfil = g_bashrc;
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
    for (int i = 0; i < nloops; i++) {
        char * p = cstr_pathadd3(cdir, sub, sfil);
        if (i == 0 and strcmp(p, g_full) != 0) { printf("NOT OK: %s\n", p); }
        free(p);
    }
    ptimer_toc(&tmr); ptimer_print_sec_usec9(tmr); printf("\n");
    return ret;
}

int test_cbuf_pathadd3(bool canon_flag = true)
{
    int ret = 0;
    const char * cdir = g_homeS;
    const char * sub = g_subS;
    const char * sfil = g_bashrc;
    size_t clen = strlen(cdir);
    size_t slen = strlen(sub);
    size_t flen = strlen(sfil);
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
    for (int i = 0; i < nloops; i++) {
        char * p = cbuf_pathadd3(cdir, clen, sub, slen, sfil, flen);
        if (i == 0 and strcmp(p, g_full) != 0) { printf("NOT OK: %s\n", p); }
        free(p);
    }
    ptimer_toc(&tmr); ptimer_print_sec_usec9(tmr); printf("\n");
    return ret;
}

int test_path_add3(bool canon_flag = true)
{
    int ret = 0;
    const csc cdir = g_homeS;
    const csc sub = g_subS;
    const csc sfil = g_bashrc;
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
    for (int i = 0; i < nloops; i++) {
        const csc p = path_add(cdir, sub, sfil, canon_flag);
        if (i == 0 and compare2(p, g_full) != 0) { printf("NOT OK: %s\n", p.c_str()); }
    }
    ptimer_toc(&tmr); ptimer_print_sec_usec9(tmr); printf("\n");
    return ret;
}

int test_memcpy(bool canon_flag = true)
{
    int ret = 0;
    pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
    for (int i = 0; i < nloops; i++) {
        char * p = cstr_pathadd(g_homeS, g_Ssub);
        if (i == 0 and strcmp(p, g_SsubShome) != 0) { printf("NOT OK: %s\n", p); }
        free(p);
    }
    ptimer_toc(&tmr); ptimer_print_sec_usec9(tmr); printf("\n");
    return ret;
}

int test_find_executable_by_name(bool canon_flag = true)
{
    enforce_eq(find_executable_by_name("X", canon_flag), "/usr/bin/X");
    enforce_eq(find_executable_by_name("ls", canon_flag), "/bin/ls");
    enforce_eq(find_executable_by_name("xx_yy_zz", canon_flag), "xx_yy_zz");
    dshowln(find_executable_by_name("X", canon_flag));
    dshowln(find_executable_by_name("ls", canon_flag));
    dshowln(find_executable_by_name("xx_yy_zz", canon_flag));
}

int test_all(bool canon_flag = true)
{
    int ret = 0;
    test_find_executable_by_name();
    printf("cstr_pathapp:  "); ret += test_cstr_pathapp(canon_flag);
    printf("cstr_pathadd:  "); ret += test_cstr_pathadd(canon_flag);
    printf("cbuf_pathadd:  "); ret += test_cbuf_pathadd(canon_flag);
    printf("path_add:   "); ret += test_path_add(canon_flag);
    printf("cstr_pathadd3:  "); ret += test_cstr_pathadd3(canon_flag);
    printf("cbuf_pathadd3:  "); ret += test_cbuf_pathadd3(canon_flag);
    printf("path_add3:   "); ret += test_path_add3(canon_flag);
    return ret;
}

int
main(int argc, char *argv[])
{
    return test_all(true);
}
