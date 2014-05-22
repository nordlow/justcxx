#include "kdTree.h"
#include "stdio_x.h"
#include "bitwise.h"
#include "sortn.h"
#include "qsort_mt.h"
#include "pdim.h"

/* ========================================================================= */

/*!
 * Construct a pair of shapelimits from the shape \p a and return it.
 * \return the new pair as either a \c kdDup or a \c kdBPart.
 */
static kdNode *
kd_new_LIM_PAIR(struct kdTree * owner, kdNode * super, vis_t * a)
{
  kdLim *x = kdLim_new(owner, 0, a, 0);
  kdLim *y = kdLim_new(owner, 0, a, 1);

  box2f bnd; vis_rdBnd_box2f(a, &bnd);

  if (is_point(bnd)) {
    /* handle special case when box dimensions are zero */
    return (kdNode*)kdDup_new(owner, super, &x->this_point, &y->this_point);
  } else {
    float new_ppos = 0;
    pdim256_t new_pdim = maxsep_c(&bnd, &new_ppos);
    kdBPart *z = kdBPart_new(owner, super, new_pdim, new_ppos,
                             &x->this_point.this_node,
                             &y->this_point.this_node);
    return (kdNode*)z;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void
kdNode_init(kdNode * node,
            KDNODE_t ntp, struct kdTree * owner, kdNode * super,
            height_t height, layer_t min_layer, layer_t max_layer)
{
  node->ntp = ntp;
  node->owner = owner;
  node->super = super;
  node->height_tag = height;
  node->min_layer = min_layer;
  node->max_layer = max_layer;
}

kdNode *
kdNode_new(KDNODE_t ntp,
           struct kdTree * owner, kdNode * super,
           height_t height, layer_t min_layer, layer_t max_layer)
{
  kdNode *node = malloc(sizeof(kdNode));
  kdNode_init(node, ntp, owner, super, height, min_layer, max_layer);
  return node;
}

void
kdNode_clear(kdNode * node)
{
  node->owner = 0;		/* needed to detect bugs */
  node->super = 0;		/* needed to detect bugs */
}

void kdNode_rdel(kdNode * node, bool leaf_flag)
{
  switch (node->ntp) {
  case KD_POINT: kdPoint_rdel((kdPoint*)node, leaf_flag); break;
  case KD_REF: kdRef_rdel((kdRef*)node, leaf_flag); break;
  case KD_LIM: kdLim_rdel((kdLim*)node, leaf_flag); break;
  case KD_CBOX: kdCBox_rdel((kdCBox*)node, leaf_flag); break;
  case KD_DUP: kdDup_rdel((kdDup*)node, leaf_flag); break;
  case KD_BPART: kdBPart_rdel((kdBPart*)node, leaf_flag); break;
  default: break;
  }
}

int
kdNode_rdBnd_box2f(const kdNode * node, box2f * ext)
{
  int ret = 0;
  switch (node->ntp) {
  case KD_POINT: ext->l = ext->u = ((kdPoint*)node)->lpoint; ret = 1; break;
  case KD_REF: ext->l = ext->u = *((kdRef*)node)->vrd; ret = 1; break;
  case KD_LIM: vis_rdBnd_box2f(((kdLim*)node)->csh, ext); ret = 1; break;
  case KD_CBOX: vis_rdBnd_box2f(((kdCBox*)node)->bsh, ext); ret = 1; break;
  case KD_DUP: ext->l = ext->u = ((kdDup*)node)->dpp; ret = 1; break;
  case KD_BPART: *ext = ((kdBPart*)node)->pbx;
  default: ret = 0; break;
  }
  return ret;
}

size_t
kdNode_getSize(const kdNode * node)
{
  size_t sz;
  switch (node->ntp) {
  case KD_POINT: sz = kdPoint_getSize((kdPoint*)node); break;
  case KD_REF: sz = kdRef_getSize((kdRef*)node); break;
  case KD_LIM: sz = kdLim_getSize((kdLim*)node); break;
  case KD_CBOX: sz = kdCBox_getSize((kdCBox*)node); break;
  case KD_DUP: sz = kdDup_getSize((kdDup*)node); break;
  case KD_BPART: sz = kdBPart_getSize((kdBPart*)node); break;
  default: sz = 0; break;
  }
  return sz;
}

void
kdNode_fprint(FILE * stream, const kdNode * node, uint level)
{
  switch (node->ntp) {
  case KD_POINT: kdPoint_fprint(stream, (kdPoint*)node, level); break;
  case KD_REF: kdRef_fprint(stream, (kdRef*)node, level); break;
  case KD_LIM: kdLim_fprint(stream, (kdLim*)node, level); break;
  case KD_CBOX: kdCBox_fprint(stream, (kdCBox*)node, level); break;
  case KD_DUP: kdDup_fprint(stream, (kdDup*)node, level); break;
  case KD_BPART: kdBPart_fprint(stream, (kdBPart*)node, level); break;
  default: break;
  }
}

kdNode *
kdNode_ins_Node(kdNode * node, kdNode * a)
{
  switch (node->ntp) {
  case KD_POINT: node = kdPoint_ins_Node((kdPoint*)node, a); break;
  case KD_REF: node = kdRef_ins_Node((kdRef*)node, a); break;
  case KD_LIM: node = kdLim_ins_Node((kdLim*)node, a); break;
  case KD_CBOX: node = kdCBox_ins_Node((kdCBox*)node, a); break;
  case KD_DUP: node = kdDup_ins_Node((kdDup*)node, a); break;
  case KD_BPART: node = kdBPart_ins_Node((kdBPart*)node, a); break;
  default: break;
  }
  return node;
}

kdNode *
kdNode_ins_shape(kdNode * node, vis_t * a, LIMASK_t limask)
{
  switch (node->ntp) {
  case KD_POINT: node = kdPoint_ins_shape((kdPoint*)node, a, limask); break;
  case KD_REF: node = kdRef_ins_shape((kdRef*)node, a, limask); break;
  case KD_LIM: node = kdLim_ins_shape((kdLim*)node, a, limask); break;
  case KD_CBOX: leprintf("cannot handle %s-node!\n", kdNode_getName(node)); break;
  case KD_DUP: node = kdDup_ins_shape((kdDup*)node, a, limask); break;
  case KD_BPART: node = kdBPart_ins_shape((kdBPart*)node, a, limask); break;
  default: break;
  }
  return node;
}

const kdNode *
kdNode_find_vec(const kdNode * node, const vec2f * a)
{
  const kdNode * hit = 0;
  switch (node->ntp) {
  case KD_POINT: hit = kdPoint_find_vec((const kdPoint*)node, a); break;
  case KD_REF: hit = kdRef_find_vec((const kdRef*)node, a); break;
  case KD_LIM: hit = kdLim_find_vec((const kdLim*)node, a); break;
  case KD_CBOX: hit = kdCBox_find_vec((const kdCBox*)node, a); break;
  case KD_DUP: hit = kdDup_find_vec((const kdDup*)node, a); break;
  case KD_BPART: hit = kdBPart_find_vec((const kdBPart*)node, a); break;
  default: hit = 0; break;
  }
  return hit;
}

int
kdNode_find_shape(const kdNode * node, const vis_t * a,
                  LIMASK_t * limask, Npv * hits)
{
  int ret = 0;
  switch (node->ntp) {
  case KD_POINT: ret = kdPoint_find_shape((const kdPoint*)node, a, limask, hits); break;
  case KD_REF: ret = kdPoint_find_shape((const kdPoint*)node, a, limask, hits); break;
  case KD_LIM: ret = kdLim_find_shape((const kdLim*)node, a, limask, hits); break;
  case KD_CBOX: ret = kdCBox_find_shape((const kdCBox*)node, a, limask, hits); break;
  case KD_DUP: ret = kdDup_find_shape((const kdDup*)node, a, limask, hits); break;
  case KD_BPART: ret = kdBPart_find_shape((const kdBPart*)node, a, limask, hits); break;
  default: ret = 0; break;
  }
  return ret;
}

kdNode *
kdNode_rm_vec(kdNode * node, vec2f * a, size_t *rmcnt_ret)
{
  switch (node->ntp) {
  case KD_POINT: node = kdPoint_rm_vec((kdPoint*)node, a, rmcnt_ret); break;
  case KD_REF: node = kdRef_rm_vec((kdRef*)node, a, rmcnt_ret); break;
  case KD_LIM: node = kdLim_rm_vec((kdLim*)node, a, rmcnt_ret); break;
  case KD_CBOX: node = kdCBox_rm_vec((kdCBox*)node, a, rmcnt_ret); break;
  case KD_DUP: node = kdDup_rm_vec((kdDup*)node, a, rmcnt_ret); break;
  case KD_BPART: node = kdBPart_rm_vec((kdBPart*)node, a, rmcnt_ret); break;
  default: break;
  }
  return node;
}

kdNode *
kdNode_rm_shape(kdNode * node, vis_t * a,
                LIMASK_t * limask, size_t *rmcnt_ret)
{
  switch (node->ntp) {
  case KD_POINT: node = kdPoint_rm_shape((kdPoint*)node, a, limask, rmcnt_ret); break;
  case KD_REF: node = kdPoint_rm_shape((kdPoint*)node, a, limask, rmcnt_ret); break;
  case KD_LIM: node = kdLim_rm_shape((kdLim*)node, a, limask, rmcnt_ret); break;
  case KD_CBOX: node = kdCBox_rm_shape((kdCBox*)node, a, limask, rmcnt_ret); break;
  case KD_DUP: node = kdDup_rm_shape((kdDup*)node, a, limask, rmcnt_ret); break;
  case KD_BPART: node = kdBPart_rm_shape((kdBPart*)node, a, limask, rmcnt_ret); break;
  default: break;
  }
  return node;
}

kdNode * kdNode_tearUp(kdNode * node,
                       pdim256_t pdim, float ppos,
                       Npv * sn0_ret, Npv * sn1_ret)
{
  kdNode * ret = NULL;
  switch (node->ntp) {
  case KD_POINT: kdPoint_tearUp((kdPoint*)node, pdim, ppos, sn0_ret, sn1_ret); break;
  case KD_REF: kdPoint_tearUp((kdPoint*)node, pdim, ppos, sn0_ret, sn1_ret); break;
  case KD_LIM: kdPoint_tearUp((kdPoint*)node, pdim, ppos, sn0_ret, sn1_ret); break;
  case KD_CBOX: kdCBox_tearUp((kdCBox*)node, pdim, ppos, sn0_ret, sn1_ret); break;
  case KD_DUP: kdDup_tearUp((kdDup*)node, pdim, ppos, sn0_ret, sn1_ret); break;
  case KD_BPART: kdBPart_tearUp((kdBPart*)node, pdim, ppos, sn0_ret, sn1_ret); break;
  default: break;
  }
  return ret;
}

kdNode * kdNode_bal(kdNode * node)
{
  kdNode * ret = node;
  switch (node->ntp) {
  case KD_POINT: break;
  case KD_REF: break;
  case KD_LIM: break;
  case KD_CBOX: break;
  case KD_DUP: break;
  case KD_BPART: kdBPart_bal((kdBPart*)node); break;
  default: break;
  }
  return ret;
}

kdNode * kdNode_rbal(kdNode * node)
{
  kdNode * ret = node;
  switch (node->ntp) {
  case KD_POINT: break;
  case KD_REF: break;
  case KD_LIM: break;
  case KD_CBOX: break;
  case KD_DUP: break;
  case KD_BPART: ret = kdBPart_rbal((kdBPart*)node); break;
  default: break;
  }
  return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

void
kdPoint_init(kdPoint * pnt,
             struct kdTree * owner, kdNode * super, const vec2f * a,
             layer_t min_layer, layer_t max_layer)
{
  kdNode_init(&pnt->this_node, KD_POINT, owner, super, 0, min_layer, max_layer);
  pnt->lpoint = *a;
  pnt->this_node.owner->point_cnt++;
}

kdPoint *
kdPoint_new(struct kdTree * owner, kdNode * super, const vec2f * a,
            layer_t min_layer, layer_t max_layer)
{
  kdPoint * pnt = malloc(sizeof(kdPoint));
  kdPoint_init(pnt, owner, super, a, min_layer, max_layer);
  return pnt;
}

void
kdPoint_clear(kdPoint * pnt)
{
  pnt->this_node.owner->point_cnt--;
}

kdNode *
kdPoint_ins_Node(kdPoint * pnt, kdNode * a)
{
  kdNode * ret = &pnt->this_node;
  switch (a->ntp) {
  case KD_POINT:		/* kdPoint + kdPoint */
  case KD_REF: 			/* kdPoint + kdRef */
  case KD_LIM:			/* kdPoint + kdLim */
    ret = kdPoint_ins_Point(pnt, (kdPoint*)a);
    break;
  case KD_CBOX:			/* kdPoint + kdCBox */
    ret = kdCBox_ins_Node((kdCBox*)a, (kdNode*)pnt);
    break;
  case KD_DUP: 			/* kdPoint + kdDup */
    ret = kdDup_ins_Point((kdDup*)a, (kdPoint*)pnt);
    break;
  case KD_BPART: {			/* kdPoint + kdBPart */
    kdBPart *bp = (kdBPart*)a;
    if (bp->bph == BPH_LIM_PAIR) {
      /* \TODO: Move case from insert(vis_t * a, LIMASK_t limask) here */
      /* and reuse this case from insert(vis_t * a, LIMASK_t limask) */
    }
    ret = kdBPart_ins_Node(bp, (kdNode*)pnt);
    break;
  }
  default:
    leprintf("cannot handle %s-node!\n", kdNode_getName(a));
    break;
  }
  return ret;
}

kdNode *
kdPoint_ins_Point(kdPoint * pnt, kdPoint * a)
{
  float new_ppos = 0;
  int do_swap = 0;
  pdim256_t new_pdim = maxsep_vv(&pnt->lpoint, &a->lpoint, &new_ppos, &do_swap);

  if (new_pdim < DIMNUM) {	       /* partitioning possible */
    kdPoint *x = pnt;
    kdPoint *y = a;
    if (do_swap) { ptr_swap((void**)&x, (void**)&y); }

    return (kdNode*)kdBPart_new(pnt->this_node.owner, 0,
                                new_pdim, new_ppos, (kdNode*)x, (kdNode*)y);
  } else {			/* partitioning not possible */
    return (kdNode*)kdDup_new(pnt->this_node.owner, 0,
                              pnt, a);
  }
}

kdNode *
kdPoint_ins_shape(kdPoint * pnt, vis_t * a, LIMASK_t limask)
{
  if (limask == BOTH_LIMS) {
    return kdNode_ins_Node((kdNode*)pnt,
                           kd_new_LIM_PAIR(pnt->this_node.owner, 0, a));
  } else if (limask) {
    return kdNode_ins_Node((kdNode*)pnt,
                           (kdNode*)kdLim_new(pnt->this_node.owner, 0, a,
                                              limask == LO_LIM ? 0 : 1));
  } else {
    PWARN("limask is zero\n");
    return (kdNode*)pnt;
  }
}

kdNode *
kdPoint_extractAt(kdPoint * pnt, const box2f * a)
{
  return box2f_includes(a, &pnt->lpoint) ? &pnt->this_node : 0;
}

const kdNode *
kdPoint_find_vec(const kdPoint * pnt, const vec2f * a)
{
  return 0;
}

int
kdPoint_find_shape(const kdPoint * pnt, const vis_t * a,
                   LIMASK_t * limask, Npv * hits)
{
  if (pnt->this_node.ntp == KD_LIM) {
    PERR("kdPoint is actually kdLim!\n");
  }
  return 0;		       /* this kdNode does not contain any shape */
}

kdNode *
kdPoint_rm_vec(kdPoint * pnt, vec2f * a, size_t *rmcnt_ret)
{
  return (kdNode*)pnt;
}

kdNode *
kdPoint_rm_shape(kdPoint * pnt, vis_t * a, LIMASK_t * limask, size_t *rmcnt_ret)
{
  return (kdNode*)pnt;	   /* this is not a kdLim (yet) so do nothing */
}

void
kdPoint_tearUp(kdPoint * pnt,
               pdim256_t pdim, float ppos,
               Npv * sn0_ret, Npv * sn1_ret)
{
  if (vec2f_at(&pnt->lpoint, pdim) < ppos) {
    Npv_app(sn0_ret, &pnt->this_node);
  } else {
    Npv_app(sn1_ret, &pnt->this_node);
  }
}

void
kdPoint_fprint(FILE * stream, const kdPoint * pnt, uint level)
{
  mputspace(stream, 2 * level);
  fprintf(stream, "POINT height=%d point=", kdNode_getHeight(&pnt->this_node));
  vec2f_fprint(stream, &pnt->lpoint);
  fendline(stream);
}

/* ---------------------------- Group Separator ---------------------------- */

void
kdRef_init(kdRef * ref, struct kdTree * owner, kdNode * super,
           const vec2f * a)
{
  kdPoint_init(&ref->this_point, owner, super, a, 0, 0);
  ref->this_point.this_node.ntp = KD_REF; /* override type */
  ref->vrd = (vec2f*)a;			  /* set reference */
  ref->this_point.this_node.owner->ref_cnt++;
}

kdRef *
kdRef_new(struct kdTree * owner, kdNode * super,
          const vec2f * a)
{
  kdRef * ref = malloc(sizeof(kdRef));
  kdRef_init(ref, owner, super, a);
  return ref;
}

void
kdRef_clear(kdRef * ref)
{
  ref->this_point.this_node.owner->ref_cnt--;
}

kdNode *
kdRef_ins_Node(kdRef * ref, kdNode * a)
{
  if (a->ntp == KD_REF) {
    return kdRef_ins_Ref(ref, (kdRef*)a);
  } else {
    return kdPoint_ins_Node(&ref->this_point, a);
  }
}

kdNode *
kdRef_ins_Ref(kdRef * ref, kdRef * a)
{
  if (ref->vrd != a->vrd) {
    return kdPoint_ins_Node(&ref->this_point, (kdNode*)a);
  } else {
    PWARN("reference already inserted!\n");
    return (kdNode*)ref;
  }
}

kdNode *
kdRef_ins_shape(kdRef * ref, vis_t * a, LIMASK_t limask)
{
  return kdPoint_ins_shape(&ref->this_point, a, limask);
}

const kdNode *
kdRef_find_vec(const kdRef * ref, const vec2f * a)
{
  if (ref->vrd == a) {
    return &ref->this_point.this_node;
  } else {
    return 0;
  }
}

kdNode *
kdRef_rm_vec(kdRef * ref, vec2f * a, size_t *rmcnt_ret)
{
  if (ref->vrd == a) {
    (*rmcnt_ret)++;
    kdRef_delete(ref);
    return 0;
  } else {
    return (kdNode*)ref;
  }
}

void
kdRef_fprint(FILE * stream, kdRef * ref, uint level)
{
  mputspace(stream, 2 * level);
  fprintf(stream,
          "REF height=%d point=", kdNode_getHeight(&ref->this_point.this_node));
  vec2f_fprint(stream, &ref->this_point.lpoint);
  fprintf(stream, " vrd=");
  vec2f_fprint(stream, ref->vrd);
  fendline(stream);
}

/* ---------------------------- Group Separator ---------------------------- */

void
kdLim_init(kdLim * lim, struct kdTree * owner, kdNode * super,
           vis_t * a, uint cidx)
{
  vec2f bnd = cidx == 0 ? vis_getBLim(a, 0) : vis_getBLim(a, 1);
  kdPoint_init(&lim->this_point, owner, super, &bnd, a->slayer, a->slayer);
  lim->this_point.this_node.ntp = KD_LIM; /* override type */
  lim->csh = a;
  lim->cidx = cidx;
  lim->this_point.this_node.owner->lim_cnt++;
}

kdLim *
kdLim_new(struct kdTree * owner, kdNode * super, vis_t * a, uint cidx)
{
  kdLim * lim = malloc(sizeof(kdLim));
  kdLim_init(lim, owner, super, a, cidx);
  return lim;
}

void
kdLim_clear(kdLim * lim)
{
  kdPoint_clear(&lim->this_point);
  lim->this_point.this_node.owner->lim_cnt--;
}

const kdNode *
kdLim_find_vec(const kdLim * lim, const vec2f * a)
{
  return 0;		       /* this points to a shape and not to a vec */
}

kdNode *
kdLim_ins_shape(kdLim * lim, vis_t * a, LIMASK_t limask)
{
  if (lim->csh == a &&
      ((lim->cidx == 0 && limask == UP_LIM) ||
       (lim->cidx == 1 && limask == LO_LIM))) {
    kdTree * owner = lim->this_point.this_node.owner;
    kdLim_delete(lim);
    return (kdNode*)kdCBox_new(owner, 0, a);
  } else {
    return (kdNode*)kdPoint_ins_shape(&lim->this_point, a, limask);
  }
}

kdNode *
kdLim_ins_Lim(kdLim * lim, kdLim * a)
{
  if (lim->csh == a->csh) {
    /* limits for the same box */

    if (lim->cidx == a->cidx) {
      PERR("limit has already been inserted\n");
      return (kdNode*)lim;
    }

    /* we are inserting the other shape limit so remove these two */
    kdLim_delete(lim);
    kdTree * owner = a->this_point.this_node.owner;
    vis_t * sh = a->csh;
    kdLim_delete(a);

    /* and create the new shape container box instead */
    return (kdNode*)kdCBox_new(owner, 0, sh);
  } else {
    return (kdNode*)kdPoint_ins_Node(&lim->this_point, (kdNode*)a);
  }
}

kdNode *
kdLim_ins_Node(kdLim * lim, kdNode * a)
{
  if (a->ntp == KD_LIM) {
    return kdLim_ins_Lim(lim, (kdLim*)a);
  } else {
    return kdPoint_ins_Node(&lim->this_point, a);
  }
}

int
kdLim_find_shape(const kdLim * lim, const vis_t * a,
                 LIMASK_t * limask, Npv * hits)
{
  int ret = 0;
  if (lim->csh == a &&
      GET_BIT(*limask, lim->cidx)) {
    CLR_BIT(*limask, lim->cidx);
    Npv_app(hits, (kdNode*)&lim->this_point.this_node);
    ret = 1;
  }
  return ret;
}

kdNode *
kdLim_rm_vec(kdLim * lim, vec2f * a, size_t *rmcnt_ret)
{
  return (kdNode*)lim;
}

kdNode *
kdLim_rm_shape(kdLim * lim, vis_t * a, LIMASK_t * limask, size_t *rmcnt_ret)
{
  kdNode * ret = (kdNode*)lim;
  if (lim->csh == a &&
      GET_BIT(*limask, lim->cidx)) {
    CLR_BIT(*limask, lim->cidx);
    (*rmcnt_ret)++;
    kdLim_delete(lim);
    ret = 0;
  }
  return ret;
}

void
kdLim_fprint(FILE * stream, kdLim * lim, uint level)
{
  box2f bnd;

  mputspace(stream, 2 * level);
  fprintf(stream, "LIM height=%d point=",
          kdNode_getHeight(&lim->this_point.this_node));
  vec2f_fprint(stream, &lim->this_point.lpoint);

  fprintf(stream, " csh(%p)= ", lim->csh);
  vis_fprint(stream, lim->csh);

  fprintf(stream, " bnd=");
  vis_rdBnd_box2f(lim->csh, &bnd);
  box2f_fprint(stream, &bnd);

  fprintf(stream, " cidx=%d", lim->cidx);
  fendline(stream);
}

/* ---------------------------- Group Separator ---------------------------- */

void
kdCBox_init(kdCBox * cbx, struct kdTree * owner, kdNode * super, vis_t * a)
{
  kdNode_init(&cbx->this_node, KD_CBOX, owner, super, 0, a->slayer, a->slayer);
  cbx->bsh = a;
  cbx->this_node.owner->cbox_cnt++;
}

kdCBox *
kdCBox_new(struct kdTree * owner, kdNode * super, vis_t * a)
{
  kdCBox * cbx = malloc(sizeof(kdCBox));
  kdCBox_init(cbx, owner, super, a);
  return cbx;
}

void
kdCBox_clear(kdCBox * cbx)
{
  cbx->this_node.owner->cbox_cnt--;
}

const kdNode *
kdCBox_find_vec(const kdCBox * cbx, const vec2f * a)
{
  return 0;		       /* this points to a shape and not to a vec */
}

kdNode *
kdCBox_ins_Node(kdCBox * cbx, kdNode * a)
{
  float new_ppos = 0;
  int do_swap = 0;
  box2f cbx_bnd, a_bnd;

  vis_rdBnd_box2f(cbx->bsh, &cbx_bnd);
  kdNode_rdBnd_box2f(a, &a_bnd);

  pdim256_t new_pdim = maxsep_bb(&cbx_bnd, &a_bnd, &new_ppos, &do_swap);

  if (new_pdim < DIMNUM) {	       /* partitioning possible */
    kdNode *x = &cbx->this_node;
    kdNode *y = a;
    if (do_swap) { ptr_swap((void**)&x, (void**)&y); }

    return (kdNode*)kdBPart_new(cbx->this_node.owner, 0,
                                new_pdim, new_ppos, x, y);
  } else {
    /* we split up the container box into two limits. */
    kdNode *bp = kd_new_LIM_PAIR(cbx->this_node.owner, 0, cbx->bsh); /* transform this */
    kdCBox_rdel(cbx, 0);     /* 0 cause we still want cbx->bsh */
    return kdNode_ins_Node(bp, a);
  }
}

kdNode *
kdCBox_extractAt(kdCBox * cbx, const box2f * a)
{
  return vis_overlap(cbx->bsh, a) ? &cbx->this_node : 0;
}

int
kdCBox_find_shape(const kdCBox * cbx, const vis_t * a,
                  LIMASK_t * limask, Npv * hits)
{
  int ret = 0;
  if (cbx->bsh == a) {
    if (*limask == BOTH_LIMS) {
      Npv_app(hits, (kdNode*)&cbx->this_node);
      ret = 2;			/* two limits found */
    } else if (*limask == LO_LIM) {
      Npv_app(hits, (kdNode*)&cbx->this_node);
      ret = 1;			/* one limit found */
    } else if (*limask == UP_LIM) {
      Npv_app(hits, (kdNode*)&cbx->this_node);
      ret = 1;			/* one limit found */
    }
    *limask = 0;		/* remove all limits */
  }
  return ret;
}

kdNode *
kdCBox_rm_vec(kdCBox * cbx, vec2f * a, size_t *rmcnt_ret)
{
  /* do nothing */
  return &cbx->this_node;
}

kdNode *
kdCBox_rm_shape(kdCBox * cbx, vis_t * a, LIMASK_t * limask, size_t *rmcnt_ret)
{
  kdNode * ret = (kdNode*)cbx;
  if (cbx->bsh == a) {
    kdTree * owner = cbx->this_node.owner;
    kdCBox_delete(cbx);
    (*rmcnt_ret)++;
    if (*limask == BOTH_LIMS) {
      *limask = 0;
      ret = 0;
    } else {
      PWARN("no sense in removing one limit from a CBox! doing it anyway.\n");
      uint cidx = (*limask == LO_LIM) ? 0 : 1;
      CLR_BIT(*limask, cidx);
      ret = (kdNode*)kdLim_new(owner, 0, a, *limask == LO_LIM ? 1 : 0);
    }
  }
  return ret;
}

void
kdCBox_tearUp(kdCBox * cbx,
              pdim256_t pdim, float ppos,
              Npv * sn0_ret, Npv * sn1_ret)
{
  box2f bnd; vis_rdBnd_box2f(cbx->bsh, &bnd);

  if (box2f_at2(&bnd, 1, pdim) < ppos) { /* whole box to the "left" */
    Npv_app(sn0_ret, &cbx->this_node);
  } else if (ppos <= box2f_at2(&bnd, 0, pdim)) { /* whole box to the "right" */
    Npv_app(sn1_ret, &cbx->this_node);
  } else {	/* kdCBox overlaps partition "line" at ppos along pdim */
    /* divide it up into two shape limits */
    Npv_app(sn0_ret, (kdNode*)kdLim_new(cbx->this_node.owner, 0, cbx->bsh, 0));
    Npv_app(sn1_ret, (kdNode*)kdLim_new(cbx->this_node.owner, 0, cbx->bsh, 1));
    kdCBox_rdel(cbx, 0);
  }
}

void
kdCBox_fprint(FILE * stream, const kdCBox * cbx, uint level)
{
  box2f bnd;

  mputspace(stream, 2 * level);
  fprintf(stream, "CBOX height=%d", kdNode_getHeight(&cbx->this_node));

  fprintf(stream, " csh=");
  vis_fprint(stream, cbx->bsh);

  fprintf(stream, " bnd=");
  vis_rdBnd_box2f(cbx->bsh, &bnd);
  box2f_fprint(stream, &bnd);

  fendline(stream);
}

/* ---------------------------- Group Separator ---------------------------- */

void
kdDup_init(kdDup * dup,
           struct kdTree * owner, kdNode * super, kdPoint * a, kdPoint * b)
{
  kdNode_init(&dup->this_node, KD_DUP, owner, super, 1,
              MIN2(a->this_node.min_layer,
                   b->this_node.min_layer),
              MAX2(a->this_node.max_layer,
                   b->this_node.max_layer));

  dup->dpp = a->lpoint;

  Npv_initN(&dup->pts, 2);
  dup->pts.a[0] = (kdNode*)a;
  dup->pts.a[1] = (kdNode*)b;

  dup->this_node.owner->dup_cnt++;
}

kdDup *
kdDup_new(struct kdTree * owner, kdNode * super, kdPoint * a, kdPoint * b)
{
  kdDup * dup = malloc(sizeof(kdDup));
  kdDup_init(dup, owner, super, a, b);
  return dup;
}

void
kdDup_clear(kdDup * dup)
{
  Npv_clear(&dup->pts);
  dup->this_node.owner->dup_cnt--;
}

kdNode *
kdDup_ins_Point(kdDup * dup, kdPoint * a)
{
  float new_ppos = 0;
  int do_swap = 0;
  pdim256_t new_pdim = maxsep_vv(&dup->dpp, &a->lpoint, &new_ppos, &do_swap);

  if (new_pdim < DIMNUM) {	       /* partitioning possible */
    kdNode *x = &dup->this_node;
    kdNode *y = (kdNode*)a;
    if (do_swap) { ptr_swap((void**)&x, (void**)&y); }

    return (kdNode*)kdBPart_new(dup->this_node.owner, 0,
                                new_pdim, new_ppos, x, y);
  } else {		       /* partitioning not possible */
    /* point a must lies at duplication point so add it */
    size_t hit_i = Npv_findF(&dup->pts, (kdNode*)a);
    if (hit_i == dup->pts.n) {
      Npv_app(&dup->pts, &a->this_node);
    } else {
      PERR("a has already been inserted\n");
    }

    return &dup->this_node;
  }

}

kdNode *
kdDup_ins_Dup(kdDup * dup, kdDup * a)
{
  if (vec2f_eq(&dup->dpp, &a->dpp)) {
    PTEST("merging two kdDup's\n");
    /* append duplication points from the smaller to the greater */

    if (dup->pts.n >= a->pts.n) { /* this is the larger than a */
      Npv_appM(&dup->pts, a->pts.a, a->pts.n);
      kdDup_delete(a);
      return &dup->this_node;
    } else {		       /* this is smaller than a */
      Npv_appM(&a->pts, dup->pts.a, dup->pts.n);
      kdDup_delete(dup);
      return &a->this_node;
    }
  } else {
    float new_ppos = 0;
    int do_swap = 0;
    pdim256_t new_pdim = maxsep_vv(&dup->dpp, &a->dpp, &new_ppos, &do_swap);

    if (new_pdim < DIMNUM) {	       /* partitioning possible */
      kdDup *x = dup;
      kdDup *y = a;
      if (do_swap) { ptr_swap((void**)&x, (void**)&y); }

      return (kdNode*)kdBPart_new(dup->this_node.owner, 0,
                                  new_pdim, new_ppos,
                                  (kdNode*)x, (kdNode*)y);
    } else {
      PERR("a pdim should have been found\n");
      return &dup->this_node;
    }
  }
}

kdNode *
kdDup_ins_shape(kdDup * dup, vis_t * a, LIMASK_t limask)
{
  kdNode * ret = (kdNode*)dup;
  if (limask == BOTH_LIMS) {
    ret = kdDup_ins_Node(dup, kd_new_LIM_PAIR(dup->this_node.owner, 0, a));
  } else if (limask) {
    ret = kdDup_ins_Node(dup, (kdNode*)kdLim_new(dup->this_node.owner, 0, a,
                                                 limask == LO_LIM ? 0 : 1));
  } else {
    PWARN("limask is zero\n");
  }
  return ret;
}

kdNode *
kdDup_ins_Node(kdDup * dup, kdNode * a)
{
  kdNode * ret = &dup->this_node;
  switch (a->ntp) {
  case KD_POINT:		/* kdDup + kdPoint */
  case KD_REF:			/* kdDup + kdRef */
  case KD_LIM:			/* kdDup + kdLim */
    ret = kdDup_ins_Point(dup, (kdPoint*)a);
    break;
  case KD_CBOX:  			/* kdDup + kdCBox */
    kdCBox_ins_Node((kdCBox*)a, (kdNode*)dup);
    break;
  case KD_DUP: 			/* kdDup + kdDup */
    ret = kdDup_ins_Dup(dup, (kdDup*)a);
    break;
  case KD_BPART: {			/* kdDup + kdBPart */
    kdBPart *bp = (kdBPart*)a;
    if (bp->bph == BPH_LIM_PAIR) {
      /* \TODO: Move case from insert(vis_t * a, LIMASK_t limask) here */
      /* and reuse this case from insert(vis_t * a, LIMASK_t limask) */
    }
    ret = kdBPart_ins_Node(bp, (kdNode*)dup);
    break;
  }
  default:
    leprintf("cannot handle %s-node!\n", kdNode_getName(a));
    break;
  }
  return &dup->this_node;
}

kdNode *
kdDup_extractAt(kdDup * dup, const box2f * a)
{
  return box2f_includes(a, &dup->dpp) ? &dup->this_node : 0;
}

const kdNode *
kdDup_find_vec(const kdDup * dup, const vec2f * a)
{
  for (size_t i = 0; i < dup->pts.n; i++) {
    const kdPoint *po = (const kdPoint*)dup->pts.a[i];
    const kdNode *hit = kdPoint_find_vec(po, a);
    if (hit) { return hit; }
  }

  return 0;
}

int
kdDup_find_shape(const kdDup * dup, const vis_t * a,
                 LIMASK_t * limask, Npv * hits)
{
  int ret = 0;
  for (size_t i = 0; i < dup->pts.n && *limask; i++) {
    const kdNode * sub_node = dup->pts.a[i];
    if (sub_node->ntp == KD_LIM) {
      const kdLim *bl = (const kdLim*)sub_node;
      ret += kdLim_find_shape(bl, a, limask, hits);
    }
  }
  return ret;
}

kdNode *
kdDup_compress(kdDup * dup)
{
  if (dup->pts.n == 0) {
    kdDup_delete(dup);
    return 0;
  } else if (dup->pts.n == 1) {
    kdPoint *single = (kdPoint*)dup->pts.a[0];
    kdDup_delete(dup);
    return (kdNode*)single;
  } else {
    return &dup->this_node;
  }
}

kdNode *
kdDup_rm_vec(kdDup * dup, vec2f * a, size_t *rmcnt_ret)
{
  for (size_t i = 0; i < dup->pts.n; i++) {
    KDNODE_t sub_ntp = dup->pts.a[i]->ntp;
    if (sub_ntp == KD_POINT ||
        sub_ntp == KD_REF ||
        sub_ntp == KD_LIM) {
      dup->pts.a[i] = kdPoint_rm_vec((kdPoint*)dup->pts.a[i], a, rmcnt_ret);
    } else {
      PERR("tree structure: sub node was nor a POINT, REF or LIM.\n");
    }
  }
  Npv_pack(&dup->pts);
  return kdDup_compress(dup);
}

kdNode *
kdDup_rm_shape(kdDup * dup, vis_t * a, LIMASK_t * limask, size_t *rmcnt_ret)
{
  kdNode * ret = (kdNode*)dup;
  for (size_t i = 0; i < dup->pts.n && *limask; i++) {
    const kdNode * sub_node = dup->pts.a[i];
    if (sub_node->ntp == KD_LIM) {
      dup->pts.a[i] = kdNode_rm_shape(dup->pts.a[i], a, limask, rmcnt_ret);
    }
  }
  Npv_pack(&dup->pts);
  ret = kdDup_compress(dup);
  return ret;
}

void
kdDup_tearUp(kdDup * dup,
             pdim256_t pdim, float ppos,
             Npv * sn0_ret, Npv * sn1_ret)
{
  if (vec2f_at(&dup->dpp, pdim) < ppos) {
    Npv_app(sn0_ret, &dup->this_node);
  } else {
    Npv_app(sn1_ret, &dup->this_node);
  }
}

void
kdDup_fprint(FILE * stream, const kdDup * dup, uint level)
{
  mputspace(stream, 2 * level);
  fprintf(stream, "DUP");
  fprintf(stream, " height=%d", kdNode_getHeight(&dup->this_node));
  fprintf(stream, " point=");
  vec2f_fprint(stream, &dup->dpp);
  fprintf(stream, " pts_size=%d", (int)dup->pts.n);
  fendline(stream);

  for (size_t i = 0; i != dup->pts.n; i++) {
    kdNode_fprint(stream, dup->pts.a[i], level + 1);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

static void
kdBPart_calcBPH(kdBPart * bpt)
{
  kdNode *p0 = bpt->part[0];
  kdNode *p1 = bpt->part[1];
  BPH_t bph = BPH_ANY;
  if (p0->ntp == KD_POINT &&
      p1->ntp == KD_POINT) {
    bph = BPH_POINT_PAIR;
  } else if (p0->ntp == KD_LIM &&
             p1->ntp == KD_LIM) {
    kdLim *l0 = (kdLim*)p0;
    kdLim *l1 = (kdLim*)p1;
    if (l0->csh == l1->csh) {
      bpt->bph = BPH_LIM_PAIR;      /* a new pair of limits detected */
    }
  }
  bpt->bph = bph;
}

void
kdBPart_init(kdBPart * bpt,
             struct kdTree * owner, kdNode * super,
             pdim256_t pdim, float ppos, kdNode * b0, kdNode * b1)
{
  box2f b0_bnd, b1_bnd;

  kdNode_init(&bpt->this_node, KD_BPART, owner, super,
              MAX2(kdNode_getHeight(b0),
                   kdNode_getHeight(b1)) + 1,
              MIN2(b0->min_layer, b1->min_layer),
              MAX2(b0->max_layer, b1->max_layer));

  kdNode_rdBnd_box2f(b0, &b0_bnd);
  kdNode_rdBnd_box2f(b1, &b1_bnd);

  box2f_unite(&b0_bnd, &b1_bnd, &bpt->pbx);
  bpt->pdim = pdim;
  bpt->ppos = ppos;
  bpt->part[0] = b0;
  bpt->part[1] = b1;

  bpt->this_node.owner->bpt_cnts[pdim]++;

  kdBPart_calcBPH(bpt);

  if (bpt->bph == BPH_LIM_PAIR) {
    bpt->this_node.owner->lopair_cnt++;
  }
}

kdBPart *
kdBPart_new(struct kdTree * owner, kdNode * super,
            pdim256_t pdim, float ppos, kdNode * b0, kdNode * b1)
{
  kdBPart * bpt = malloc(sizeof(kdBPart));
  kdBPart_init(bpt, owner, super, pdim, ppos, b0, b1);
  return bpt;
}

void
kdBPart_clear(kdBPart * bpt)
{
  if (bpt->bph == BPH_LIM_PAIR) {
    bpt->this_node.owner->lopair_cnt--;
  }
  bpt->this_node.owner->bpt_cnts[bpt->pdim]--;
}

kdNode *
kdBPart_ins_shape(kdBPart * bpt, vis_t * a, LIMASK_t limask)
{
  box2f a_bnd;
  vis_rdBnd_box2f(a, &a_bnd);

  if (limask == BOTH_LIMS) {	/* if we are inserting two shlims */
    if (box2f_at2(&a_bnd, 1, bpt->pdim) < bpt->ppos) {
      /* both shlims lie on the "left" */
      bpt->part[0] = kdNode_ins_shape(bpt->part[0], a, limask); /* recurse */
    } else if (bpt->ppos <= box2f_at2(&a_bnd, 0, bpt->pdim)) {
      /* both shlims lie on the "right" */
      bpt->part[1] = kdNode_ins_shape(bpt->part[1], a, limask); /* recurse */
    } else {
      /* shlims are separated by this kdBPart */
      if (box2f_at2(&a_bnd, 0, bpt->pdim) < bpt->ppos) {
        bpt->part[0] = kdNode_ins_shape(bpt->part[0], a, LO_LIM);
        bpt->part[1] = kdNode_ins_shape(bpt->part[1], a, UP_LIM);
      } else {
        PERR("tree structure: " "a(LO_LIM) not < ppos\n");
        bpt->part[1] = kdNode_ins_shape(bpt->part[1], a, LO_LIM);
        bpt->part[0] = kdNode_ins_shape(bpt->part[0], a, UP_LIM);
      }
    }
  } else if (limask) {	      /* if we are inserting one kdLim only */
    /* a kdPoint always fits into one of the sides */
    uint cidx = (limask == LO_LIM) ? 0 : 1;
    if (box2f_at2(&a_bnd, cidx, bpt->pdim) < bpt->ppos) { /* the "left" subtree or */
      bpt->part[0] = kdNode_ins_shape(bpt->part[0], a, limask);
    } else {			/* the "right" subtree */
      bpt->part[1] = kdNode_ins_shape(bpt->part[1], a, limask);
    }
  } else {
    PWARN("limask is zero\n");
  }

  kdBPart_updateAll(bpt);
  return kdBPart_bal(bpt);
}

kdNode *
kdBPart_ins_Point(kdBPart * bpt, kdPoint * pnt)
{
  kdNode * ret = NULL;
  float new_ppos = 0;
  int do_swap = 0;
  pdim256_t new_pdim = maxsep_vb(&pnt->lpoint, &bpt->pbx, &new_ppos, &do_swap);

  if (new_pdim < DIMNUM) {		/* partitioning possible */
    kdNode *x = (kdNode*)pnt;
    kdNode *y = &bpt->this_node;
    if (do_swap) { ptr_swap((void**)&x, (void**)&y); }

    ret = (kdNode*)kdBPart_new(bpt->this_node.owner, 0,
                               new_pdim, new_ppos, x, y);
    if (ret) { ret = kdNode_bal(ret); }
  }
  return ret;
}

int
kdBPart_check_pbx(kdBPart * bpt)
{
  box2f p0_bnd; kdNode_rdBnd_box2f(bpt->part[0], &p0_bnd);
  box2f p1_bnd; kdNode_rdBnd_box2f(bpt->part[1], &p1_bnd);
  box2f bnd; box2f_unite(&p0_bnd, &p1_bnd, &bnd);
  if (box2f_neq(&bnd, &bpt->pbx)) {
    PERR("pbx differs from union of partitions pbx's\n");
    return 0;
  }
  return 1;
}

int
kdNode_check_pbx(kdNode * node)
{
  int ret = 0;
  switch (node->ntp) {
  case KD_POINT: ret = 1; break;
  case KD_REF: ret = 1; break;
  case KD_LIM: ret = 1; break;
  case KD_CBOX: ret = 1; break;
  case KD_DUP: ret = 1; break;
  case KD_BPART: ret = kdBPart_check_pbx((kdBPart*)node); break;
  default: break;
  }
  return ret;
}

kdNode *
kdBPart_ins_Node_relate_unions(kdBPart * bpt, kdNode * a)
{
  kdNode * ret = NULL;

  box2f p0_bnd; kdNode_rdBnd_box2f(bpt->part[0], &p0_bnd);
  box2f p1_bnd; kdNode_rdBnd_box2f(bpt->part[1], &p1_bnd);
  box2f a_bnd; kdNode_rdBnd_box2f(a, &a_bnd);

  /* kdBPart + kdBPart */
  uint min_i = box2f_relate_unions(&p0_bnd, &a_bnd, &p1_bnd);

  if (min_i == 0 ||	 /* union(p0_bnd, a_bnd) lies beside p1_bnd */
      min_i == 1) {	 /* union(a_bnd, p1_bnd) lies beside p0_bnd */
    bpt->part[min_i] = kdNode_ins_Node(bpt->part[min_i], a);
    kdBPart_updateAll(bpt);

    kdNode_rdBnd_box2f(bpt->part[0], &p0_bnd);
    kdNode_rdBnd_box2f(bpt->part[1], &p1_bnd);

    float new_ppos = 0;
    int do_swap = 0;
    pdim256_t new_pdim = maxsep_bb(&p0_bnd, &p1_bnd, &new_ppos, &do_swap);

    if (new_pdim < DIMNUM) {	       /* partitioning possible */
      kdBPart_repartAt(bpt, new_pdim, new_ppos);
      if (do_swap) { ptr_swap((void**)&bpt->part[0], (void**)&bpt->part[1]); }

      kdBPart_updateAll(bpt);
      ret = kdBPart_bal(bpt);
    } else {
      PERR("should not happen because of min_i\n");
    }
  } else if (min_i == 2) { /* union(p0_bnd, p1_bnd) lies beside a_bnd */
    float new_ppos = 0;
    int do_swap = 0;
    pdim256_t new_pdim = maxsep_bb(&bpt->pbx, &a_bnd, &new_ppos, &do_swap);

    if (new_pdim < DIMNUM) {	/* partitioning possible */
      kdNode *x = &bpt->this_node;
      kdNode *y = a;
      if (do_swap) { ptr_swap((void**)&x, (void**)&y); }

      ret = (kdNode*)kdBPart_new(bpt->this_node.owner, 0,
                                 new_pdim, new_ppos, x, y);
    } else {
      PERR("should not happen because of min_i\n");
    }
  }
  return ret;
}

kdNode *
kdBPart_ins_Node_standard(kdBPart * bpt, kdNode * a)
{
  kdNode * ret = NULL;
  box2f a_bnd; kdNode_rdBnd_box2f(a, &a_bnd);

  /* kdBPart + kdNode */
  if (box2f_at2(&a_bnd, 1, bpt->pdim) < bpt->ppos) { /* entire a fits in first part */
    bpt->part[0] = kdNode_ins_Node(bpt->part[0], a);
    kdBPart_updateAll(bpt);
  } else if (bpt->ppos <= box2f_at2(&a_bnd, 0, bpt->pdim)) { /* entire a fits in second part */
    bpt->part[1] = kdNode_ins_Node(bpt->part[1], a);
    kdBPart_updateAll(bpt);
  } else {			/* a covers both parts */
    Npv sn0, sn1;		/* subnodes */
    Npv_init(&sn0);
    Npv_init(&sn1);
    /* tear up a in two sets where */
    kdNode_tearUp(a, bpt->pdim, bpt->ppos, &sn0, &sn1);

    /* all the nodes in the first set fit in the first part and */
    for (size_t i = 0; i < sn0.n; i++) {
      bpt->part[0] = kdNode_ins_Node(bpt->part[0], sn0.a[i]);
    }
    Npv_clear(&sn0);

    /* all the nodes in the second set fit in the second part */
    for (size_t i = 0; i < sn1.n; i++) {
      bpt->part[1] = kdNode_ins_Node(bpt->part[1], sn1.a[i]);
    }
    Npv_clear(&sn1);
    kdBPart_updateAll(bpt);
  }

  ret = kdBPart_bal(bpt);

  return ret;
}

kdNode *
kdBPart_ins_Node(kdBPart * bpt, kdNode * a)
{
  kdNode * ret = NULL;
  kdBPart_check_pbx(bpt);

  if (a->ntp == KD_POINT ||	/* kdBPart + kdPoint */
      a->ntp == KD_LIM) {	/* kdBPart + kdLim */
    kdNode * ret = kdBPart_ins_Point(bpt, (kdPoint*)a);
    if (ret) { return ret; }
  }

  box2f a_bnd; kdNode_rdBnd_box2f(a, &a_bnd);

  /* kdBPart + kdCBox/kdBPart */
  if (a->ntp == KD_CBOX ||
      a->ntp == KD_BPART) {
    ret = kdBPart_ins_Node_relate_unions(bpt, a);
    if (ret) { return ret; }
  }

  ret = kdBPart_ins_Node_standard(bpt, a);

  return ret;
}

void
kdBPart_balAVL(kdBPart * bpt)
{
  if (kdNode_getHeight(bpt->part[0]) >
      kdNode_getHeight(bpt->part[1]) && /* unbalanced */
      bpt->part[0]->ntp == KD_BPART) {
    kdBPart * bp = (kdBPart*)bpt->part[0];
    if (bpt->pdim == bp->pdim &&
        kdNode_getHeight(bp->part[0]) >
        kdNode_getHeight(bp->part[1])) {
      /* rotate left branch along */

      kdNode *x = bpt->part[1];
      kdNode *y = bp->part[1];
      kdNode *z = bp->part[0];

      const float ppos0 = bp->ppos;
      const float ppos1 = bpt->ppos;

      /* update branch */
      bp->ppos = ppos1;
      bp->part[0] = y;
      bp->part[1] = x;
      kdBPart_updateAll(bp);

      /* and then current kdNode */
      bpt->ppos = ppos0;
      bpt->part[0] = z;
      bpt->part[1] = (kdNode*)bp;
      kdBPart_updateAll(bpt);

      /* PNOTE("rotated left branch along pdim %d\n", bpt->pdim); */
    }
  } else if (kdNode_getHeight(bpt->part[0]) <
             kdNode_getHeight(bpt->part[1]) && /* unbalanced */
             bpt->part[1]->ntp == KD_BPART) {
    kdBPart * bp = (kdBPart*)bpt->part[1];
    if (bpt->pdim == bp->pdim &&
        kdNode_getHeight(bp->part[0]) <
        kdNode_getHeight(bp->part[1])) {
      /* rotate right branch along */

      kdNode *x = bpt->part[0];
      kdNode *y = bp->part[0];
      kdNode *z = bp->part[1];

      const float ppos0 = bpt->ppos;
      const float ppos1 = bp->ppos;

      /* update branch */
      bp->ppos = ppos0;
      bp->part[0] = x;
      bp->part[1] = y;
      kdBPart_updateAll(bp);

      /* and then current kdNode */
      bpt->ppos = ppos1;
      bpt->part[0] = (kdNode*)bp;
      bpt->part[1] = z;
      kdBPart_updateAll(bpt);

      /* PNOTE("rotated right branch along pdim %d\n", bpt->pdim); */
    }
  } else if (bpt->part[0]->ntp == KD_BPART &&
             bpt->part[1]->ntp == KD_BPART) {
  }
}

/*! Compare Bounding Box Area. */
static inline int
kdNode_cmp_BBoxArea(const void *a, const void *b)
{
  const kdNode *noda = *(kdNode**)a; assert(noda);
  const kdNode *nodb = *(kdNode**)b; assert(nodb);
  box2f bnda, bndb;
  kdNode_rdBnd_box2f(noda, &bnda);
  kdNode_rdBnd_box2f(nodb, &bndb);
  return float_cmp(bnda.area(),
                   bndb.area());
}

/*!
 * Sort the argument \p a in descending order according to the
 * bounding box area of each element.
 *
 * The first element has the greatest, second element has the second greatest
 * bound and so on.
 */
static inline void
Npv_descending_BBArea_sort(Npv * npv)
{
  qsort_mt(npv->a, npv->n, sizeof(kdNode*), kdNode_cmp_BBoxArea, 0,0);
}

void
kdBPart_balPDim(kdBPart * bpt)
{
  pdim65536_t best_i = DIMNUM;		/* indicate nothing found yet */
  float best_bnd = 0;		/* intialize to worst case */

  for (pdim65536_t i = 0; i < DIMNUM; i++) {
    const float bnd = box2f_getDimi(&bpt->pbx, i);
    if (best_i == DIMNUM ||(best_bnd < bnd)) { /* first or better */
      best_i = i;
      best_bnd = bnd;
    }
  }

  /* if we found a "better" pdim along which we should partition */
  if (best_bnd > 2 * box2f_getDimi(&bpt->pbx, bpt->pdim)) {
    /* new parameters */
    pdim256_t new_pdim = best_i;	/* change pdim */
    float new_ppos = bpt->pbx.cen(new_pdim); /* along bounds center */

    Npv sn0, sn1;		/* subnodes */

    Npv_init(&sn0);
    Npv_init(&sn1);
    bpt->part[0] = kdNode_tearUp(bpt->part[0], new_pdim, new_ppos, &sn0, &sn1);
    bpt->part[1] = kdNode_tearUp(bpt->part[1], new_pdim, new_ppos, &sn0, &sn1);
    if (bpt->part[0]) { PWARN("part[0] non-zero\n"); }
    if (bpt->part[1]) { PWARN("part[1] non-zero\n"); }

    Npv_descending_BBArea_sort(&sn0);
    Npv_descending_BBArea_sort(&sn1);

    if (! (sn0.n > 0 && sn1.n > 0)) { PERR("tearUp gave one empty Npv\n"); }

    /* alter kdBPart with first item in each Npv */
    kdBPart_repartAt(bpt, new_pdim, new_ppos);
    if (sn0.n > 0) { bpt->part[0] = sn0.a[0]; }	/* insert first in first part */
    if (sn1.n > 0) { bpt->part[1] = sn1.a[0]; }	/* insert first in second part */
    kdBPart_updateAll(bpt);

    /* insert the rest in first part */
    for (size_t i = 1; i < sn0.n; i++) {
      bpt->part[0] = kdNode_ins_Node(bpt->part[0], sn0.a[i]);
    }
    Npv_clear(&sn0);		/* we are done with sn0 */

    /* insert the rest in second part */
    for (size_t i = 1; i < sn1.n; i++) {
      bpt->part[1] = kdNode_ins_Node(bpt->part[1], sn1.a[i]);
    }
    Npv_clear(&sn1);		/* we are done with sn1 */

    kdBPart_updateAll(bpt);
  }
}

kdNode *
kdBPart_compress(kdBPart * bpt)
{
  kdNode * ret = &bpt->this_node;
  if (bpt->bph == BPH_LIM_PAIR) {
    if (bpt->part[0]->ntp == KD_LIM &&
        bpt->part[1]->ntp == KD_LIM) {
      kdLim *shl0 = (kdLim*)bpt->part[0];
      kdLim *shl1 = (kdLim*)bpt->part[1];
      if (shl0->csh == shl1->csh) {
        kdCBox *shb = kdCBox_new(bpt->this_node.owner, 0, shl0->csh);
        kdBPart_rdel(bpt, 0);	/* delete BPart and its Lims */
        ret = (kdNode*)shb;
      }
    } else {
      PWARN("BPH_LIM_PAIR parts are not both of type KD_LIM!\n");
    }
  }
  return ret;
}

kdNode *
kdBPart_bal(kdBPart * bpt)
{
  /* kdBPart_balPDim(bpt); */
  kdBPart_balAVL(bpt);
  return kdBPart_compress(bpt);
}

kdNode *
kdBPart_rbal(kdBPart * bpt)
{
  bpt->part[0] = kdNode_rbal(bpt->part[0]);
  bpt->part[1] = kdNode_rbal(bpt->part[1]);
  kdBPart_updateAll(bpt);
  return kdBPart_bal(bpt);
}

kdNode *
kdBPart_extractAt(kdBPart * bpt, const box2f * a)
{
  if (box2f_inside_box2f(a, &bpt->pbx)) {
    if (vec2f_at(&a->u, bpt->pdim) < bpt->ppos) {
      return bpt->part[0];
    } else if (bpt->ppos <= vec2f_at(&a->l, bpt->pdim)) {
      return bpt->part[1];
    } else {
      return &bpt->this_node;
    }
  } else {
    return &bpt->this_node;
  }
}

const kdNode *
kdBPart_find_vec(const kdBPart * bpt, const vec2f * a)
{
  if (vec2f_at(a, bpt->pdim) < bpt->ppos) {
    return kdNode_find_vec(bpt->part[0], a);
  } else {
    return kdNode_find_vec(bpt->part[1], a);
  }
}

int
kdBPart_find_shape(const kdBPart * bpt, const vis_t * a,
                   LIMASK_t * limask, Npv * hits)
{
  int ret = 0;
  box2f a_bnd;
  vis_rdBnd_box2f(a, &a_bnd);

  if (*limask == BOTH_LIMS) {
    if (box2f_at2(&a_bnd, 1, bpt->pdim) < bpt->ppos) {
      /* whole shape lie on the "left" */
      ret += kdNode_find_shape(bpt->part[0], a, limask, hits);
    } else if (bpt->ppos <= box2f_at2(&a_bnd, 0, bpt->pdim)) {
      /* whole shape lie on the "right" */
      ret += kdNode_find_shape(bpt->part[1], a, limask, hits);
    } else {
      /* shlims are separated by this kdBPart */

      /* update search statistics */
      height_t h = kdNode_getHeight(&bpt->this_node);
      kdTree * tree = bpt->this_node.owner;
      size_t old_len = tree->fbs_len;
      tree->fbs_len = MAX2(old_len, (size_t)h + 1);
      tree->fbs = realloc(tree->fbs, tree->fbs_len * sizeof(size_t));
      /* zero new counters */
      for (size_t i = old_len; i < tree->fbs_len; i++) { tree->fbs[i] = 0; }
      tree->fbs[h]++;		/* increase current counter */

      if (box2f_at2(&a_bnd, 0, bpt->pdim) < bpt->ppos) {
        /* two finds */
        ret += kdNode_find_shape(bpt->part[0], a, limask, hits);
        ret += kdNode_find_shape(bpt->part[1], a, limask, hits);
      } else {
        PERR("tree structure: a(LO_LIM) not < ppos\n");
      }
    }
  } else if (*limask) {
    size_t cidx = (*limask == LO_LIM) ? 0 : 1;
    if (box2f_at2(&a_bnd, cidx, bpt->pdim) < bpt->ppos) {
      ret += kdNode_find_shape(bpt->part[0], a, limask, hits);
    } else {
      ret += kdNode_find_shape(bpt->part[1], a, limask, hits);
    }
  }
  return ret;
}

kdNode *
kdBPart_rm_vec(kdBPart * bpt, vec2f * a, size_t *rmcnt_ret)
{
  uint dirty = 0;

  if (vec2f_at(a, bpt->pdim) < bpt->ppos) {
    if ((bpt->part[0] = kdNode_rm_vec(bpt->part[0], a, rmcnt_ret)) == 0) {
      /* left branch was removed, so remove kdBPart and return other branch */
      kdNode *other = bpt->part[1];	/* remember other branch */
      kdBPart_delete(bpt);		/* delete partition */
      return other;	       /* return other branch */
    } else {
      dirty = 1;	       /* something might have changed */
    }
  } else {
    if ((bpt->part[1] = kdNode_rm_vec(bpt->part[1], a, rmcnt_ret)) == 0) {
      /* left branch was removed, so remove kdBPart and return other branch */
      kdNode *other = bpt->part[0];	/* remember other branch */
      kdBPart_delete(bpt);		/* delete partition */
      return other;	       /* return other branch */
    } else {
      dirty = 1;	       /* something might have changed */
    }
  }

  if (dirty) {
    kdBPart_updateAll(bpt);
    return kdBPart_bal(bpt);
  } else {
    return &bpt->this_node;
  }
}

kdNode *
kdBPart_rm_shape(kdBPart * bpt, vis_t * a, LIMASK_t * limask, size_t *rmcnt_ret)
{
  uint dirty = 0;
  box2f a_bnd;
  vis_rdBnd_box2f(a, &a_bnd);

  if (*limask == BOTH_LIMS) {
    if (box2f_at2(&a_bnd, 1, bpt->pdim) < bpt->ppos) { /* both in part[0] */
      /* remove both limits from part[0] */
      if ((bpt->part[0] = kdNode_rm_shape(bpt->part[0],
                                          a, limask, rmcnt_ret)) == 0) {
        /* left branch was removed, so remove kdBPart and return other */
        kdNode *other = bpt->part[1];	/* remember other branch */
        kdBPart_delete(bpt);		/* delete partition */
        return other;	       /* return other branch */
      } else {
        dirty = 1;	       /* something might have changed */
      }
    } else if (bpt->ppos <= box2f_at2(&a_bnd, 0, bpt->pdim)) { /* both in part[1] */
      /* remove both limits from part[1] */
      if ((bpt->part[1] = kdNode_rm_shape(bpt->part[1],
                                          a, limask, rmcnt_ret)) == 0) {
        /* left branch was removed, so remove kdBPart and return other */
        kdNode *other = bpt->part[0];	/* remember other branch */
        kdBPart_delete(bpt);		/* delete partition */
        return other;	       /* return other branch */
      } else {
        dirty = 1;	       /* something might have changed */
      }
    } else {		       /* remove each limit separately */
      /* recurse */
      LIMASK_t limask_lo = LO_LIM, limask_up = UP_LIM;
      kdNode * ret = (kdNode*)bpt;
      ret = kdNode_rm_shape(ret, a, &limask_lo, rmcnt_ret);
      ret = kdNode_rm_shape(ret, a, &limask_up, rmcnt_ret);
      *limask = limask_lo | limask_up;
      return ret;
    }
  } else {
    uint cidx = (*limask == LO_LIM) ? 0 : 1;
    if (box2f_at2(&a_bnd, cidx, bpt->pdim) < bpt->ppos) {
      if ((bpt->part[0] = kdNode_rm_shape(bpt->part[0],
                                          a, limask, rmcnt_ret)) == 0) {
        /* left branch was removed, so remove kdBPart and return other */
        kdNode *other = bpt->part[1];	/* remember other branch */
        kdBPart_delete(bpt);		/* delete partition */
        return other;	       /* return other branch */
      } else {
        dirty = 1;	       /* something might have changed */
      }
    } else {
      if ((bpt->part[1] = kdNode_rm_shape(bpt->part[1],
                                          a, limask, rmcnt_ret)) == 0) {
        /* left branch was removed, so remove kdBPart and return other */
        kdNode *other = bpt->part[0];	/* remember other branch */
        kdBPart_delete(bpt);		/* delete partition */
        return other;		/* return other branch */
      } else {
        dirty = 1;		/* something might have changed */
      }
    }
  }

  if (dirty) {
    kdBPart_updateAll(bpt);
    return kdBPart_bal(bpt);
  } else {
    return &bpt->this_node;
  }
}

void
kdBPart_tearUp(kdBPart * bpt, pdim256_t pdim, float ppos,
               Npv * sn0_ret, Npv * sn1_ret)
{
  if (box2f_at2(&bpt->pbx, 1, pdim) < ppos) { /* whole kdBPart to the "left" */
    Npv_app(sn0_ret, &bpt->this_node); /* no tearing up needed - add whole kdBPart */
  } else if (ppos <= box2f_at2(&bpt->pbx, 0, pdim)) { /* whole kdBPart to the "right" */
    Npv_app(sn1_ret, &bpt->this_node); /* no tearing up needed - add whole kdBPart */
  } else {  /* kdBPart overlaps partition "line" at ppos along pdim */
    /* tearing up of this kdBPart is needed */
    box2f p0_bnd, p1_bnd;

    kdNode_rdBnd_box2f(bpt->part[0], &p0_bnd);
    if (box2f_at2(&p0_bnd, 1, pdim) < ppos) {
      Npv_app(sn0_ret, bpt->part[0]);
    } else {
      kdNode_tearUp(bpt->part[0], pdim, ppos, sn0_ret, sn1_ret); /* recurse */
    }

    kdNode_rdBnd_box2f(bpt->part[1], &p1_bnd);
    if (ppos <= box2f_at2(&p1_bnd, 0, pdim)) {
      Npv_app(sn1_ret, bpt->part[1]);
    } else {
      kdNode_tearUp(bpt->part[1], pdim, ppos, sn0_ret, sn1_ret); /* recurse */
    }

    kdBPart_delete(bpt);		/* delete partition */
  }
}

void
kdBPart_repartAt(kdBPart * bpt, pdim256_t new_pdim, float new_ppos)
{
  if (bpt->pdim != new_pdim) {
    bpt->this_node.owner->bpt_cnts[bpt->pdim]--;
    bpt->this_node.owner->bpt_cnts[new_pdim]++;
  }
  bpt->pdim = new_pdim;
  bpt->ppos = new_ppos;
}

void
kdBPart_updateBBox(kdBPart * bpt)
{
  /* warn about sub-bparts that has obseleted pbx:es */
  kdNode_check_pbx(bpt->part[0]);
  kdNode_check_pbx(bpt->part[1]);

  box2f p0_bnd, p1_bnd;
  kdNode_rdBnd_box2f(bpt->part[0], &p0_bnd);
  kdNode_rdBnd_box2f(bpt->part[1], &p1_bnd);
  box2f_unite(&p0_bnd, &p1_bnd, &bpt->pbx);
}

void
kdBPart_updateHeight(kdBPart * bpt)
{
  kdNode_setHeight(&bpt->this_node,
                   MAX2(kdNode_getHeight(bpt->part[0]),
                        kdNode_getHeight(bpt->part[1])) + 1);
}

void
kdBPart_updateBPH(kdBPart * bpt)
{
  BPH_t old_bph = bpt->bph;	/* save old hint */
  kdBPart_calcBPH(bpt);		/* set new hint */
  /* update statistics */
  if (old_bph != BPH_LIM_PAIR && bpt->bph == BPH_LIM_PAIR) {
    bpt->this_node.owner->lopair_cnt++;
  } else if (old_bph == BPH_LIM_PAIR && bpt->bph != BPH_LIM_PAIR) {
    bpt->this_node.owner->lopair_cnt--;
  }
}

void
kdBPart_updateAll(kdBPart * bpt)
{
  kdBPart_updateBBox(bpt);
  kdBPart_updateHeight(bpt);
  kdBPart_updateBPH(bpt);
}

void
kdBPart_fprint(FILE * stream, kdBPart * bpt, uint level)
{
  mputspace(stream, 2 * level);
  fprintf(stream, "BPART height=%d pbx=", kdNode_getHeight(&bpt->this_node));
  box2f_fprint(stream, &bpt->pbx);

  fprintf(stream, " ppos=%f", bpt->ppos);
  fprintf(stream, " pdim=%d", (int)bpt->pdim);
  fprintf(stream, " bph=%s", BPH_getName(bpt->bph));

  fendline(stream);

  kdNode_fprint(stream, bpt->part[0], level + 1);
  kdNode_fprint(stream, bpt->part[1], level + 1);
}

/* ---------------------------- Group Separator ---------------------------- */

static void
kdTree_init(kdTree * tree)
{
  tree->root = 0;
  tree->point_cnt = 0;
  tree->ref_cnt = 0;
  tree->lim_cnt = 0;
  tree->cbox_cnt = 0;
  tree->dup_cnt = 0;

  tree->lopair_cnt = 0;

  for (size_t i = 0; i != DIMNUM; i++) {
    tree->bpt_cnts[i] = 0;
  }

  tree->fbs = 0;
  tree->fbs_len = 0;
}

kdTree *
kdTree_new(void)
{
  kdTree * tree = malloc(sizeof(kdTree));
  kdTree_init(tree);
  return tree;
}

void
kdTree_clear(kdTree * tree, bool leaf_flag)
{
  if (tree->root) {
    kdNode_rdel(tree->root, leaf_flag);
    tree->root = 0;
  }
  if (tree->fbs) { free(tree->fbs); }
}

void
kdTree_delete(kdTree * tree, bool leaf_flag)
{
  kdTree_clear(tree, leaf_flag);
  free(tree);
}

size_t
kdTree_getSize(const kdTree * tree)
{
  if (tree->root) {
    return sizeof(kdTree) + kdNode_getSize(tree->root);
  } else {
    return sizeof(kdTree);
  }
}

void
kdTree_ins_vec(kdTree * tree, vec2f * a)
{
  kdPoint *po = (kdPoint*)kdRef_new(tree, 0, a);
  if (tree->root) {
    tree->root = kdNode_ins_Node(tree->root, (kdNode*)po);
  } else {
    tree->root = (kdNode*)po;
  }
}

void
kdTree_ins_helper1(kdTree * tree, vis_t * a, LIMASK_t limask)
{
  if (tree->root) {
    tree->root = kdNode_ins_shape(tree->root, a, limask);
  } else {
    tree->root = (kdNode*)kdLim_new(tree, 0, a, limask);
  }
}

void
kdTree_twostep_ins(kdTree * tree, vis_t * a)
{
  kdTree_ins_helper1(tree, a, LO_LIM);
  kdTree_ins_helper1(tree, a, UP_LIM);
}

void
kdTree_onestep_ins(kdTree * tree, vis_t * a)
{
  if (tree->root) {
    tree->root = kdNode_ins_shape(tree->root, a, BOTH_LIMS);
  } else {
    tree->root = kd_new_LIM_PAIR(tree, 0, a);
  }
}

void
kdTree_lim_pair_ins(kdTree * tree, vis_t * a)
{
  kdNode * a_node = (kdNode*)kd_new_LIM_PAIR(tree, 0, a);
  if (tree->root) {
    tree->root = kdNode_ins_Node(tree->root, a_node);
  } else {
    tree->root = a_node;
  }
}

void
kdTree_cbox_ins(kdTree * tree, vis_t * a)
{
  kdNode * a_node = (kdNode*)kdCBox_new(tree, 0, a);
  if (tree->root) { tree->root = kdNode_ins_Node(tree->root, a_node); }
  else { tree->root = a_node; }
}

void
kdTree_ins_shape(kdTree * tree, vis_t * a)
{
  kdTree_cbox_ins(tree, a);
}

int
kdTree_rm_vec(kdTree * tree, vec2f * a)
{
  if (!tree->root) { PWARN("Tree is empty!\n"); return 0; }
  size_t rmcnt = 0;
  tree->root = kdNode_rm_vec(tree->root, a, &rmcnt);
  if (rmcnt == 1) {
    return 1;
  } else if (rmcnt > 1) {
    PERR("tree structure: " "same vector removed multiple times!\n");
    return 0;
  } else {
    PWARN("vector not found!\n");
    return 0;
  }
}

int
kdTree_rm_shape(kdTree * tree, vis_t * a)
{
  if (!tree->root) { PWARN("Tree is empty!\n"); return 0; }
  size_t rmcnt = 0;
  LIMASK_t limask = BOTH_LIMS;
  tree->root = kdNode_rm_shape(tree->root, a, &limask, &rmcnt);
  if (limask == 0) {
    return 1;
  } else {
    lprintf("tree structure: limask=%d after search for %p ",
            limask, a);
    vis_fprint(stdout, a); endline();
    return 0;
  }
}

int kdTree_reins_shape(kdTree * tree, vis_t * a)
{
  int ret = 0;
  ret += kdTree_rm_shape(tree, a);
  kdTree_ins_shape(tree, a);
  return ret;
}

const kdNode *
kdTree_find_vec(const kdTree * tree, const vec2f * a)
{
  if (tree->root) {
    const kdNode *hit = kdNode_find_vec(tree->root, a);
    if (! hit) {
      PNOTE("tree structure: found no vec!\n");
    }
    return hit;
  } else {
    return 0;
  }
}

int
kdTree_find_shape(const kdTree * tree, const vis_t * a)
{
  int ret = 0;
  if (tree->root) {
    LIMASK_t limask = BOTH_LIMS;
    Npv hits;
    Npv_init(&hits);
    ret = kdNode_find_shape(tree->root, a, &limask, &hits);
    if (limask == 0) {
      /*       printf("HIT:\n"); */
      /*       kdNode_fprint(stdout, hit, 1); */
    } else {
      lprintf("tree structure: hits.n=%zd ret=%d limask=%d after search for %p ",
              hits.n, ret, limask, a);
      vis_fprint(stdout, a); endline();
    }
    Npv_clear(&hits);
  }
  return ret;
}

height_t
kdTree_getHeight(const kdTree * tree)
{
  if (tree->root) {
    return kdNode_getHeight(tree->root);
  } else {
    return 0;
  }
}

void kdTree_rbal(kdTree * tree)
{
  if (tree->root) {
    tree->root = kdNode_rbal(tree->root);
  }
}

void
kdTree_fprint(FILE * stream, const kdTree * tree, uint show)
{
  if (tree->root) {
    size_t total_cnt = (tree->point_cnt + tree->cbox_cnt + tree->dup_cnt);

    for (pdim65536_t i = 0; i < DIMNUM; i++) {
      total_cnt += tree->bpt_cnts[i];
    }

    fprintf(stream,
            "TREE"
            " bytes=%zd"
            " height=%d"
            " POINT=%d"
            " REF=%d"
            " LIM=%d"
            " CBOX=%d"
            " DUP=%d"
            " BPART=[%d,%d]"
            " lopair_cnt=%d"
            " total_cnt=%zd",
            kdNode_getSize(tree->root),
            kdNode_getHeight(tree->root),
            tree->point_cnt,
            tree->ref_cnt,
            tree->lim_cnt,
            tree->cbox_cnt,
            tree->dup_cnt,
            tree->bpt_cnts[0], tree->bpt_cnts[1],
            tree->lopair_cnt,
            total_cnt);

    fprintf(stream, " fbs=[");
    for (size_t i = 0; i < tree->fbs_len; i++) {
      if (i > 0) { fprintf(stream, ","); }
      fprintf(stream, "%zd", tree->fbs[i]);
    }
    fprintf(stream, "]\n");

    if (show) {
      kdNode_fprint(stream, tree->root, 1);
    }
  } else {
    fprintf(stream, "TREE: empty\n");
  }
}

void
kdTree_reset_fbs(kdTree * tree)
{
  for (size_t i = 0; i < tree->fbs_len; i++) { tree->fbs[i] = 0; }
}
