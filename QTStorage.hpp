#pragma once

// tree storage

class QTStorage
{
public:

    class Node
    {
    public:

        typedef enum
        {
            QBRANCH,            // quad-tree branch
            BBRANCH0,           // binary-tree branch along dimension 0
            BBRANCH1,           // binary-tree branch along dimension 1
            LEAF                // leaf (no branch)
        }
        Type;

        Node(Node *parent,
             Vec qpos,
             Node *q00, Node *q01,
             Node *q10, Node *q11,
             Node *qxx)
            :

            parent(parent),

            type(QBRANCH),
            qpos(qpos),
            q00(q00), q01(q01), q10(q10), q11(q11),
            qxx(qxx)
            {}

        Node(Node *parent,
             Type type,
             float bpos,
             Node *b0,
             Node *b1,
             Node *bx)
            :

            parent(parent),

            type(type),
            bpos(bpos),
            b0(b0), b1(b1),
            bx(bx)
            {}

        Node(Node *parent,
             ObjV leaves)
            :

            parent(parent),

            type(LEAF),

            leaves(leaves)
            {}

    public:
        Node *parent;

        Type type;

        // QBRANCH data
        Vec qpos;                // position
        Node *q00, *q01;
        Node *q10, *q11;
        Node *qxx;

        // BBRANCH0,1 data
        float bpos;                // position
        Node *b0, *b1;
        Node *bx;

        // LEAF data
        ObjV leaves;

    };

    // \todo SOLve this with abstract methods in these classes instead
//     class QBranch : public Node
//     {
//     public:
//     private:
//     };
//     class QLeaf : public Node
//     {
//     public:
//     private:
//     };

    // \todo MAKe this a stack of QLeaf's instead
    typedef std::stack<const Node*> QStack;

    QTStorage() : root(0) {}
    QTStorage(const ObjV& objv) : root(0)
        {
            allcont = objv;
            rebuild();
        }

    virtual ~QTStorage() { clear(); }
    virtual void clear();

    void add(Obj *obj) { allcont.push_back(obj); }
    uint size() const { return allcont.size(); }

    virtual void rebuild();

    // select the objv that is totally or partially visible in View
    virtual void select_in(const View& view);

    virtual ObjV& get_all() { return allcont; }

    virtual ObjV& get_selection() { return selcont; }

private:
    ObjV allcont;            // all objv
    Node *root;                 // root of quad tree
    ObjV selcont;            // selected objv
};

