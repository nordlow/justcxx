/*!
 * \file kdTree.h
 * \brief k-dimensional Binary Space Partitioner Tree.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * An efficient storage and search structure for points in arbitrary (DIMNUM)
 * dimensions.
 *
 * Optimal structure is emerged through insertion attractors.
 * All insertions of a node y into a node x should be designed like attractors,
 * where an attractor measures how well y fits into x.
 * The attractor is only relevant if we have several choices for choosing the
 * best subtree for inserting y into. "Best" here means the subtree where an
 * insertion results in the smallest change in bytesize.
 * This change can even be negative (int)
 *
 * This search structure enables a set of desired applications:
 * - Efficient O(log(n)) rendering of 2D and 3D shapes used as
 *   - a visualization
 *   - an evaluation of functions (localized to bounding boxes)
 *     that model information in arbitrary dimensions, often when emerging
 *     models sound, image and video.
 */

/* ========================================================================= */

/*!
 * Number of dimensions.
 */
#define DIMNUM (2)

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "box2f.hpp"
#include "color.hpp"
#include "color_constants.hpp"
#include "pdim.h"
#include "vis.hpp"
#include "geometry/find_maxsep.hpp"
#include "geometry/relate.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Type Precision for tree heights.
 *
 * 16-bit precision (65536 levels) should be more than enough for all trees.
 */
typedef uint16_t height_t;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Limit Mask.
 *
 * When inserting a box into a tree we split up the box into its
 * - "lower" boundary (LO_LIM) and
 * - "upper" boundary (UP_LIM) and
 * store pointers to these separatly.
 *
 * When referring to both limits we use the constant BOTH_LIMS.
 */
typedef enum {
  LO_LIM = (1 << 0),
  UP_LIM = (1 << 1),
  BOTH_LIMS = 3
} __attribute__((packed)) LIMASK_t;

/* ---------------------------- Group Separator ---------------------------- */

struct kdTree;

struct kdNode;
struct kdPoint;
struct kdRef;
struct kdLim;
struct kdCBox;
struct kdDup;

/*!
 * Node Type Code.
 */
typedef enum {
  KD_POINT,
  KD_REF,
  KD_LIM,
  KD_CBOX,
  KD_DUP,
  KD_BPART
} __attribute__((packed)) KDNODE_t;

/*!
 * Lookup Node Type ntp as a string.
 */
