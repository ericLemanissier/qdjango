/*
 * Copyright (C) 2010-2014 Jeremy Lainé
 * Copyright (C) 2011 Mathias Hasselmann
 * Contact: https://github.com/jlaine/qdjango
 *
 * This file is part of the QDjango Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef QDJANGO_QUERYSET_H
#define QDJANGO_QUERYSET_H

#include "QDjango.h"
#include "QDjangoWhere.h"
#include "QDjangoQuerySet_p.h"

/** \brief The QDjangoQuerySet class is a template class for performing
 *   database queries.
 *
 *  The QDjangoQuerySet template class allows you to define and manipulate
 *  sets of QDjangoModel objects stored in the database.
 *
 *  You can chain filter expressions using the filter() and exclude() methods
 *  or apply limits on the number of rows using the limit() method.
 *
 *  You can retrieve database values using the values() and valuesList()
 *  methods or retrieve model instances using the get() and at() methods.
 *
 *  You can also delete sets of objects using the remove() method.
 *
 *  Behinds the scenes, the QDjangoQuerySet class uses implicit sharing to
 *  reduce memory usage and avoid needless copying of data.
 *
 * \ingroup Database
 */
class QDJANGO_EXPORT  QDjangoQuerySet
{
public:
    /** \cond declarations for STL-style container algorithms */
    typedef int size_type;
    typedef QObject value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef qptrdiff difference_type;
    /** \endcond */

    /** The QDjangoQuerySet::const_iterator class provides an STL-style const iterator
     *  for QDjangoQuerySet.
     *
     *  QDjangoQuerySet::const_iterator allows you to iterate over a QDjangoQuerySet.
     *  As a const iterator it doesn't permit you to modify the QDjangoQuerySet.
     *
     *  The default QDjangoQuerySet::const_iterator constructor creates an uninitialized iterator. You must
     *  initialize it using a QDjangoQuerySet function like QDjangoQuerySet::constBegin(), or
     *  QDjangoQuerySet::constEnd() before you can start iterating. Here's a typical loop that
     *  prints all the objects stored in a set:
     *
     *  \code
     *  QDjangoQuerySet<Weblog::Post> posts;
     *
     *  foreach(const Weblog::Post &p, posts) {
     *      cout << p << endl;
     *  }
     *  \endcode
     */
    class const_iterator
    {
        friend class QDjangoQuerySet;

    public:
        /** A synonym for std::bidirectional_iterator_tag indicating this iterator
         *  permits bidirectional access.
         */
        typedef std::bidirectional_iterator_tag  iterator_category;

        /** \cond declarations for STL-style container algorithms */
        typedef qptrdiff difference_type;
        typedef QObject value_type;
        typedef QObject *pointer;
        typedef QObject &reference;
        /** \endcond */

        /** Constructs an uninitialized iterator.
         *
         *  Functions like operator*() and operator++() should not be called on an uninitialized
         *  iterator. Use const_iterator::operator=() to assign a value to it before using it.
         *
         *  \sa See also QDjangoQuerySet::constBegin() and QDjangoQuerySet::constEnd().
         */
        const_iterator()
            : m_querySet(0)
            , m_fetched(-1)
            , m_offset(0)
        {
        }

        /** Constructs a copy of \p other.
         */
        const_iterator(const const_iterator &other)
            : m_querySet(other.m_querySet)
            , m_fetched(-1)
            , m_offset(other.m_offset)
        {
        }

    private:
        const_iterator(const QDjangoQuerySet *querySet, int offset = 0)
            : m_querySet(querySet)
            , m_fetched(-1)
            , m_offset(offset)
        {
        }

    public:
        /** Returns the current item.
         *
         *  \sa operator->()
         */
        const QObject &operator*() const { return *t(); }

        /** Returns a pointer to the current item.
         *
         *  \sa operator*()
         */
        const QObject *operator->() const { return t(); }


        /** Returns \c true if \p other points to the same item as this iterator;
         *  otherwise returns \c false.
         *
         *  \sa operator!=()
         */
        bool operator==(const const_iterator &other) const
        {
            return m_querySet == other.m_querySet && m_offset == other.m_offset;
        }

        /** Returns \c true if \p other points to a different item than this iterator;
         *  otherwise returns \c false.
         *
         *  \sa operator==()
         */
        bool operator!=(const const_iterator &other) const
        {
            return m_querySet != other.m_querySet || m_offset != other.m_offset;
        }

        /** Returns \c true if other \p points to a position behind this iterator;
         *  otherwise returns \c false.
         */
        bool operator<(const const_iterator& other) const
        {
            return (m_querySet == other.m_querySet && m_offset < other.m_offset)
                    || m_querySet < other.m_querySet;
        }

        /** Returns \c true if other \p points to a position behind or equal this iterator;
         *  otherwise returns \c false.
         */
        bool operator<=(const const_iterator& other) const
        {
            return (m_querySet == other.m_querySet && m_offset <= other.m_offset)
                    || m_querySet < other.m_querySet;
        }