void QTStorage::rebuild()
{
    if (root) { clear(); }

    if (allcont.empty()) { return; }

    root = new Node(0, allcont); // root has no parent

    QStack qstack;
    qstack.push(root);

    while (qstack.size() > 0)
    {
        const Node *topn = qstack.top(); // top node
        const Node::Type type = topn->type;
        qstack.pop();

        if (type != Node::LEAF)
        {
            cout << "this should not happen!" << endl;
            exit(-1);
        }

        const ObjV& objv = topn->leaves;

        // center of "mass"
        double sum0 = 0;
        double sum1 = 0;

        for (ObjV::const_iterator i=begin(objv); i != end(objv); i++)
        {
            auto ext = (*i)->bounds(); // \todo BUFfer these
            Vec cenf = ext.center();

            sum0 += cenf(0);
            sum1 += cenf(1);
        }

        // branching position
        uint n = objv.size();
        Vec qpos = Vec(static_cast<float>(sum0 / n),
                       static_cast<float>(sum1 / n));

        // classify into nine different categories

        ObjV q00cont, q01cont, q10cont, q11cont;
        ObjV b00cont, b01cont, b10cont, b11cont;
        ObjV qxxcont;

        for (auto i=begin(objv); i != end(objv); i++)
        {
            auto ext = (*i)->bounds(); // \todo REUse buffered above
            const Vec& pos = ext.pos;
            const Vec& dim = ext.dim;

            if      (pos(0) + dim(0) < qpos(0))
            {
                if      (pos(1) + dim(1) < qpos(1))
                {
                    q00cont.push_back(*i);
                }
                else if (pos(1)          > qpos(1))
                {
                    q01cont.push_back(*i);
                }
                else
                {
                    b00cont.push_back(*i);
                }
            }
            else if (pos(0)          > qpos(0))
            {
                if      (pos(1) + dim(1) < qpos(1))
                {
                    q10cont.push_back(*i);
                }
                else if (pos(1)          > qpos(1))
                {
                    q11cont.push_back(*i);
                }
                else
                {
                    b10cont.push_back(*i);
                }
            }
            else
            {
                if (pos(1) + dim(1) < qpos(1))
                {
                    b01cont.push_back(*i);
                }
                else if (pos(1)          > qpos(1))
                {
                    b11cont.push_back(*i);
                }
                else
                {
                    qxxcont.push_back(*i);
                }
            }

        }

        uint qcnt =
            (q00cont.size() > 0) +
            (q01cont.size() > 0) +
            (q10cont.size() > 0) +
            (q11cont.size() > 0);

        uint bcnt =
            (b00cont.size() > 0) +
            (b01cont.size() > 0) +
            (b10cont.size() > 0) +
            (b11cont.size() > 0);

        if (qcnt == 2)
        {
            // use quad-tree

            // append binary-tree content to border content
            qxxcont.insert(end(qxxcont), begin(b00cont), end(b00cont));
            qxxcont.insert(end(qxxcont), begin(b01cont), end(b01cont));
            qxxcont.insert(end(qxxcont), begin(b10cont), end(b10cont));
            qxxcont.insert(end(qxxcont), begin(b11cont), end(b11cont));

            // clear binary-tree content
            b00cont.clear();
            b01cont.clear();
            b10cont.clear();
            b11cont.clear();

            Node *par = topn->parent;
            bool at_root = (par == 0);

            Node **link;

            if (at_root)
            {
                link = &root;
            }
            else
            {
                if (par->type != Node::QBRANCH)
                {
                    cerr << GFUN << "parent is not a QBRANCH" << endl;
                    exit(-1);
                }
                if      (topn == par->q00) { link = &par->q00; }
                else if (topn == par->q01) { link = &par->q01; }
                else if (topn == par->q10) { link = &par->q10; }
                else if (topn == par->q11) { link = &par->q11; }
                else if (topn == par->qxx) { link = &par->qxx; }
                else
                {
                    cerr << GFUN << "no match in par" << endl;
                    exit(-1);
                }
            }

            // reallocate link to correct type

            delete *link;
            Node *parpar;
            if (at_root) { parpar = 0; } else { parpar = par->parent; }
            *link = new Node(parpar,
                             qpos,
                             0, 0,
                             0, 0,
                             0);

            // set children

            Node *q00, *q01, *q10, *q11, *qxx;

            if (q00cont.size() > 0)
            {
                q00 = new Node(par, q00cont);
                q00->parent = *link; (*link)->q00 = q00; qstack.push(q00);
            }
            else { q00 = 0; }
            if (q01cont.size() > 0)
            {
                q01 = new Node(par, q01cont);
                q01->parent = *link; (*link)->q01 = q01; qstack.push(q01);
            }
            else { q01 = 0; }
            if (q10cont.size() > 0)
            {
                q10 = new Node(par, q10cont);
                q10->parent = *link; (*link)->q10 = q10; qstack.push(q10);
            }
            else { q10 = 0; }
            if (q11cont.size() > 0)
            {
                q11 = new Node(par, q11cont);
                q11->parent = *link; (*link)->q11 = q11; qstack.push(q11);
            }
            else { q11 = 0; }
            if (qxxcont.size() > 0)
            {
                qxx = new Node(par, qxxcont);
                qxx->parent = *link; (*link)->qxx = qxx; qstack.push(qxx);
            }
            else { qxx = 0; }

        }
        else if (bcnt == 2)
        {
            // use binary-tree(s)

            // append quad-tree content to binary-tree content
            b01cont.insert(end(b01cont), begin(q00cont), end(q00cont));
            b01cont.insert(end(b01cont), begin(q01cont), end(q01cont));
            b11cont.insert(end(b11cont), begin(q10cont), end(q10cont));
            b11cont.insert(end(b11cont), begin(q11cont), end(q11cont));

            // clear quad-tree content
            q00cont.clear();
            q01cont.clear();
            q10cont.clear();
            q11cont.clear();

        }
        else if (objv.size() == qxxcont.size())
        {
            // all objv are obstructed by the borders - stop partitioning
        }
        else
        {

        }

    }

    cout << endl;
}