static inline const char *
NTP_getName(KDNODE_t ntp)
{
  const char * str = NULL;
  switch (ntp) {
  case KD_POINT: str = "POINT"; break;
  case KD_REF: str = "REF"; break;
  case KD_LIM: str = "LIM"; break;
  case KD_CBOX: str = "CBOX"; break;
  case KD_DUP: str = "DUP"; break;
  case KD_BPART: str = "BPART"; break;
  default: break;
  }
  return str;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * A vector of Node Pointers.
 */
typedef struct
{
  struct kdNode ** a;
  size_t n;
} Npv;

/*!
 * Initialize \p npv to be empty.
 */
static inline void
Npv_init(Npv * npv)
{
#ifndef NDEBUG
  if (npv == 0) { PERR("npv is NULL\n"); return; }
#endif
  npv->a = 0;
  npv->n = 0;
}

/*!
 * Initialize \p npv to containg n uninitialized node-pointers.
 */
static inline void
Npv_initN(Npv * npv, size_t n)
{
#ifndef NDEBUG
  if (npv == 0) { PERR("npv is NULL\n"); return; }
#endif
  npv->a = (struct kdNode**) malloc(n * sizeof(struct kdNode*));
  npv->n = n;
}

/*!
 * Clear \p npv.
 */
static inline void
Npv_clear(Npv * npv)
{
#ifndef NDEBUG
  if (npv == 0) { PERR("npv is NULL\n"); return; }
#endif
  if (npv->a) {
    free(npv->a);
    npv->a = 0;
  }
  npv->n = 0;
}

/*!
 * Append node node at end of \p npv.
 */
static inline void
Npv_app(Npv * npv, struct kdNode * node)
{
#ifndef NDEBUG
  if (npv == 0) { PERR("npv is NULL\n"); return; }
#endif
  npv->a = (struct kdNode**)realloc(npv->a, (npv->n + 1) * sizeof(struct kdNode*));
  npv->a[npv->n] = node;
  npv->n++;
}

/*!
 * Append \p nodes_num number nodes nodes at the end of \p npv.
 */
static inline void
Npv_appM(Npv * npv, struct kdNode ** nodes, size_t nodes_num)
{
#ifndef NDEBUG
  if (npv == 0) { PERR("npv is NULL\n"); return; }
#endif
  npv->a = (struct kdNode**)realloc(npv->a, (npv->n + nodes_num) * sizeof(struct kdNode*));
  for (size_t i = 0; i < nodes_num; i++) {
    npv->a[npv->n + i] = nodes[i];
  }
  npv->n += nodes_num;
}

/*!
 * Find First Occurrence of node node in \p npv.
 * \return index at hit or npv->n if node could no be found.
 */
static inline size_t
Npv_findF(Npv * npv, struct kdNode * node)
{
#ifndef NDEBUG
  if (npv == 0) { PERR("npv is NULL\n"); return -1; }
#endif
  for (size_t i = 0; i < npv->n; i++) {
    if (npv->a[i] == node) {
      return i;			/* node was found at index  */
    }
  }
  return npv->n;		/* node could not be found so return n */
}

/*!
 * Pack \p npv by removing all its zero references.
 * \return number of empty slots that were removed.
 */
static inline int
Npv_pack(Npv * npv)
{
#ifndef NDEBUG
  if (npv == 0) { PERR("npv is NULL\n"); return -1; }
#endif
  size_t i, pi;		/* normal index and pack index */
  for (i = pi = 0; i < npv->n; i++) {
    if (npv->a[i]) {
      npv->a[pi] = npv->a[i];
      pi++;
    }
  }
  if (npv->n != pi) {
    npv->n = pi;
    npv->a = (struct kdNode**)dynalloc(npv->a, npv->n * sizeof(struct kdNode*));
  }
  return (npv->n - pi);
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * The Node with which the kdTree is built up.
 */
typedef struct kdNode
{
  KDNODE_t ntp;			/**< Node type. */
  struct kdTree *owner;		/**< Owning tree. */
  struct kdNode *super;		/**< Super (parenting) node. */
  height_t height_tag;		/**< Tree-height at this node. */
  layer_t min_layer; /**< The lowest layer contained in this subtree. */
  layer_t max_layer; /**< The highest layer contained in this subtree. */
} kdNode;

/*!
 * Initialize an existing pre-allocated kdNode having an owner.
 */
void
kdNode_init(kdNode * node,
	    KDNODE_t ntp, struct kdTree *owner, kdNode * super,
	    height_t height, layer_t min_layer, layer_t max_layer);

/*!
 * Create a kdNode having an owner.
 */
kdNode *
kdNode_new(KDNODE_t ntp,
	   struct kdTree *owner, kdNode * super,
	   height_t height, layer_t min_layer, layer_t max_layer);

/*!
 * Clear contents of \p node.
 */
void kdNode_clear(kdNode * node);

/*!
 * Recursive Delete tree from this kdNode.
 * \param[in] leaf_flag non-zero if leaves containing dynamically allocated
 * structures should be deleted aswell.
 */
void kdNode_rdel(kdNode * node, bool leaf_flag);

/*!
 * Insert shape \p a.
 */
kdNode * kdNode_ins_shape(kdNode * node, vis_t * a, LIMASK_t limask);

/*!
 * Insert kdNode a.
 * \param[in,out] a contains the leaves to insert and is often deleted by this
 * operation.
 */
kdNode *kdNode_ins_Node(kdNode * node, kdNode * a);

/*!
 * Limit this kdNode to a potential subnode to region a and return it.
 */
kdNode *kdNode_extractAt(kdNode * node, const box2f * a);

/*!
 * Find the only occurence of the point a.
 */
const kdNode *kdNode_find_vec(const kdNode * node, const vec2f * a);

/*!
 * Find the shape and return the hit.
 */
int kdNode_find_shape(const kdNode * node, const vis_t * a,
		      LIMASK_t * limask, Npv * hits);

/*!
 * Remove the vector \p a.
 * \return the modified tree structure a this kdNode.
 */
kdNode *kdNode_rm_vec(kdNode * node, vec2f * a, size_t *rmcnt_ret);

/*!
 * Remove the limit limithint belonging to box a.
 * \return the modified tree structure at this \p node.
 */
kdNode *kdNode_rm_shape(kdNode * node, vis_t * a,
			LIMASK_t * limask, size_t *rmcnt_ret);

/*!
 * Tear up this \c kdNode along dimension \p pdim at \p ppos into two
 * lists of subnodes \p sn0 and \p sn1.
 */
kdNode * kdNode_tearUp(kdNode * node,
		       pdim256_t pdim, float ppos,
		       Npv * sn0_ret, Npv * sn1_ret);

/*!
 * Balance at \p node.
 */
kdNode * kdNode_bal(kdNode * node);

/*!
 * Recursive Balance at \p node.
 */
kdNode * kdNode_rbal(kdNode * node);

/*!
 * Recursive Balance at \p node.
 */
void kdNode_fprint(FILE * stream, const kdNode * node, uint level);

/*!
 * Read the Bounding Box of the tree's leaves at this kdNode.
 */
int kdNode_rdBnd_box2f(const kdNode * node, box2f * ext);

/*!
 * Get the Bounding Box of the tree's leaves at this kdNode.
 */
static inline box2f
kdNode_getBnd_box2f(const kdNode * node)
{
  box2f bnd; kdNode_rdBnd_box2f(node, &bnd);
  return bnd;
}

/*!
 * Get boundary limit nr \p i of the tree's leaves at this kdNode.
 */
vec2f kdNode_getBL(const kdNode * node, int i);

/*!
 * Get the number of bytes used to store the tree at this kdNode.
 */
size_t kdNode_getSize(const kdNode * node);

/*!
 * Get the name of node \p a.
 */
static inline const char *
kdNode_getName(kdNode * node) { return NTP_getName(node->ntp); }

/*!
 * Get height at current \p node.
 */
static inline height_t
kdNode_getHeight(const kdNode * node) { return node->height_tag;}

/*!
 * Set height at current \p node.
 */
static inline void
kdNode_setHeight(kdNode * node, height_t a) { node->height_tag = a; }

/* ---------------------------- Group Separator ---------------------------- */

typedef struct kdPoint
{
  kdNode this_node;		/**< Inherit. */
  vec2f lpoint;			/**< Leaf point. */
} kdPoint;

void
kdPoint_init(kdPoint * pnt,
	     struct kdTree *owner, kdNode * super, const vec2f * a,
	     layer_t min_layer, layer_t max_layer);

kdPoint *
kdPoint_new(struct kdTree *owner, kdNode * super, const vec2f * a,
	    layer_t min_layer, layer_t max_layer);

void kdPoint_clear(kdPoint * pnt);

static inline void
kdPoint_delete(kdPoint * pnt, bool leaf_flag)
{
  kdPoint_clear(pnt);
  free(pnt);
}

static inline void
kdPoint_rdel(kdPoint * pnt, bool leaf_flag)
{
  kdPoint_delete(pnt, leaf_flag);
}

static inline box2f
kdPoint_getBnd_box2f(const kdPoint * pnt)
{
  box2f bnd;
  bnd.l = bnd.u = pnt->lpoint;
  return bnd;
}

static inline vec2f
kdPoint_getBL(const kdPoint * pnt, int i) { return pnt->lpoint; }

static inline size_t
kdPoint_getSize(const kdPoint * pnt) { return sizeof(kdPoint); }

kdNode *kdPoint_ins_Node(kdPoint * pnt, kdNode * a);

kdNode *kdPoint_ins_Point(kdPoint * pnt, kdPoint * a);

kdNode *kdPoint_ins_shape(kdPoint * pnt, vis_t * a, LIMASK_t limask);

kdNode *kdPoint_extractAt(kdPoint * pnt, const box2f * a);

const kdNode *kdPoint_find_vec(const kdPoint * pnt, const vec2f * a);
int kdPoint_find_shape(const kdPoint * pnt,
		       const vis_t * a, LIMASK_t * limask,
		       Npv * hits);

kdNode *kdPoint_rm_vec(kdPoint * pnt, vec2f * a, size_t *rmcnt_ret);

kdNode *kdPoint_rm_shape(kdPoint * pnt, vis_t * a,
			 LIMASK_t * limask, size_t *rmcnt_ret);

void kdPoint_tearUp(kdPoint * pnt,
		    pdim256_t pdim, float ppos,
		    Npv * sn0_ret, Npv * sn1_ret);

void kdPoint_fprint(FILE * stream, const kdPoint * pnt, uint level);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Point Reference.
 */
typedef struct kdRef
{
  kdPoint this_point;			/**< Inherit. */
  vec2f *vrd;				/**< Point reference data. */
} kdRef;

void kdRef_init(kdRef * ref, struct kdTree *owner, kdNode * super,
		const vec2f * a);

kdRef * kdRef_new(struct kdTree *owner, kdNode * super,
		  const vec2f * a);

void kdRef_clear(kdRef * ref);

static inline void
kdRef_delete(kdRef * ref)
{
  kdRef_clear(ref);
  free(ref);
}

static inline void
kdRef_rdel(kdRef * ref, bool leaf_flag)
{
  if (leaf_flag) { free(ref->vrd); }
  kdRef_delete(ref);
}

static inline size_t
kdRef_getSize(kdRef * ref) { return sizeof(kdRef); }

kdNode *kdRef_ins_Node(kdRef * ref, kdNode * a);

kdNode *kdRef_ins_Ref(kdRef * ref, kdRef * a);

kdNode *kdRef_ins_shape(kdRef * ref, vis_t * a, LIMASK_t limask);

const kdNode *kdRef_find_vec(const kdRef * ref, const vec2f * a);

kdNode *kdRef_rm_vec(kdRef * ref, vec2f * a, size_t *rmcnt_ret);

void
kdRef_fprint(FILE * stream, kdRef * ref, uint level);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Limit for vis_t.
 */
typedef struct kdLim
{
  kdPoint   this_point;		/**< Inherit. */
  vis_t *csh;		/**< Limit shape. */
  uint      cidx:2;		/**< Limit index. */
} kdLim;

void
kdLim_init(kdLim * lim,
	   struct kdTree *owner, kdNode * super,
	   vis_t * a, uint cidx);

kdLim *
kdLim_new(struct kdTree *owner, kdNode * super,
	  vis_t * a, uint cidx);

void kdLim_clear(kdLim * lim);

static inline void
kdLim_delete(kdLim * lim)
{
  kdLim_clear(lim);
  free(lim);
}

static inline void
kdLim_rdel(kdLim * lim, bool leaf_flag)
{
  if (leaf_flag) { vis_delete(lim->csh); }
  free(lim);
}

static inline size_t
kdLim_getSize(kdLim * lim) { return sizeof(kdLim); }

kdNode *kdLim_ins_Lim(kdLim * lim, kdLim * a);

kdNode *kdLim_ins_Node(kdLim * lim, kdNode * a);

kdNode *kdLim_ins_shape(kdLim * lim, vis_t * a, LIMASK_t limask);

const kdNode *kdLim_find_vec(const kdLim * lim, const vec2f * a);
int kdLim_find_shape(const kdLim * lim, const vis_t * a,
		     LIMASK_t * limask, Npv * hits);

kdNode *kdLim_rm_vec(kdLim * lim, vec2f * a, size_t *rmcnt_ret);

kdNode *kdLim_rm_shape(kdLim * lim, vis_t * a,
		       LIMASK_t * limask, size_t *rmcnt_ret);

void kdLim_fprint(FILE * stream, kdLim * lim, uint level);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Container Box for vis_t.
 */
typedef struct kdCBox
{
  kdNode this_node;		/**< Inherit. */
  vis_t *bsh;		/**< The box shape. */
} kdCBox;

void
kdCBox_init(kdCBox * cbx, struct kdTree *owner, kdNode * super, vis_t * a);

kdCBox *
kdCBox_new(struct kdTree *owner, kdNode * super, vis_t * a);

void kdCBox_clear(kdCBox * cbx);

static inline void
kdCBox_delete(kdCBox * cbx)
{
  kdCBox_clear(cbx);
  free(cbx);
}

static inline void
kdCBox_rdel(kdCBox * cbx, bool leaf_flag)
{
  if (leaf_flag) { vis_delete(cbx->bsh); }
  kdCBox_delete(cbx);
}

static inline box2f
kdCBox_getBnd_box2f(const kdCBox * cbx)
{
  return vis_getBnd_box2f(cbx->bsh);
}

static inline vec2f
kdCBox_getBL(const kdCBox * cbx, int i)
{
  box2f bnd;
  vis_rdBnd_box2f(cbx->bsh, &bnd);
  return box2f_at(&bnd, i);
}

static inline size_t
kdCBox_getSize(const kdCBox * cbx) { return sizeof(kdCBox); }

kdNode *kdCBox_ins_Node(kdCBox * cbx, kdNode * a);

kdNode *kdCBox_extractAt(kdCBox * cbx, const box2f * a);

const kdNode *kdCBox_find_vec(const kdCBox * cbx, const vec2f * a);
int kdCBox_find_shape(const kdCBox * cbx, const vis_t * a,
		      LIMASK_t * limask, Npv * hits);

kdNode *kdCBox_rm_vec(kdCBox * cbx, vec2f * a, size_t *rmcnt_ret);

kdNode *kdCBox_rm_shape(kdCBox * cbx, vis_t * a,
			LIMASK_t * limask, size_t *rmcnt_ret);

void kdCBox_tearUp(kdCBox * cbx,
		   pdim256_t pdim, float ppos,
		   Npv * sn0_ret, Npv * sn1_ret);

void
kdCBox_fprint(FILE * stream, const kdCBox * cbx, uint level);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Point Duplication.
 */
typedef struct kdDup
{
  kdNode this_node;		/**< Inherit. */
  vec2f dpp;			/**< Duplication point. */
  Npv pts;	 /**< Duplication points. Must be of type kdPoint!. */
} kdDup;

void
kdDup_init(kdDup * dup,
	   struct kdTree *owner, kdNode * super, kdPoint * a, kdPoint * b);

kdDup *
kdDup_new(struct kdTree *owner, kdNode * super, kdPoint * a, kdPoint * b);

void kdDup_clear(kdDup * dup);

static inline void
kdDup_delete(kdDup * dup)
{
  kdDup_clear(dup);
  free(dup);
}

static inline void
kdDup_rdel(kdDup * dup, bool leaf_flag)
{
  for (size_t i = 0; i != dup->pts.n; i++) {
    kdPoint_rdel((kdPoint*)dup->pts.a[i], leaf_flag); /* recurse */
  }
  kdDup_delete(dup);
}

static inline box2f
kdDup_getBnd_box2f(const kdDup * dup)
{
  box2f bnd;
  bnd.l = bnd.u = dup->dpp;
  return bnd;
}

static inline vec2f
kdDup_getBL(const kdDup * dup, int i)
{
  return dup->dpp;
}

static inline size_t
kdDup_getSize(const kdDup * dup)
{
  size_t sum = sizeof(kdDup);

  for (size_t i = 0; i != dup->pts.n; i++) {
    sum += kdPoint_getSize((kdPoint*)dup->pts.a[i]);
  }

  return sum;
}

kdNode *kdDup_ins_Point(kdDup * dup, kdPoint * a);

kdNode *kdDup_ins_Dup(kdDup * dup, kdDup * a);

kdNode *kdDup_ins_shape(kdDup * dup, vis_t * a, LIMASK_t limask);

kdNode *kdDup_ins_Node(kdDup * dup, kdNode * a);

kdNode *kdDup_extractAt(kdDup * dup, const box2f * a);

const kdNode *kdDup_find_vec(const kdDup * dup, const vec2f * a);
int kdDup_find_shape(const kdDup * dup, const vis_t * a,
		     LIMASK_t * limask, Npv * hits);

/*!
 * Compress this kdNode.
 *
 * If this contains 0 points delete it and return 0.
 * If this contains 1 point delete it and return the point.
 * If this contains >= 2 points return this intact (do nothing).
 */
kdNode *kdDup_compress(kdDup * dup);

kdNode *kdDup_rm_vec(kdDup * dup, vec2f * a, size_t *rmcnt_ret);

kdNode *kdDup_rm_shape(kdDup * dup, vis_t * a,
		       LIMASK_t * limask, size_t *rmcnt_ret);

void kdDup_tearUp(kdDup * dup,
		  pdim256_t pdim, float ppos,
		  Npv * sn0_ret, Npv * sn1_ret);

void kdDup_fprint(FILE * stream, const kdDup * dup, uint level);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Binary Partitioning Hint.
 * \WARNING BPH_getName() must be updated along with this.
 */
typedef enum {
  BPH_ANY,
  BPH_POINT_PAIR,
  BPH_LIM_PAIR,
  BPH_undefined_
} __attribute__((packed)) BPH_t;

/*!
 * Lookup Binary Partitioning Hint bph as a string.
 */
static inline const char *
BPH_getName(BPH_t bph)
{
  const char * str = NULL;
  switch (bph) {
  case BPH_ANY: str = "BPH_ANY";
  case BPH_POINT_PAIR: str = "BPH_POINT_PAIR";
  case BPH_LIM_PAIR: str = "BPH_LIM_PAIR";
  default: break;
  }
  return str;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Binary (Space) Partitioner using an axis-aligned partitioning plane.
 *
 * Locally stores the bounding rectangle.
 */
typedef struct kdBPart
{
  kdNode this_node;		/**< Inherit. */
  box2f pbx;			/**< Partition bounding box. */
  pdim256_t pdim; /**< Dimension along which to partition. 256 should suffice. */
  float ppos; /**< Position at which the partitioning is performed. */
  kdNode *part[2];		/**< Two (binary) branches. */
  BPH_t bph;			/**< Hint. */
} kdBPart;

void
kdBPart_init(kdBPart * bpt,
	     struct kdTree *owner, kdNode * super,
	     pdim256_t pdim, float ppos, kdNode * b0, kdNode * b1);

/*!
 * Explicit general construction.
 */
kdBPart *
kdBPart_new(struct kdTree *owner, kdNode * super,
	    pdim256_t pdim, float ppos, kdNode * b0, kdNode * b1);

void kdBPart_clear(kdBPart * bpt);

static inline void
kdBPart_delete(kdBPart * bpt)
{
  kdBPart_clear(bpt);
  free(bpt);
}

static inline void
kdBPart_rdel(kdBPart * bpt, bool leaf_flag)
{
  kdNode_rdel(bpt->part[0], leaf_flag); /* recurse */
  kdNode_rdel(bpt->part[1], leaf_flag); /* recurse */
  kdBPart_delete(bpt);
}

static inline box2f
kdBPart_getBnd_box2f(const kdBPart * bpt)
{
  return bpt->pbx;
}

static inline vec2f
kdBPart_getBL(const kdBPart * bpt, int i)
{
  return box2f_at(&bpt->pbx, i);
}

static inline size_t
kdBPart_getSize(const kdBPart * bpt)
{
  size_t sum = 0;

  sum += kdNode_getSize(bpt->part[0]);
  sum += kdNode_getSize(bpt->part[1]);

  sum += sizeof(kdBPart);

  return sum;
}

kdNode *kdBPart_ins_shape(kdBPart * bpt, vis_t * a, LIMASK_t limask);

kdNode * kdBPart_ins_Point(kdBPart * bpt, kdPoint * pnt);

kdNode *kdBPart_ins_Node_relate_unions(kdBPart * bpt, kdNode * a);
kdNode *kdBPart_ins_Node_standard(kdBPart * bpt, kdNode * a);
kdNode *kdBPart_ins_Node(kdBPart * bpt, kdNode * a);

/*!
 * \name Balancing.
 */

/* @{ */

/*!
 * One-dimensional AVL-tree Balancing at \p bpt.
 */
void kdBPart_balAVL(kdBPart * bpt);

/*!
 * Balance this Partition by changing \p pdim if any of the dimensions
 * of \p pbx is larger than the dimension along which the partitioning
 * is currently performed.
 */
void kdBPart_balPDim(kdBPart * bpt);

/* @{ */

/*!
 * Compress this kdNode to a new possibly transformed kdNode and return it.
 */
kdNode *kdBPart_compress(kdBPart * bpt);

/*!
 * Balance Partition \p bpt.
 */
kdNode *kdBPart_bal(kdBPart * bpt);

/*!
 * Recursive Balance Partition \p bpt.
 */
kdNode * kdBPart_rbal(kdBPart * bpt);

kdNode *kdBPart_extractAt(kdBPart * bpt, const box2f * a);

const kdNode *kdBPart_find_vec(const kdBPart * bpt, const vec2f * a);
int kdBPart_find_shape(const kdBPart * bpt,
		       const vis_t * a, LIMASK_t * limask,
		       Npv * hits);

kdNode *kdBPart_rm_vec(kdBPart * bpt, vec2f * a, size_t *rmcnt_ret);

kdNode *kdBPart_rm_shape(kdBPart * bpt, vis_t * a,
			 LIMASK_t * limask, size_t *rmcnt_ret);

void kdBPart_tearUp(kdBPart * bpt,
		    pdim256_t pdim, float ppos,
		    Npv * sn0_ret, Npv * sn1_ret);

void kdBPart_fprint(FILE * stream, kdBPart * bpt, uint level);

/*!
 * Repartition at \p bpt.
 */
void kdBPart_repartAt(kdBPart * bpt, pdim256_t new_pdim, float new_ppos);

/*!
 * \name Updaters
 */

/* @{ */

/*!
 * Update Partition Bounding Box.
 */
void kdBPart_updateBBox(kdBPart * bpt);

/*!
 * Update Partition Height.
 */
void kdBPart_updateHeight(kdBPart * bpt);

/*!
 * Update Partition Hint.
 */
void kdBPart_updateBPH(kdBPart * bpt);

/*!
 * Update All Partition States.
 */
void kdBPart_updateAll(kdBPart * bpt);

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * k-D Tree in 2 Dimensions.
 */
typedef struct kdTree
{
  kdNode *root;		      /**< Pointer to the root of the tree. */
  uint point_cnt;
  uint ref_cnt;
  uint lim_cnt;
  uint cbox_cnt;
  uint dup_cnt;
  uint lopair_cnt;
  uint bpt_cnts[DIMNUM];
  size_t * fbs;			/**< Find box splitting statistics. */
  size_t fbs_len;	 /**< Find box splitting statistics length. */
} kdTree;

/*!
 * Construct a Default Empty Tree.
 */
kdTree * kdTree_new(void);

/*!
 * Empty Tree \p tree along with its leaves.
 * \param[in] leaf_flag non-zero if leaves containing dynamically allocated
 * structures should be deleted aswell.
 */
void kdTree_clear(kdTree * tree, bool leaf_flag);

/*!
 * Destructor Deletes Whole Tree \p tree.
 * \param[in] leaf_flag non-zero if leaves containing dynamically allocated
 * structures should be deleted aswell.
 */
void kdTree_delete(kdTree * tree, bool leaf_flag);

/*!
 * Get the Number of Bytes used to store this \p tree.
 */
size_t kdTree_getSize(const kdTree * tree);

/*!
 * Insert the vector \p a into \p tree.
 */
void kdTree_ins_vec(kdTree * tree, vec2f * a);

/*!
 * Insert the shape \p a corner by corner into \p tree.
 */
void kdTree_twostep_ins(kdTree * tree, vis_t * a);

/*!
 * Insert the shape \p a into \p tree.
 */
void kdTree_onestep_ins(kdTree * tree, vis_t * a);

/*!
 * Insert the shape \p a into \p tree.
 */
void kdTree_lim_pair_ins(kdTree * tree, vis_t * a);

/*!
 * Insert the shape \p a into \p tree.
 */
void kdTree_cbox_ins(kdTree * tree, vis_t * a);

/*!
 * Insert the shape \p a into \p tree.
 */
void kdTree_ins_shape(kdTree * tree, vis_t * a);

/*!
 * Remove the vector \p a from \p tree.
 *
 * \return 1 if vector \p a was found and successfully removed,
 * 0 otherwise.
 */
int kdTree_rm_vec(kdTree * tree, vec2f * a);

/*!
 * Remove the shape \p a from \p tree.
 *
 * \return 1 if shape \p a was found and successfully removed,
 * 0 otherwise.
 */
int kdTree_rm_shape(kdTree * tree, vis_t * a);

/*!
 * Remove and then Reinsert then shape \p a into \p tree.
 *
 * Usually called when the shape \p a has been altered.
 *
 * \return 1 if shape \p a was found and successfully removed and then
 * reinserted, 0 otherwise.
 */
int kdTree_reins_shape(kdTree * tree, vis_t * a);

/*!
 * Find the vector \p a into \p tree.
 *
 * \return 1 upon success, 0 otherwise.
 */
const kdNode * kdTree_find_vec(const kdTree * tree, const vec2f * a);

/*!
 * Find the shape \p a in \p tree.
 *
 * \return 1 upon hit, 0 otherwise.
 */
int kdTree_find_shape(const kdTree * tree, const vis_t * a);

/*!
 * Get the Height of the Tree \p tree.
 */
height_t kdTree_getHeight(const kdTree * tree);

/*!
 * Recursive Balance Tree \p tree.
 */
void kdTree_rbal(kdTree * tree);

/*!
 * Print the Tree \p tree to \p stream.
 */
void kdTree_fprint(FILE * stream, const kdTree * tree, uint show);

void kdTree_reset_fbs(kdTree * tree);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
