/*! \file bbox.hpp
 * \brief Bounding Box.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <ostream>
#include <limits>
#include <vector>
#include <memory>
#include "box.hpp"
#include "../split.hpp"
#include "../show.hpp"

/*! Bounding Box in N Dimensions.
 * Contains a vector subs of void pointers where each elements (at position i)
 * points either to an object of type bbox or a box2
 * depending on the bool value at the corresponding position i in flags.
 */
template<class T, std::size_t N>
class bbox
{
public:
    bbox() : m_box(vec<T,N>(std::numeric_limits<T>::max()),
                   vec<T,N>(std::numeric_limits<T>::min())) {}

    bbox(const std::vector<void *>& subs,
         const std::vector<bool>& flags);

    bbox(std::shared_ptr<box<T,N> > a,
         std::shared_ptr<box<T,N> > b)
        : m_subs(2),
          m_flags(2, false)
    {
        m_box = unite(a.get(),
                      b.get());
        m_subs[0] = a;
        m_subs[1] = b;
    }
    bbox(bbox* a,
         bbox* b)
        : m_subs(2),
          m_flags(2, true)
    {
        m_box = unite(a->m_box,
                      b->m_box);
        m_subs[0] = a;
        m_subs[1] = b;
    }

    /// Reset to the argument.
    void reset_to(const box<T,N>& a) { m_box = a; }

    /// Extend using the argument.
    void extend_to(const box<T,N>& a) { m_box = unite(m_box, a); }

    /// Reset the bbox to all its subs.
    void reset_all();

    /// Insert a reference to \p a.
    void insert_ref_to(box<T,N>* a);

    /*! Remove one or many references to \p a.
     * \param found set to true if any reference(s) was found and removed.
     * \param all if given as true all references found are zeroed, not only
     * the first found.
     * \param do_delete if true the reference is also (if found)
     * deleted.
     */
    bbox* remove_ref_to(box<T,N>* a, bool& found,
                         bool all, bool do_delete);

    /*! Recursively balance the tree (where this is the root) of bounding boxes.
     * \param flag is set to true if we returned a bbox, otherwise false.
     */
    void* balance(bool& flag, unsigned int subnum_max = 4);

    /// Print the leaves of the tree.
    void print_leaves(std::ostream& os) const;

    /*! Print the leaves at the given depth, meaning an indentation of 4 * depth.
     * Recursively defined to print all the subtrees aswell.
     */
    void print_at(std::ostream& os, unsigned int depth) const;

    /*! Select a set of objects that are partially or fully visible inside of
     * the in argument vrect.
     */
    void select(const box<T,N>& vrect, // view rect
                std::vector< box<T,N>* >& robjs) const; // returned objs
private:
    box<T,N> m_box;

    std::vector<void*> m_subs;

    /*! Each Bit indicates whether the corresponding
     * sub is a bbox (true) or a box2 (false).
     */
    std::vector<bool> m_flags;
};

template<class T, std::size_t N>
bbox<T,N>::bbox(const std::vector<void *>& subs,
                const std::vector<bool>& flags)
    : m_box(0),
      m_subs(subs),
      m_flags(flags)
{
    if (subs.empty()) { return; }
    if (flags[0]) {
        bbox<T,N> * a = reinterpret_cast<bbox<T,N> *>(subs[0]);
        m_box = a->m_box;
    } else {
        box<T,N> * a = reinterpret_cast<box<T,N> *>(subs[0]);
        m_box = *a;
    }
    for (unsigned int i = 1; i < subs.size(); i++) {
        if (flags[i]) {
            bbox<T,N> * a = reinterpret_cast<bbox<T,N> *>(subs[i]);
            m_box = unite(m_box, a->m_box);
        } else {
            box<T,N> * a = reinterpret_cast<box<T,N> *>(subs[i]);
            m_box = unite(m_box, *a);
        }
    }
}

template<class T, std::size_t N>
void bbox<T,N>::reset_all()
{
    if (m_subs.empty()) {
        m_box = bbox<T,N>(box<T,N>(0)); // no m_subs => this m_box becomes (0,0,0,0)
        return;
    }
    reset_to(m_subs[0]);
    for (unsigned int i = 0; i < m_subs.size(); i++) {
        bbox<T,N> * s = reinterpret_cast<bbox<T,N> *>(m_subs[i]);
        extend_to(s);
    }
}

template<class T, std::size_t N>
void bbox<T,N>::insert_ref_to(box<T,N> * a)
{
    for (unsigned int i = 0; i < m_subs.size(); i++) {
        if (m_flags[i])            // if its a bbox
        {
            bbox<T,N> * s = reinterpret_cast<bbox<T,N> *>(m_subs[i]);
            if (a->inside(s->m_box)) // and if its inside it
            {
                return s->insert_ref_to(a);   // recurse
            }
        }
    }
    // otherwise we add it to this bbox-node
    m_subs.push_back(a);          // add it
    m_flags.push_back(false);
    extend_to(*a);
}

