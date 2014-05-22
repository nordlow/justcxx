#pragma once


/*!
 * Tree storage using Binary Space Partitioning along object extent centers
 */
class BSPSet : public Obj
{
public:
    virtual const char * get_typename() const { return "BSPSet"; }

//     class TNode
//     {
//     public:
//         typedef enum { BPART, LEAF } Type;
//         Type get_type() const = 0;
//     };

//     // binary space partition
//     class BPart : public TNode
//     {
//     public:
//         Type get_type() const { return BPART; }
//     private:
//     };

//     class Leaf : public TNode
//     {
//     public:
//         Type get_type() const { return LEAF; }
//     private:
//     };

    class Node
    {
    public:
        typedef enum { BRANCH, LEAF } Type;

        Node(Node *parent, uint pdim, float pos, Node *b0, Node *b1, Node *bx)
            : parent(parent), type(BRANCH), pdim(pdim), pos(pos),
              b0(b0), b1(b1), bx(bx) {}

        Node(Node *parent, const ObjPs& leaves)
            : parent(parent), type(LEAF),
              leaves(leaves) {}
    public:
        Node *parent;
        Type type;

        // BRANCH data
        uint pdim:1;
        float pos;                // position
        Node *b0, *b1, *bx;

        // LEAF data
        ObjPs leaves;
    };

    BSPSet() : root(0) {}

    BSPSet(const ObjPs& objps) : root(0)
    {
	alls = objps;
	rebuild_all();
    }

    virtual ~BSPSet() { clear_tree(); }

    virtual void delete_all()
    {
	for (auto i = begin(alls); i != end(alls); i++)
	{
	    delete (*i);
	}
	clear_tree();
    }

    void add(Obj * obj) { alls.push_back(obj); }
    uint get_size() const { return alls.size(); }

    virtual void rebuild_all();

    // select the ObjPs that are totally or partially visible in View
    virtual void clip_to(const Rect& rect);
    virtual ObjPs& get_all() { return alls; }
    virtual ObjPs& get_clipped() { return clips; }

    // Obj functions

    virtual Obj * get_nearest_to(const Pos& pos);
    virtual void render(View& view, const RendReq& rendreq);
private:
    // \todo MAKe this a stack of QLeaf's instead
    typedef std::stack<const Node*> QStack;

    ObjPs alls;                 ///< all objps
    Node *root;                 ///< root of quad tree
    ObjPs clips;                ///< selected objps

private:
    void clear_tree();
};

void BSPSet::rebuild_all()
{
    if (root) { clear_tree(); }

    if (alls.empty()) { return; }

    root = new Node(0, alls); // root has no parent

    QStack qstack;
    qstack.push(root);

    while (qstack.size() > 0)
    {
        const Node *top = qstack.top(); // top node
        const Node::Type type = top->type;
        qstack.pop();

        if (type != Node::LEAF)
        {
            ZERROR("this should not happen!");
            exit(-1);
        }

        const ObjPs& objps = top->leaves;
        uint n = objps.size();

        // iterators and counters

        ObjPs::const_iterator i;
        uint c;

        // extents and centers

        Rect exts[n];
        Pos cens[n];
        for (i = begin(objps), c = 0;
             i != end(objps);
             i++, c++)
        {
            exts[c] = (*i)->bounds();
            cens[c] = exts[c].center();
        }

        // area center mean

        double sum0 = 0, sum1 = 0;
        for (c = 0; c < n; c++)
        {
            sum0 += cens[c](0);
            sum1 += cens[c](1);
        }
        Pos cpos = Pos(static_cast<float>(sum0 / n),
                       static_cast<float>(sum1 / n));

        // area center variance

        double var0 = 0, var1 = 0;
        for (c = 0; c < n; c++)
        {
            var0 += pnw::square(static_cast<double>(cens[c](0) - cpos(0)));
            var1 += pnw::square(static_cast<double>(cens[c](1) - cpos(1)));
        }

        // partition

        uint pdim = var0 > var1 ? 0 : 1; // partitioning dimension
        ObjPs ov0, ov1, ovx;
        for (i = begin(objps), c = 0;
             i != end(objps);
             i++, c++)
        {
            const Pos& pos = exts[c].l();
            const Pos& dim = exts[c].dim();
            if      (pos(pdim) + dim(pdim) < cpos(pdim)) { ov0.push_back(*i); }
            else if (pos(pdim)             > cpos(pdim)) { ov1.push_back(*i); }
            else                                         { ovx.push_back(*i); }
        }

        if (n == ovx.size())
        {
            // no partitioning possible - leave top untouched
        }
        else
        {
            Node *par = top->parent;
            bool at_root = (par == 0);

            Node **link;

            if (at_root)
            {
                link = &root;
            }
            else
            {
                if (par->type != Node::BRANCH)
                {
                    // \todo REMove later on.
                    ZERROR("parent is not a BRANCH");
                    exit(-1);
                }
                // \todo AVOid this error prone code somehow
                if      (top == par->b0) { link = &par->b0; }
                else if (top == par->b1) { link = &par->b1; }
                else if (top == par->bx) { link = &par->bx; }
                else
                {
                    // \todo REMove later on.
                    ZERROR("no match in par");
                    exit(-1);
                }
            }

            // reallocate link to correct type

            delete *link;
            Node *parpar;
            if (at_root) { parpar = 0; } else { parpar = par->parent; }
            *link = new Node(parpar, pdim, cpos(pdim), 0, 0, 0);

            // set children

            if (ov0.size() > 0)
            {
                Node *b0 = new Node(*link, ov0);
                b0->parent = *link;
                (*link)->b0 = b0;
                qstack.push(b0);
            }

            if (ov1.size() > 0)
            {
                Node *b1 = new Node(*link, ov1);
                b1->parent = *link;
                (*link)->b1 = b1;
                qstack.push(b1);
            }

            if (ovx.size() > 0)
            {
                Node *bx = new Node(*link, ovx);
                bx->parent = *link;
                (*link)->bx = bx;
                qstack.push(bx);
            }
        }
    }
}

