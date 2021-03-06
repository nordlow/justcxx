/*!
 * \file OPSEM_enum.hpp
 * \brief Operation Semantic Classes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Categories Operations in the way read/modify their arguments (obs) and/or
 * create new (obs).
 */

#pragma once

/* ========================================================================= */

/*!
 * Operation (\c OP_t) Semantics Type Code.
 */
typedef enum {
    OPSEM_0,			///< 0-in 0-out (No Operation, NOp)

    OPSEM_1i,			///< 1-in
    OPSEM_1o,			///< 1-out

    OPSEM_Mi,			///< M-in
    OPSEM_Mo,			///< M-outs

    OPSEM_Mi1c,			///< M-in => 1-created
    OPSEM_MiMc,			///< M-in => N-created
    OPSEM_MiMm,			///< M-in => N-modified

    OPSEM_Mm,			///< N-mods

    OPSEM_1i1o,			///< 1-in (pel) => 1-out (currsel)
    OPSEM_1iMo,			///< 1-in (pel) => M-out (currsel)
    OPSEM_Mi1o,			///< M-in (pel) => 1-out (currsel) 
    OPSEM_MiMo,			///< M-in (pel) => M-out (currsel) 

    OPSEM_1o1i,			///< 1-out (pel) => 1-in (currsel)
    OPSEM_1oMi,			///< 1-out (pel) => M-in (currsel)
    OPSEM_Mo1i,			///< M-out (pel) => 1-in (currsel)
    OPSEM_MoMi,			///< M-out (pel) => M-in (currsel)

    OPSEM_MiMiMc,               ///< M-in and M-in => N-created

    OPSEM_MERGE_SELS,           ///< Previous and current Selection is merged
} __attribute__ ((packed)) OPSEM_t;
