/*! \file vis.hpp
 * \brief Generalized Shape describing the Visual Form of Two-dimensional object.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <cfloat>
#include "utils.h"
#include "color.hpp"
#include "color_constants.hpp"
#include "palette.h"
#include "vec.hpp"
#include "box.hpp"
#include "sphere.hpp"
#include "ell.hpp"
#include "poly.hpp"
#include "rbox.hpp"
#include "polyn.hpp"
#include "ubox.hpp"
#include "cir.hpp"
#include "sphere_shell.hpp"
#include "crarc.hpp"
#include "type_pairs.h"
#include "shapelayer.h"
#include "SFILL_enum.h"

#if defined(HAVE_INTTYPES_H)
#  include <inttypes.h>
#endif

#ifndef NDEBUG
extern const char * g_shape_warn_sform_nodef;
extern const char * g_shape_warn_sform;
extern const char * g_shape_warn_bbox;
extern const char * g_shape_warn_zerodat;
extern const char * g_shape_warn_index_ctrlp_mask_index;
#endif

struct kdTree;

/*! Set internal kdTree-pointer to \p owner. */
void vis_kdtree_init(struct kdTree * owner);

/*! Shape <em>Form</em> Type Code. */
typedef enum {
    SFORM_BOX2f,			/**< Rectangle 2D. */
    SFORM_ELL2f,			/**< Ellipse 2D. */

    SFORM_POLY2f,			/**< Polygon 2D. */
    SFORM_TRI2f,			/**< Triangle 2D. */
    SFORM_QUAD2f,			/**< Quad 2D. */

    SFORM_SQR2f,			/**< Square 2D. */
    SFORM_CIR2f,			/**< Circle 2D. */

    SFORM_CRING2f,		/**< Circular Ring 2D. */
    SFORM_CRARC2f,			/**< Circular Ring-Segment 2D. */

    SFORM_RBOX2f_ell,             /**< Elliptically Rounded Rectangle 2D. */
    SFORM_RBOX2f_lin,             /**< Line (Straightly) Rounded Rectangle 2D. */

    SFORM_BOX2f_D,                /**< Rectangle with Depth Dimension 2D. */
    SFORM_ELL2f_D,		/**< Ellipse with Depth Dimension 2D. */

    SFORM_POLY2f_D,		/**< Polygon 2D with Depth Dimension. */
    SFORM_TRI2f_D,		/**< Triangle 2D with Depth Dimension. */
    SFORM_QUAD2f_D,		/**< Quad 2D with Depth Dimension. */

    SFORM_SQR2f_D,		/**< Square 2D with Depth Dimension. */
    SFORM_CIR2f_D,		/**< Circle 2D with Depth Dimension. */

    SFORM_CRING2f_D,		/**< Circular Ring 2D with Depth Dimension. */
    SFORM_CRARC2f_D,		/**< Circular Ring-Segment 2D with Depth Dimension. */

    SFORM_RBOX2f_ell_D,           /**< Elliptically Rounded Rectangle 2D with Depth Dimension. */
    SFORM_RBOX2f_lin_D,           /**< Line (Straightly) Rounded Rectangle 2D with Depth Dimension. */

    SFORM_BOX3f,			/**< 3d-Box. */

    SFORM_SPHEREf,             /**< Sphere. */

    SFORM_rel,			/**< Relative Box 2D of any. */
    SFORM_rel_BOX2f,		/**< Relative Box 2D of floats. */

    SFORM_rel_BOX2u8,             /**< Relative Box 2D of 8-bit uint. */
    SFORM_rel_BOX2u16,            /**< Relative Box 2D of 16-bit uint. */
    SFORM_rel_BOX2u32,            /**< Relative Box 2D of 32-bit uint. */

    SFORM_rel_BOX2s8,             /**< Relative Box 2D of 8-bit sint. */
    SFORM_rel_BOX2s16,            /**< Relative Box 2D of 16-bit sint. */
    SFORM_rel_BOX2s32,            /**< Relative Box 2D of 32-bit sint. */

    SFORM_comb2,			/**< Combination of other shapes. */
    SFORM_comb2_UNION,		/**< Union. */
    SFORM_comb2_ISECT,		/**< Intersection. */
    SFORM_comb2_DIFF,		/**< Difference. */

    SFORM_INDEXED,		/**< Indexed (Compressed). */

    SFORM_undefined_,	/**< Undefined. */
    SFORM_NUM = SFORM_undefined_
} __attribute__ ((packed)) SFORM_t;