void BSPSet::clip_to(const Rect& rect)
{
    clips.clear();
    if (not root) { return; }

    QStack qstack; qstack.push(root);

    while (qstack.size() > 0)
    {
        const Node *top = qstack.top(); // top node
        const Node::Type& type = top->type;
        qstack.pop();

        if (type == Node::LEAF)
        {
            const ObjPs& objps = top->leaves;
            for (auto i = begin(objps); i != end(objps); i++)
            {
                if (not (*i)->hidden_from(rect))
                {
                    clips.push_back(*i);
                }
            }
        }
        else if (type == Node::BRANCH)
        {
            const uint pdim = top->pdim;
            const float qpos = top->pos;
            const Node *b0 = top->b0;
            const Node *b1 = top->b1;
            const Node *bx = top->bx;

            const Pos& vpos = rect.l();
            const Pos& vdim = rect.dim();

            if      (vpos(pdim) + vdim(pdim) < qpos)
            {
                if (b0) { qstack.push(b0); }
            }
            else if (vpos(pdim)             > qpos)
            {
                if (b1) { qstack.push(b1); }
            }
            else
            {
                if (b0) { qstack.push(b0); }
                if (b1) { qstack.push(b1); }
            }
            if (bx) { qstack.push(bx); }
        }
        else
        {
            // \todo REMove later on.
            cout << GPFUN << "unknown Node Type" << endl;
            exit(-1);
        }
    }
}

void BSPSet::clear_tree()
{
    if (not root) { return; }

    QStack qstack;
    qstack.push(root);

    while (qstack.size() > 0)
    {
        const Node *top = qstack.top(); // top node
        const Node::Type& type = top->type;
        qstack.pop();

        if (type == Node::LEAF)
        {
            // no dynamic objps to delete
        }
        else if (type == Node::BRANCH)
        {
            if (top->b0) { qstack.push(top->b0); }
            if (top->b1) { qstack.push(top->b1); }
            if (top->bx) { qstack.push(top->bx); }
        }
        else
        {
            // \todo REMove later on.
            ZERROR("unknown Node Type");
            exit(-1);
        }

        delete top;
    }

    root = 0;
}

/*!
 * \todo Optimize by doing a tree search
 */
Obj * BSPSet::get_nearest_to(const Pos& pos)
{
    const ObjPs& all = get_all();

    if (all.empty()) { return 0; }

    float mindist = std::numeric_limits<float>::max();
    Obj * nearest = NULL;

    for (auto i = begin(all); i != end(all); i++)
    {
        float dist = (*i)->get_distance_to(pos);
        if (dist < mindist)
        {
            mindist = dist;
            nearest = *i;
        }
    }

    if (nearest)
    {
        // we had a hit so return it
        return nearest->get_nearest_to(pos);
    }
    else
    {
        // otherwise return zero
        return 0;
    }
}

/*!
 * \todo Clip_to has a performance problem for large data -
 * \todo Optimize by doing a tree traversal
 */
void BSPSet::render(View& view, const RendReq& rendreq)
{
    clip_to(view.rect);

    const ObjPs& sel = get_clipped();

    for (auto i = begin(sel); i != end(sel); i++)
    {
        view.req_rend(*i, rendreq.order);
    }
}

// ============================================================================