template<class T, std::size_t N>
bbox<T,N> * bbox<T,N>::remove_ref_to(box<T,N> * a, bool& found,
                                     bool all, bool do_delete)
{
    found = false;

    // if we delete we must remove all references
    if (do_delete)
    {
        all = true;
    }

    for (unsigned int i = 0; i < m_subs.size(); i++)
    {
        if (m_flags[i])            // if its a bbox
        {
            bbox<T,N> *& s = reinterpret_cast<bbox<T,N> * &>(m_subs[i]);
            if (a->inside(s->m_box)) // and if its inside it
            {
                s = s->remove_ref_to(a, found, all, do_delete);
            }
        }
        else
        {
            box<T,N> *& s = reinterpret_cast<box<T,N> * &>(m_subs[i]);
            if (s == a)
            {
                found = true;
                if (do_delete)
                {
                    delete s;
                    do_delete = false; // delete only the first
                }
                s = 0;		// remove ref
            }
        }
    }

    if (m_subs.size() > 0)
    {
        return this;
    }
    else
    {
        delete this;
        return 0;
    }
}

template<class T, std::size_t N>
void * bbox<T,N>::balance(bool& flag, unsigned int subnum_max)
{
    // depth-first recurse to children

    std::vector<void *> nnsubs; // non-null m_subs
    std::vector<bool> nnflags; // and its m_flags
    for (unsigned int i = 0; i < m_subs.size(); i++)
    {
        if (m_flags[i])		// if bbox balance it
        {
            bbox<T,N> * b = reinterpret_cast<bbox<T,N> *>(m_subs[i]);
            bool f;
            void * c = b->balance(f, subnum_max);
            if (c)
            {
                nnsubs.push_back(c); // append non-null sub
                nnflags.push_back(f);
            }

        }
        else			// otherwise copy it
        {
            box<T,N> * b = reinterpret_cast<box<T,N> *>(m_subs[i]);
            nnsubs.push_back(b);
            nnflags.push_back(false);
        }
    }
    m_subs = nnsubs;              // update m_subs
    m_flags = nnflags;

    nnsubs.resize(0);           // save memory space
    nnflags.resize(0);           // save memory space

    // do the real balancing at this level

    const unsigned int n = m_subs.size();

    if (n == 0)                 // box has no m_subs so remove everything
    {
        delete this;
        flag = false;
        return 0;
    }
    else if (n == 1)            // replace box with its only sub
    {
        void * sub0 = m_subs[0];
        flag = m_flags[0];
        delete this;
        return sub0;
    }
    else if (n <= subnum_max)	// leave untouched
    {
        flag = true;
        return this;
    }
    else
    {
        // split m_subs in two parts and create a new bbox for these

        std::vector<void *> a, b;
        std::vector<bool> ai, bi;

        pnw::mid_split(m_subs, a, b);
        pnw::mid_split(m_flags, ai, bi);

        delete this;

        flag = true;
        return new bbox<T,N>(new bbox<T,N>(a, ai),
                             new bbox<T,N>(b, bi));
    }
}

template<class T, std::size_t N>
void bbox<T,N>::select(const box<T,N>& vrect,
                       std::vector< box<T,N>* >& robjs) const
{
    for (auto i = begin(m_subs); i != end(m_subs); i++) {
        const box<T,N> * const& o = *i;

        if (o->has_typename("bbox<T,N>"))
        {
            const bbox<T,N> * const& b =
                reinterpret_cast<const bbox<T,N> * const &>(o);
            const box<T,N>& r = b->rect;

            if (vrect.overlap(r))
            {
                b->select(vrect, robjs);
            }
        }
        else
        {
            robjs.push_back(const_cast<box<T,N>*>(o));
        }
    }
}

template<class T, std::size_t N>
void bbox<T,N>::print_leaves(std::ostream& os) const
{
    for (unsigned int i = 0; i < m_subs.size(); i++)
    {
        if (m_flags[i])         // if its a bbox
        {
            bbox<T,N> * s = reinterpret_cast<bbox<T,N> *>(m_subs[i]);
            s->print_leaves(os);   // recurse
        }
        else
        {
            box<T,N> * s = reinterpret_cast<box<T,N> *>(m_subs[i]);
            os << *s;
            if (i != m_subs.size() - 1)
            {
                os << " ";
            }
        }
    }
}

#include "../ostream_x.hpp"

template<class T, std::size_t N>
void bbox<T,N>::print_at(std::ostream& os, unsigned int depth) const
{
    os_spaces(os, depth * 4);
    os << "[" << std::endl;
    for (unsigned int i = 0; i < m_subs.size(); i++) {
        if (m_flags[i])         // if its a bbox
        {
            bbox<T,N> * s = reinterpret_cast<bbox<T,N> *>(m_subs[i]);
            s->print_at(os, depth + 1);	// recurse
        } else {
            box<T,N> * s = reinterpret_cast<box<T,N> *>(m_subs[i]);
            os_spaces(os, (depth + 1) * 4);
            os << *s << endl;
        }
    }
    os_spaces(os, depth * 4);
    os << "]" << std::endl;
}
