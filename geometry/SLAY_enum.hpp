/*! \file SLAY_enum.hpp
 * \brief Shape Layout Type.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <unistd.h>

/*! Shape Layout Type Code.
 */
typedef enum {
    SLAY_RING,			/**< Ring Layout */

    SLAY_SUNBURST,		/**< (Circular) Sun Burst Layout */

    /** \name Grid Layouts */
    /*@{*/
    SLAY_HGRID,			/**< Horizontal 1D-Grid Layout */
    SLAY_VGRID,			/**< Vertical 1D-Grid Layout */
    SLAY_RGRID,			/**< Rectangular 2D-Grid Layout */
    /*@}*/

    /** \name Tree Layouts */
    /*@{*/
    SLAY_HTREE,			/**< Horizontal Tree Layout */
    SLAY_VTREE,			/**< Vertical Tree Layout */
    /*@}*/

    SLAY_GRAPHVIZ_DOT,		/**< GraphViz dot */
    SLAY_GRAPHVIZ_NEATO,		/**< GraphViz neato */
    SLAY_GRAPHVIZ_FDP,		/**< GraphViz fdp */
    SLAY_GRAPHVIZ_TWOPI,		/**< GraphViz twopi */
    SLAY_GRAPHVIZ_CIRCO,		/**< GraphViz circo */
} __attribute__ ((packed)) SLAY_t;

/*! Return non-zero if \p slay belongs to the \p GRID-group.
 */
inline int SLAY_is_GRID(SLAY_t slay) {
    return (slay == SLAY_HGRID ||
            slay == SLAY_VGRID ||
            slay == SLAY_RGRID);
}

/*! Return non-zero if \p slay belongs to the \p TREE-group.
 */
inline int SLAY_is_TREE(SLAY_t slay) {
    return (slay == SLAY_HTREE ||
            slay == SLAY_VTREE);
}

/*! Return non-zero if \p slay belongs to the \p GRAPHVIZ-group.
 */
inline int SLAY_is_GRAPHVIZ(SLAY_t slay) {
    return (slay == SLAY_GRAPHVIZ_DOT ||
            slay == SLAY_GRAPHVIZ_NEATO ||
            slay == SLAY_GRAPHVIZ_FDP ||
            slay == SLAY_GRAPHVIZ_TWOPI ||
            slay == SLAY_GRAPHVIZ_CIRCO);
}

/*! Lookup recursion type \p slay name as a C-string.
 * \return C-string, 0 nullptr if type \p slay was unknown.
 */
inline const char* SLAY_getName(SLAY_t slay)
{
    const char* str = nullptr;
    switch (slay) {
    case SLAY_RING:  str = "SLAY_RING"; break;
    case SLAY_SUNBURST: str = "SLAY_SUNBURST"; break;

    case SLAY_HGRID: str = "SLAY_HGRID"; break;
    case SLAY_VGRID: str = "SLAY_VGRID"; break;
    case SLAY_RGRID: str = "SLAY_RGRID"; break;

    case SLAY_HTREE: str = "SLAY_HTREE"; break;
    case SLAY_VTREE: str = "SLAY_VTREE"; break;

    case SLAY_GRAPHVIZ_DOT: str = "SLAY_GRAPHVIZ_DOT"; break;
    case SLAY_GRAPHVIZ_NEATO: str = "SLAY_GRAPHVIZ_NEATO"; break;
    case SLAY_GRAPHVIZ_FDP: str = "SLAY_GRAPHVIZ_FDP"; break;
    case SLAY_GRAPHVIZ_TWOPI: str = "SLAY_GRAPHVIZ_TWOPI"; break;
    case SLAY_GRAPHVIZ_CIRCO: str = "SLAY_GRAPHVIZ_CIRCO"; break;
    }
    return str;
}