void QTStorage::select_in(const View& view)
{
    selcont.clear();

    if (! root) { return; }

    QStack qstack;
    qstack.push(root);

    while (qstack.size() > 0)
    {
        cout << endl << "stacksize: " << qstack.size() << " ";

        const Node *topn = qstack.top(); // top node
        const Node::Type& type = topn->type;
        qstack.pop();

        if (type == Node::LEAF)
        {
            cout << __FUNCTION__ << " LEAF:" << topn << " ";

            // add objs that are partially or fully visible in view

            const ObjV& objv = topn->leaves;
            cout << " has objv " << objv << endl;
            for (auto i=begin(objv); i != end(objv); i++)
            {
                if (! (*i)->hidden_from(view))
                {
                    selcont.push_back(*i);
                }
            }
        }
        else if (type == Node::QBRANCH)
        {
            const Vec& qpos = topn->qpos;
            const Vec& pos = view.pos;
            const Vec& dim = view.dim;

            const Node *q00 = topn->q00;
            const Node *q01 = topn->q01;
            const Node *q10 = topn->q10;
            const Node *q11 = topn->q11;
            const Node *qxx = topn->qxx;

            cout << __FUNCTION__ << " entering QBRANCH:" << topn << " has"
                 << " q00: " << (void*)q00
                 << " q01: " << (void*)q01
                 << " q10: " << (void*)q10
                 << " q11: " << (void*)q11
                 << " qxx: " << (void*)qxx
                 << endl;

            if      (pos(0) + dim(0) < qpos(0))
            {
                if      (pos(1) + dim(1) < qpos(1))
                {
                    if (q00) { qstack.push(q00); }
                }
                else if (pos(1)          > qpos(1))
                {
                    if (q01) { qstack.push(q01); }
                }
                else
                {
                    if (q00) { qstack.push(q00); }
                    if (q01) { qstack.push(q01); }
                }
            }
            else if (pos(0)          > qpos(0))
            {
                if      (pos(1) + dim(1) < qpos(1))
                {
                    if (q10) { qstack.push(q10); }
                }
                else if (pos(1)          > qpos(1))
                {
                    if (q11) { qstack.push(q11); }
                }
                else
                {
                    if (q10) { qstack.push(q10); }
                    if (q11) { qstack.push(q11); }
                }
            }
            else
            {
                if (q00) { qstack.push(q00); }
                if (q01) { qstack.push(q01); }
                if (q10) { qstack.push(q10); }
                if (q11) { qstack.push(q11); }
            }

            if (qxx) { qstack.push(qxx); }
        }
    }

    cout << endl;
}

void QTStorage::clear()
{
    if (! root) { return; }

    QStack qstack;
    qstack.push(root);

    while (qstack.size() > 0)
    {
        const Node *topn = qstack.top(); // top node
        const Node::Type& type = topn->type;
        qstack.pop();

        if (type == Node::LEAF)
        {
            // no dynamic objv to delete
        }
        else if (type == Node::QBRANCH)
        {
            if (topn->q00) { qstack.push(topn->q00); }
            if (topn->q01) { qstack.push(topn->q01); }
            if (topn->q10) { qstack.push(topn->q10); }
            if (topn->q11) { qstack.push(topn->q11); }
            if (topn->qxx) { qstack.push(topn->qxx); }
        }

        delete topn;
    }

    root = 0;
}
