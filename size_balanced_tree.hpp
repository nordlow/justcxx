/*!
 * \file size-balanced-tree.hpp
 * \brief Size-Balanced Tree Template
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-06-11 20:27
 */

#pragma once

#include <iostream>

using namespace std;

template<typename T>
struct size_balanced_node
{
    size_balanced_node () { }
    size_balanced_node (size_balanced_node<T> *l, size_balanced_node<T> *r, int s, T k):
        lchild (l), rchild (r), size (s), key (k) {}
    int size;
    T key;
    size_balanced_node < T > *lchild, *rchild;
};

template<typename T>
class size_balanced_tree
{
public:
    size_balanced_tree<T>::size_balanced_tree () {
        m_null = new size_balanced_node<T> ();
        root = m_null;
        root->lchild = root->rchild = m_null;
        root->size = 0;
    }

    ~size_balanced_tree() { clear (); }

    void insert (const T & key);
    void erase (const T & key);

    T get_rank (int k) const
    {
        size_balanced_node<T> *tmp = root;
        for (;;)
        {
            if (tmp->lchild->size > k)
                tmp = tmp->lchild;
            else if (k > tmp->lchild->size)
            {
                k -= tmp->lchild->size + 1;
                tmp = tmp->rchild;
            }
            else
                break;
        }
        return tmp->key;
    }

    T get_min () const;
    T get_max () const;

    void clear() { clear (root); delete m_null; }
    void travel() { travel (root); }

private:
    size_balanced_node<T> *root; //< Tree Root Node.
    size_balanced_node<T> *m_null;

    void left_rotate (size_balanced_node<T> *&root);

    void right_rotate (size_balanced_node<T> *&root);

    void maintain (size_balanced_node<T> *&root, bool style);

    void insert (size_balanced_node<T> *&root, const T & key);

    void erase (size_balanced_node<T> *&root, const T & key);

    void clear (size_balanced_node<T> *root) {
        if (root != m_null) {
            clear(root->lchild);
            clear(root->rchild);
            delete root;
        }
    }

    void travel (size_balanced_node<T> *root) {
        if (root != m_null)
        {
            travel (root->lchild);
            cout << root->key << "  ";
            travel (root->rchild);
        }
    }
};

template<typename T>
inline T size_balanced_tree<T>::get_min () const
{
    if (root == m_null)
	return T ();
    size_balanced_node<T> *tmp = root;
    while (tmp->lchild != m_null)
	tmp = tmp->lchild;
    return tmp->key;
}

template<typename T>
inline T size_balanced_tree<T>::get_max () const
{
    if (root == m_null)
	return T();
    size_balanced_node<T> *tmp = root;
    while (tmp->rchild != m_null)
	tmp = tmp->rchild;
    return tmp->key;
}

template<typename T>
inline void size_balanced_tree<T>::erase (const T & key)
{
    erase (root, key);
}

template<typename T>
inline void size_balanced_tree<T>::erase (size_balanced_node<T> *&root, const T & key)
{
    if (root == m_null)
	return;
    root->size--;
    if (root->key == key)
    {
	size_balanced_node<T> *tmp;
	if (root->lchild != m_null && root->rchild == m_null)
	{
	    tmp = root;
	    root = tmp->lchild;
	    delete tmp;
	}
	else if (root->lchild == m_null && root->rchild == m_null)
	{
	    tmp = root;
	    root = m_null;
	    delete tmp;
	}
	else
	{
	    tmp = root->rchild;
	    while (tmp->lchild != m_null)
		tmp = tmp->lchild;
	    root->key = tmp->key;
	    erase (root->rchild, tmp->key);
	}
    }
    else if (key < root->key)
	erase (root->lchild, key);
    else
	erase (root->rchild, key);
}

template<typename T>
inline void size_balanced_tree<T>::insert (size_balanced_node<T> *&root, const T & key)
{
    if (root == m_null)
    {
	root = new size_balanced_node<T> (m_null, m_null, 1, key);
	return;
    }
    root->size++;
    if (key < root->key)
	insert (root->lchild, key);
    else
	insert (root->rchild, key);
    maintain (root, key >= root->key);
}

template<typename T>
inline void size_balanced_tree<T>::insert (const T & key)
{
    insert (root, key);
}

template<typename T>
inline void size_balanced_tree<T>::left_rotate (size_balanced_node<T> *&root)
{
    size_balanced_node<T> *tmp = root->rchild;
    root->rchild = tmp->lchild;
    tmp->lchild = root;
    tmp->size = root->size;
    root->size = root->lchild->size + root->rchild->size + 1;
    root = tmp;
}

template<typename T>
inline void size_balanced_tree<T>::right_rotate (size_balanced_node<T> *&root)
{
    size_balanced_node<T> *tmp = root->lchild;
    root->lchild = tmp->rchild;
    tmp->rchild = root;
    tmp->size = root->size;
    root->size = root->lchild->size + root->rchild->size + 1;
    root = tmp;
}

template<typename T>
inline void size_balanced_tree<T>::maintain (size_balanced_node<T> *&root, bool style)
{
    if (root == m_null)
	return;
    if (!style)
    {
	if (root->lchild->lchild->size > root->rchild->size)
	    right_rotate (root);
	else if (root->lchild->rchild->size > root->rchild->size)
	{
	    left_rotate (root->lchild);
	    right_rotate (root);
	}
	else
	    return;
    }
    else
    {
	if (root->rchild->rchild->size > root->lchild->size)
	    left_rotate (root);
	else if (root->rchild->lchild->size > root->lchild->size)
	{
	    right_rotate (root->rchild);
	    left_rotate (root);
	}
	else
	    return;
    }
    maintain (root->lchild, false);
    maintain (root->rchild, true);
    maintain (root, false);
    maintain (root, true);
}
