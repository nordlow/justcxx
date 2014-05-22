/*!
 * \file cpuid.h
 * \brief Intel and AMD x86 CPUID display program v 3.3 (1 Jan 2002)
 * \author Copyright 2012 Phil Karn, KA9Q
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Decode Intel TLB and Cache Info Descriptors */
void cpuid_decode_Intel_TLB(int);

/*! Decode Cyrix TLB and Cache Info Descriptors */
void cpuid_decode_Cyrix_TLB(int);

/* ---------------------------- Group Separator ---------------------------- */

/*! Intel-specific information */
void cpuid_doIntel(int);

/*! AMD-specific information */
void cpuid_doAMD(int);

/*! Cyrix-specific information */
void cpuid_doCyrix(int);

/* ---------------------------- Group Separator ---------------------------- */

void cpuid_printregs(int eax, int ebx, int ecx, int edx);

/* ---------------------------- Group Separator ---------------------------- */

void cpuid_test(void);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