        /** Returns \c true if other \p points to a position before this iterator;
         *  otherwise returns \c false.
         */
        bool operator>(const const_iterator& other) const
        {
            return (m_querySet == other.m_querySet && m_offset > other.m_offset)
                    || m_querySet > other.m_querySet;
        }

        /** Returns \c true if other \p points to a position before or equal this iterator;
         *  otherwise returns \c false.
         */
        bool operator>=(const const_iterator& other) const
        {
            return (m_querySet == other.m_querySet && m_offset >= other.m_offset)
                    || m_querySet > other.m_querySet;
        }

        /** The prefix ++ operator (\c ++it) advances the iterator to the next item in the set
         *  and returns an iterator to the new current item.
         *
         *  Calling this function on QDjangoQuerySet::end() leads to undefined results.
         *
         *  \sa operator--()
         */
        const_iterator &operator++() { ++m_offset; return *this; }

        /** The postfix ++ operator (\c it++) advances the iterator to the next item in the set and
         *  returns an iterator to the previously current item.
         *
         *  Calling this function on QDjangoQuerySet::end() leads to undefined results.
         *
         *  \sa operator--(int)
         */
        const_iterator operator++(int) { const_iterator n(*this); ++m_offset; return n; }

        /** Advances the iterator by \p i items.
         * (If \p i is negative, the iterator goes backward.)
         *
         *  \sa operator-=() and operator+().
         */
        const_iterator &operator+=(int i) { m_offset += i; return *this; }

        /** Returns an iterator to the item at \p i positions forward from this iterator.
         * (If \p i is negative, the iterator goes backward.)
         *
         *  \sa operator-() and operator+=()
         */
        const_iterator operator+(int i) const { return const_iterator(m_querySet, m_offset + i); }

        /** Makes the iterator go back by \p i items.
         * (If \p i is negative, the iterator goes forward.)
         *
         * \sa operator+=() and operator-()
         */
        const_iterator &operator-=(int i) { m_offset -= i; return *this; }

        /** Returns an iterator to the item at \p i positions backward from this iterator.
         * (If \p i is negative, the iterator goes forward.)
         *
         *  \sa operator+() and operator-=()
         */
        const_iterator operator-(int i) const { return const_iterator(m_querySet, m_offset - i); }

        /** The prefix -- operator (\c --it) makes the preceding item current
         *  and returns an iterator to the new current item.
         *
         *  Calling this function on QDjangoQuerySet::begin() leads to undefined results.
         *
         *  \sa operator++().
         */
        const_iterator &operator--() { --m_offset; return *this; }

        /** The postfix -- operator (\c it--) makes the preceding item current
         *  and returns an iterator to the previously current item.
         *
         *  Calling this function on QDjangoQuerySet::begin() leads to undefined results.
         *
         *  \sa operator++(int).
         */
        const_iterator operator--(int) { const_iterator n(*this); --m_offset; return n; }


        /** Returns the number of items between the item pointed to by \p other
         *  and the item pointed to by this iterator.
         */
        difference_type operator-(const const_iterator &other) const { return m_offset - other.m_offset; }

    private:
        const QObject *t() const
        {
            if (m_fetched != m_offset && m_querySet) {
                if (const_cast<QDjangoQuerySet *>(m_querySet)->at(m_offset, &m_object)) {
                    m_fetched = m_offset;
                }
            }

            return m_fetched == m_offset ? &m_object : 0;
        }

    private:
        const QDjangoQuerySet *m_querySet;
        mutable int m_fetched;
        mutable QObject m_object;

        int m_offset;
    };

    /** Qt-style synonym for QDjangoQuerySet::const_iterator. */
    typedef const_iterator ConstIterator;

    QDjangoQuerySet(const QMetaObject &meta);
    QDjangoQuerySet(const QDjangoQuerySet &other);
    ~QDjangoQuerySet();

    QDjangoQuerySet all() const;
    QDjangoQuerySet exclude(const QDjangoWhere &where) const;
    QDjangoQuerySet filter(const QDjangoWhere &where) const;
    QDjangoQuerySet limit(int pos, int length = -1) const;
    QDjangoQuerySet none() const;
    QDjangoQuerySet orderBy(const QStringList &keys) const;
    QDjangoQuerySet selectRelated() const;

    int count() const;
    QDjangoWhere where() const;

    bool remove();
    int size();
    int update(const QVariantMap &fields);
    QList<QVariantMap> values(const QStringList &fields = QStringList());
    QList<QVariantList> valuesList(const QStringList &fields = QStringList());

    QObject *get(const QDjangoWhere &where, QObject *target = 0) const;
    QObject *at(int index, QObject *target = 0);

    const_iterator constBegin() const;
    const_iterator begin() const;

    const_iterator constEnd() const;
    const_iterator end() const;

    QDjangoQuerySet &operator=(const QDjangoQuerySet &other);

private:
    QDjangoQuerySetPrivate *d;
    const QMetaObject &m_metaObject;
};

#endif
