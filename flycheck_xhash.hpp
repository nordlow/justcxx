/*!
 * \file xhash.hpp
 * \brief Hash Table C++ Wrappers for libght.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Provides C++ Wrappers for libght.
 */

#pragma once
#include "phash.h"

class xHash
{
    typedef ght_iterator_t iterator_t;
public:
    /*! \em Construct Hash Table with \p i_size the number of buckets. */
    xHash(unsigned int i_size = 0) :m_ht(nullptr) { if (i_size) { check_init(i_size); } else { m_ht = nullptr; } }

    ~xHash() { clear(); }

    /*!
     * \em Check/Assure that Hash Table is \em Initialized and if need creates a
     *     hash-table with \p i_size the number of buckets.
     *
     * \return true if table was initialized, false otherwise.
     */
    bool check_init(unsigned int i_size = 1) { if (!m_ht) { m_ht = ght_create(i_size); return true; } else { return false; } }

    /*!
     * \em Insert the \em Data Pointer \p data at \p key of size \p key_data.
     * \return 0 upon success, -1 if data indexed by \p key already
     *         existed in \p p_ht.
     */
    int insert(void* data, unsigned int key_size, const void *key_data) { check_init(); return ght_insert(m_ht, data, key_size, key_data); }

    /*!
     * \em Insert the \em Data Pointer \p data at \p key.
     * \return 0 upon success, -1 if data indexed by \p key already
     *         existed in \p p_ht.
     */
    template <class T>
    int insert(void* data, const T & key) { check_init(); return ght_insert(m_ht, data, sizeof(key), &key); }

    /*!
     * \em Insert Empty (\c nullptr) Pointer at \p key.
     * \return 0 upon success, -1 if data indexed by \p key already
     *         existed in \p p_ht.
     */
    template <class T>
    int insert(const T & key) { check_init(); return ght_insert(m_ht, nullptr, sizeof(key), &key); }

    /*!
     * \em Get the \em Data Pointer stored at \p key_data of size \p key_data.
     * \return the data pointer upon success, nullptr if no data was
     *         indexed by \p key.
     */
    void* get(unsigned int key_size, const void *key_data) const { return m_ht ? ght_get(m_ht, key_size, key_data) : nullptr; }

    /*!
     * \em Get the \em Data Pointer stored at \p key.
     * \return the data pointer upon success, nullptr if no data was
     *         indexed by \p key.
     */
    template <class T>
    void* get(const T & key) const { return m_ht ? ght_get(m_ht, sizeof(key), &key) : nullptr; }

    /*!
     * \em Remove the \em Data Pointer at \p key of size \p key_data.
     * \return a pointer to the removed entry or nullptr if the entry
     *         could be found.
     */
    void* remove(unsigned int key_size, const void *key_data) { return m_ht ? ght_remove(m_ht, key_size, key_data) : nullptr; }

    /*!
     * \em Remove the \em Data Pointer at \p key.
     * \return a pointer to the removed entry or nullptr if the entry
     *         could be found.
     */
    template <class T>
    void* remove(const T & key) { return m_ht ? ght_remove(m_ht, sizeof(key), &key) : nullptr; }

    /*!
     * \em Replace the Entry Data at \p key with \p data.
     * \return a pointer to the old value data or nullptr if no entry at
     *         \p key exists.
     */
    template <class T>
    void* replace(void* data, const T & key) { return m_ht ? ght_replace(m_ht, data, sizeof(key), &key) : nullptr; }

    /*! Get the \em Size (the number of items) of \p this. */
    size_t size() const { return m_ht ? ght_size(m_ht) : 0; }

    /*! Return the first entry in \p this. This function should be used for
     *  iteration and is used together with \c next(). Note that you \em cannot
     *  assume anything about the \em order in which the entries are
     *  accessed. If an entry is inserted during an iteration, the entry might
     *  or might not occur in the iteration. Note that removal during an
     *  iteration is only safe for the current entry or an entry which has
     *  already been iterated over.
     *
     * \return a pointer to the first entry or nullptr if there are no entries.
     */
    void* first(iterator_t & iter, const void * & p_key) { return m_ht ? ght_first(m_ht, &iter, &p_key) : nullptr; }

    /*! Return the next entry in the hash table. This function should be used
     *  for iteration, and must be called after \c first().
     *
     * \return a pointer to the next entry (data) or nullptr if there are no more
     * entries.
     */
    void* next(iterator_t & iter, const void * & p_key) { return m_ht ? ght_first(m_ht, &iter, &p_key) : nullptr; }

    /*! Empty Hash Table. */
    void clear() { if (m_ht) { ght_finalize(m_ht); m_ht = nullptr; } }

    /*!
     * \em Coarse-Grained Rehashing of \p this.
     *
     * Note: this is exactly what \c ght_set_rehash() does for
     * automatically but we want this only in idle times.
     * \return 1 if rehashing was performed, 0 otherwise.
     */
    int rehash_coarse() const { // Note: const because it does the data
        bool rehash = false;
        if (m_ht)  {
            xHash * nc = const_cast<xHash*>(this);
            rehash = ght_rehash_coarse(nc->m_ht);
        }
        return rehash;
    }

private:
    ght_hash_table_t *m_ht;
};
