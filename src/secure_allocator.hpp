/*
    Copyright (c) 2007-2019 Contributors as noted in the AUTHORS file

    This file is part of libzmq, the ZeroMQ core engine in C++.

    libzmq is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    As a special exception, the Contributors give you permission to link
    this library with independent modules to produce an executable,
    regardless of the license terms of these independent modules, and to
    copy and distribute the resulting executable under terms of your choice,
    provided that you also meet, for each linked independent module, the
    terms and conditions of the license of that module. An independent
    module is a module which is not derived from or based on this library.
    If you modify this library, you must extend this exception to your
    version of the library.

    libzmq is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_SECURE_ALLOCATOR_HPP_INCLUDED__
#define __ZMQ_SECURE_ALLOCATOR_HPP_INCLUDED__

#include "platform.hpp"
#include "macros.hpp"

#ifdef ZMQ_HAVE_CURVE

#if defined(ZMQ_USE_LIBSODIUM)
#include "sodium.h"
#endif

#include <memory>
#include <new>
#include <type_traits>

namespace zmq
{
template <class T> struct secure_allocator_t
{
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;

    secure_allocator_t () ZMQ_DEFAULT;

    template <class U>
    secure_allocator_t (const secure_allocator_t<U> &) ZMQ_NOEXCEPT
    {
    }
    T *allocate (std::size_t n) ZMQ_NOEXCEPT
    {
 #if defined(ZMQ_USE_LIBSODIUM)
        T *res = static_cast<T *> (sodium_allocarray (n, sizeof (T)));
        alloc_assert (res);
        return res;
 #else
        return static_cast<T *> (::operator new (n * sizeof (T)));
 #endif
    }
    void deallocate (T *p, std::size_t) ZMQ_NOEXCEPT
    {
 #if defined(ZMQ_USE_LIBSODIUM)
        sodium_free (p);
 #else
        ::operator delete (p);
 #endif
    }
    template <class U> struct rebind
    {
        typedef secure_allocator_t<U> other;
    };

    void construct (pointer p, const_reference val)
    {
        new ((void *) p) value_type (val);
    }
    void destroy (pointer p) { p->~value_type (); }
    size_type max_size () const { return SIZE_MAX; }
};
template <class T, class U>
bool operator== (const secure_allocator_t<T> &, const secure_allocator_t<U> &)
{
    return true;
}
template <class T, class U>
bool operator!= (const secure_allocator_t<T> &, const secure_allocator_t<U> &)
{
    return false;
}
}

#endif

#endif