/*! Return non-zero if sform belongs to the group of Shape Set Combinators. */
inline bool
SFORM_is_Combinator(SFORM_t sform)
{
    return (sform == SFORM_comb2_UNION or
            sform == SFORM_comb2_ISECT or
            sform == SFORM_comb2_DIFF);
}

/*! Return non-zero if sform belongs to the group of Shape Set Combinators. */
inline bool
SFORM_is_CRING2f(SFORM_t sform)
{
    return (sform == SFORM_CRING2f or
            sform == SFORM_CRING2f_D);
}

/*! Return non-zero if sform belongs to the group of Shape Set Combinators. */
inline bool
SFORM_is_CRARC2f(SFORM_t sform)
{
    return (sform == SFORM_CRARC2f or
            sform == SFORM_CRARC2f_D);
}

/*! Return non-zero if sform belongs to the group of Relative Shapes. */
inline bool
SFORM_is_REL(SFORM_t sform)
{
    return (sform == SFORM_rel or
            sform == SFORM_rel_BOX2f or
            sform == SFORM_rel_BOX2u8 or
            sform == SFORM_rel_BOX2u16 or
            sform == SFORM_rel_BOX2u32 or
            sform == SFORM_rel_BOX2s8 or
            sform == SFORM_rel_BOX2s16 or
            sform == SFORM_rel_BOX2s32);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Shape <em>Frame</em> Type Code. */
typedef enum {
    SFRAME_NONE,			/**< None. */

    SFRAME_INNER_BOXED,		/**< Inner Boxed. */
    SFRAME_OUTER_BOXED,		/**< Outer Boxed. */

    SFRAME_OUTER_ROUNDED,		/**< Outer (Minimum Circularly Rounded). */

    SFRAME_undefined_,	/**< Undefined. */

    SFRAME_NUM = SFRAME_undefined_

} __attribute__ ((packed)) SFRAME_t;

/* ---------------------------- Group Separator ---------------------------- */

/*! Shape Fill (SFILL) Data Union. */
typedef union {
    color_t        single;	/**< Single Color. */
    color_pair_t * grad;		/**< Two-Color (Pair) Gradient. */
    cpal_t *       pal;		/**< Multi-Color Palette Gradient. */
} sfillD_t;

/*! Visual (Shape).
 * This is the abstract class representing the @e mvisual \em appearance of
 * an object.
 */
typedef struct vis_t
{
    SFORM_t sform;		/**< \em Form Type. */
    SFRAME_t sframe:3;		/**< \em Frame Type. */

  SFILL_t sfill:4;		/**< \em Fill Type. */
  sfillD_t sfillD;		/**< \em Fill Data. */

  layer_t slayer;		/**< Layer. */
  void * sformD;		/**< The Geometry (form) data. */
} vis_t;

/*! Shape-to-Shape. */
typedef struct {
  vis_t src;			/**< Source. */
  vis_t dst;			/**< Destination. */
} vis_pair_t;

typedef struct { vis_t this_shp; box2f box; } vis_box2f;
typedef struct { vis_t this_shp; ell2f ell; } vis_ell2f;
typedef struct { vis_t this_shp; poly2f poly; } vis_poly2f;
typedef struct { vis_t this_shp; tri2f tri; } vis_tri2f;
typedef struct { vis_t this_shp; vec2f quad[4]; } vis_quad2f;
typedef struct { vis_t this_shp; sqr2f sqr; } vis_sqr2f;
typedef struct { vis_t this_shp; cirf cir; } vis_cirf;
typedef struct { vis_t this_shp; rbox2f rbox; } vis_rbox2f;

typedef struct { vis_t this_shp; box2f box; float z; } vis_box2f_z;
typedef struct { vis_t this_shp; ell2f ell; float z; } vis_ell2f_z;
typedef struct { vis_t this_shp; poly2f poly; float z; } vis_poly2f_z;
typedef struct { vis_t this_shp; tri2f tri; float z; } vis_tri2f_z;
typedef struct { vis_t this_shp; vec2f quad[4]; float z; } vis_quad2f_z;
typedef struct { vis_t this_shp; sqr2f sqr; float z; } vis_sqr2f_z;
typedef struct { vis_t this_shp; cirf cir; float z; } vis_cirf_z;
typedef struct { vis_t this_shp; rbox2f rbox; float z; } vis_rbox2f_z;

typedef struct { vis_t this_shp; box3f box; } vis_box3f;
typedef struct { vis_t this_shp; spheref sphere; } vis_spheref;
g
/* ---------------------------- Group Separator ---------------------------- */

void vis_clear(vis_t& shp);

/* ---------------------------- Group Separator ---------------------------- */

/*! Set shape \p shp's layer.
 * \return 1 if layer was changed, 0 otherwise.
 */
int vis_setLayer(vis_t& shp, layer_t layer);

/*! Get shape \p shp's layer. */
inline layer_t vis_getLayer(vis_t shp) { return shp.slayer; }

/*! Set shape \p shp frame to \p sframe.
 * \return 1 if color was changed, 0 otherwise.
 */
int vis_set_SFRAME(vis_t& shp, SFRAME_t sframe);

/* ---------------------------- Group Separator ---------------------------- */

/*! Set shape \p shp's fill to the <em>Single Color</em> color.
 * \return 1 if color was changed, 0 otherwise.
 */
void vis_set_SFILL_SINGLE(vis_t& shp, color_t color);

void vis_set_SFILL_GRAD(vis_t& shp, SFILL_t sfill,
                           color_t color0, color_t color1);
/*! Set shape \p shp's fill to the <em>Gradient</em> grad.
 * \return 1 if color was changed, 0 otherwise.
 */
void vis_set_SFILL_GRAD_pair(vis_t& shp, SFILL_t sfill,
				 const color_pair_t * grad);

void vis_set_SFILL_GRAD_default(vis_t& shp, SFILL_t sfill);

/*! Set shape \p shp's fill to the <em>palette</em> pal.
 * \return 1 if color was changed, 0 otherwise.
 */
void vis_set_SFILL_PAL(vis_t& shp, SFILL_t sfill,
			  const cpal_t * pal);

/*! Set shape \p shp's fill to the <em>palette</em> [color0, color1, color2].
 * \return 1 if color was changed, 0 otherwise.
 */
void vis_set_SFILL_PAL3(vis_t& shp, SFILL_t sfill,
			   color_t color0,
			   color_t color1,
			   color_t color2);

/* ---------------------------- Group Separator ---------------------------- */

/*! Set shape \p shp's \p idx:th color to \p color.
 * \return 1 if color was changed, 0 otherwise.
 */
int vis_setColor(vis_t& shp, color_t color, size_t idx);

/*! Read shape \p shp's \p idx:th color into \p color.
 * \return 1 if color was read, 0 otherwise.
 */
int vis_rdColor(vis_t shp, size_t idx, color_t * color);

color_t vis_getColor(vis_t shp, size_t idx);

/* ---------------------------- Group Separator ---------------------------- */

/*! Randomize all fill colors in shape \p shp. */
int vis_randRGB_SFILL(vis_t& shp);

int vis_randRGBA_SFILL(vis_t& shp);

/* ---------------------------- Group Separator ---------------------------- */

/*! Indication Higlighting of shape \p shp. */
int vis_highlightInd(vis_t& shp, uint8_t prg);

/*! Modify the Alpha-Transparency part of shape \p shp. */
int vis_modA(vis_t& shp, uint8_t prg);

/*! Scale the Alpha-Transparency part of shape \p shp. */
int vis_scaleT(vis_t& shp, uint8_t alpha);

/* ---------------------------- Group Separator ---------------------------- */

/*! Set shape \p shp to \p b. */
void vis_setShape(vis_t& shp, vis_t b);

/*! Clone the shape \p a.
 * \return the cloned shape.
 */
vis_t& vis_clone(vis_t a);

/* ---------------------------- Group Separator ---------------------------- */

/*! Set the \em center of the shape \p a to \p cen and put the result
 * in \p b.
 * \return 1 if center was changed, -1 if type not supported, 0 otherwise.
 */
int center_at(vis_t& a, vec2f cen);

/*! Set the Inner or Outer \em Dimension of the Shape \p a to \p dim.
 * \param[in] outer_flag non-zero if outer dimension is wanted, otherwise inner dimension.
 * \return 1 if dimension was changed, -1 if type not supported, 0 otherwise.
 */
int set_dim(vis_t& a, vec2f dim, bool outer_flag);

/* ---------------------------- Group Separator ---------------------------- */

/*! Fit shape \p shp precisely Inside (Bounding) Box extents \p ext.
 * \param morph_flag If set circles/squares are \em morphed to
 *                   ellipses/rectangles.
 * \return 1 if extents were changed, -1 if type not supported, 0 otherwise.
 */
int fit_in(vis_t& shp, box2f ext, bool morph_flag);

/*! Scale the shape \p a the factor [\p fx, \p fy].
 * \param morph_flag If set circles/squares are \em morphed to
 *                   ellipses/rectangles.
 * \return 1 if shape was scaled, -1 if type not supported, 0 otherwise.
 */
int vis_scalexy_self(vis_t& shp, float fx, float fy, bool morph_flag);

/* ---------------------------- Group Separator ---------------------------- */

/*! Read Control Points \p vecN_ret and Dimensions \p dimN_ret that
 * defines the shape \p shp.
 * \return 1 if stat was read, -1 if type not supported, 0 otherwise.
 */
int vis_rdCtrlPD(vis_t shp,
                    vec2f& vecs_ret, int * vecsN_ret,
                    float * dims_ret, int * dimsN_ret);

/*! Shape Control Point Focus Mask at (mouse) position
 * \p mpos.
 *
 * \param[in] cover_flag non-zero if we only want shapes that cover \p mpos.
 * \param[in] set_flag is set if bit is to be set, or 0 if bit is to
 *            be cleared.
 *
 * \param[out] mask bit at index given by the control point number is set.
 * \return 1 if index was read, -1 if type not supported, 0 otherwise.
 */
int
vis_rdCtrlPM_SFORM(vis_t shp, float dirang,
                      vec2f mpos, uint8_t * mask,
                      bool cover_flag, bool set_flag);

int
vis_rdCtrlPM_SFILL(vis_t shp, float dirang,
                      vec2f mpos, uint8_t * mask,
                      bool cover_flag, bool set_flag);

/*! Resize shape using the user (mouse) drag from \p mpos the shift \p mshift.
 * \param[in] mpos Mouse Position before shift occurred.
 * \param[in] mshift Mouse Shift.
 * \param[in] mask Control Point Mask to use.
 * \param[in] PAR Preferred Aspect Ratio (if respected) or \c NULL otherwise.
 * \return 1 if extents were changed, -1 if type not supported, 0 otherwise.
 */
int vis_shiftCtrlP(vis_t& shp, float dirang,
		      vec2f mpos, vec2f mshift,
		      vec2f mindim, const uint8_t * mask,
		      vec2f PAR, bool outer_flag);

/* ---------------------------- Group Separator ---------------------------- */

/*! Change shape \p shp to a Rectangular Box at \p box.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setBOX(vis_t& shp, box2f box);

/*! Change shape \p shp to an Ellipse at \p ell.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setELL(vis_t& shp, ell2f ell);

/*! Change shape \p shp to a Triangle at \p tri.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setTRI(vis_t& shp, const vec2f tri[3]);

/*! Change shape \p shp to a Quad at \p quad.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setQUAD(vis_t& shp, const vec2f quad[4]);

/*! Change shape \p shp to a Polygon at \p vtx of \p vtxN vertices.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setPOLY(vis_t& shp, vec2f vtx, size_t vtxN);

/*! Change shape \p shp to a Square at \p square.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setSQR(vis_t& shp, sqr2f sqr);

/*! Change shape \p shp to a Circle at \p cir.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setCIR(vis_t& shp, const cirf * cir);

/*! Change shape \p shp to a Ring-Segment at \p cring.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setCRING(vis_t& shp, cring2f cring);

/*! Change shape \p shp to a Ring-Segment at \p crarc.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setRSEG(vis_t& shp, crarc2f crarc);

/*! Change shape \p shp to a Volume Box at \p box.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setBOX3(vis_t& shp, box3f box);

/*! Change shape \p shp to a Sphere at \p sphere.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setSPHERE(vis_t& shp, const spheref * sphere);

/*! Change shape \p shp to a RBOX_ELL at \p box, rad.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setRBOX_ELL(vis_t& shp,
			box2f box, vec2f rad, CORNMASK_t cornmask);

/*! Change shape \p shp to a RBOX_LIN at \p box, rad.
 * \return 1 if shape was changed, -1 if type not supported, 0 otherwise.
 */
int vis_setRBOX_LIN(vis_t& shp,
			box2f box, vec2f rad, CORNMASK_t cornmask);

/* ---------------------------- Group Separator ---------------------------- */

/*! Translate the shape \p shp the shift shift. */
int vis_move(vis_t& shp, vec2f shift);

/* ---------------------------- Group Separator ---------------------------- */

/*! Initialize the shape \p shp to be undefined. */
void vis_init_undefined(vis_t& shp);

/*! Initialize the shape \p shp to be a clone of b. */
void vis_init_vis(vis_t& shp, vis_t b);

/* ---------------------------- Group Separator ---------------------------- */

/*! Generalized Initialize.
 * Initialize the shape \p shp of a typesformfitted inside bnd with color \p shcol.
 *
 * Convenient function to construct shapes that are layed out inside a
 * bounding box bnd without having to specify shape coordinates for each
 * specific SFORM_t.
 */
void
vis_init(vis_t& shp,
	     SFORM_t sform, box2f bnd, color_t shcol, layer_t layer);

/*! Initialize a (Rectangular) Box at \p box. */
void
vis_initBOX(vis_t& shp,
		box2f box, color_t shcol, layer_t layer);

/*! Initialize an Ellipse at \p ell. */
void
vis_initELL(vis_t& shp,
		ell2f ell, color_t shcol, layer_t layer);

/*! Initialize a Triangle at \p tri. */
void
vis_initTRI(vis_t& shp,
		const vec2f tri[3], color_t shcol, layer_t layer);

/*! Initialize a Quad at \p quad. */
void
vis_initQUAD(vis_t& shp,
		 const vec2f quad[4], color_t shcol, layer_t layer);

/*! Initialize a Elliptically Rounded Rectangle at \p box, rad. */
void
vis_initRBOX_ELL(vis_t& shp,
		     box2f box, vec2f rad,
		     color_t shcol, layer_t layer);

/*! Initialize a Line Rounded Rectangle at \p box, rad. */
void
vis_initRBOX_LIN(vis_t& shp,
		     box2f box, vec2f rad,
		     color_t shcol, layer_t layer);

/*! Initialize a Circle at \p cir. */
void
vis_initCIR(vis_t& shp,
		const cirf * cir, color_t shcol, layer_t layer);

/*! Initialize a Circular Ring-Segment at \p cring. */
void
vis_initCRING(vis_t& shp,
		  cring2f cring, color_t shcol, layer_t layer);

/*! Initialize a Circular Ring-Segment at \p crarc. */
void
vis_initRSEG(vis_t& shp,
		  crarc2f crarc, color_t shcol, layer_t layer);

/*! Initialize a Circular Ring-Segment at \p sphere. */
void
vis_initSPHERE(vis_t& shp,
                  const spheref * sphere, color_t shcol, layer_t layer);

/*! Initialize a vtxN-Ordered Polygon at \p vtx. */
void
vis_initPOLY(vis_t& shp,
		vec2f vtx, size_t vtxN,
		color_t shcol, layer_t layer);

/*! Initialize a shape-algebraic union of the shapes \p a and \p b. */
void vis_init_comb2_UNION(vis_t& shp, vis_t& a, vis_t& b);

/*! Initialize a shape-algebraic intersection of the shapes \p a and \p b. */
void vis_init_comb2_ISECT(vis_t& shp, vis_t& a, vis_t& b);

/*! Initialize a shape-algebraic difference of the shapes \p a and \p b. */
void vis_init_comb2_DIFF(vis_t& shp, vis_t& a, vis_t& b);

/* ---------------------------- Group Separator ---------------------------- */

/*! Construct a shape of type \p sform default-layouted (fitted) inside \p bnd with
 * color \p shcol.
 *
 * This is a convenient way to create a shape and
 * assure that its is layed out without
 * having to specify shape coordinates for every specific shape type.
 * \return the create shape, or \c NULL if no shape was created.
 */
vis_t& vis_new(SFORM_t sform, box2f bnd, color_t shcol, layer_t layer);

/* ---------------------------- Group Separator ---------------------------- */

/*! \name Boundaries. */

/* @{ */

/*! Outer Box a of \p shp. */
box2f vis_outer_box(vis_t shp);

/*! Get Boundary limit nr \p i.
 * \param[in] i is 0 for lower bound and one for upper bound.
 */
vec2f vis_getBLim(vis_t shp, int i);

/*! Read Boundaries Dimensions. */
void vis_rdBDims(vis_t shp, vec2f& dim);

/*! Get Boundaries Dimensions. */
vec2f vis_getBDims(vis_t shp);

/*! Get Center of \p shp. */
vec2f vis_cen(vis_t shp);

/*! Read Area \p area of \p shp. */
float vis_area(vis_t shp);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*! \name Boundaries.
 */
/* @{ */
/*! Inner (Sub-Contents) Box Extents \p a of \p shp. */
box2f inner_box(vis_t shp);
/*! Read Inner (Sub-Contents) Center \p cen and Radius \p rad of \p shp.
 * \return 1 if extents was read, -1 if type not supported, 0 otherwise.
 */
int vis_rdICenRad(vis_t shp, vec2f& cen, vec2f& rad);
/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*! Check whether shape partially or completely overlap the box a. */
bool vis_overlap(vis_t shp, box2f a);
/*! Check whether shape lies completely inside the box a. */
bool vis_inside_box2f(vis_t shp, box2f a);
/*! Check whether shape covers the point a. */
bool vis_includes(vis_t shp, float dirang, vec2f a);
/*! Check whether shape covers the box a.
 * Used when scanning this shape.
 */
bool vis_includes(vis_t shp, box2f a);

/*! Print shape \p shp. */
void vis_fprint(FILE * stream, vis_t shp);
