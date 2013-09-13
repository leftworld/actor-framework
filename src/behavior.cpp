/******************************************************************************\
 *           ___        __                                                    *
 *          /\_ \    __/\ \                                                   *
 *          \//\ \  /\_\ \ \____    ___   _____   _____      __               *
 *            \ \ \ \/\ \ \ '__`\  /'___\/\ '__`\/\ '__`\  /'__`\             *
 *             \_\ \_\ \ \ \ \L\ \/\ \__/\ \ \L\ \ \ \L\ \/\ \L\.\_           *
 *             /\____\\ \_\ \_,__/\ \____\\ \ ,__/\ \ ,__/\ \__/.\_\          *
 *             \/____/ \/_/\/___/  \/____/ \ \ \/  \ \ \/  \/__/\/_/          *
 *                                          \ \_\   \ \_\                     *
 *                                           \/_/    \/_/                     *
 *                                                                            *
 * Copyright (C) 2011-2013                                                    *
 * Dominik Charousset <dominik.charousset@haw-hamburg.de>                     *
 *                                                                            *
 * This file is part of libcppa.                                              *
 * libcppa is free software: you can redistribute it and/or modify it under   *
 * the terms of the GNU Lesser General Public License as published by the     *
 * Free Software Foundation; either version 2.1 of the License,               *
 * or (at your option) any later version.                                     *
 *                                                                            *
 * libcppa is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 * See the GNU Lesser General Public License for more details.                *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with libcppa. If not, see <http://www.gnu.org/licenses/>.            *
\******************************************************************************/


#include "cppa/behavior.hpp"
#include "cppa/partial_function.hpp"

namespace cppa {

class continuation_decorator : public detail::behavior_impl {

    typedef behavior_impl super;

 public:

    typedef typename behavior_impl::pointer pointer;

    continuation_decorator(const partial_function& fun, pointer ptr)
    : super(ptr->timeout()), m_fun(fun), m_decorated(std::move(ptr)) {
        CPPA_REQUIRE(m_decorated != nullptr);
    }

    template<typename T>
    inline optional<any_tuple> invoke_impl(T& tup) {
        auto res = m_decorated->invoke(tup);
        if (res) return m_fun(*res);
        return none;
    }

    optional<any_tuple> invoke(any_tuple& tup) {
        return invoke_impl(tup);
    }

    optional<any_tuple> invoke(const any_tuple& tup) {
        return invoke_impl(tup);
    }

    bool defined_at(const any_tuple& tup) {
        return m_decorated->defined_at(tup);
    }

    pointer copy(const generic_timeout_definition& tdef) const {
        return new continuation_decorator(m_fun, m_decorated->copy(tdef));
    }

    void handle_timeout() { m_decorated->handle_timeout(); }

 private:

    partial_function m_fun;
    pointer m_decorated;

};

behavior::behavior(const partial_function& fun) : m_impl(fun.m_impl) { }

behavior behavior::add_continuation(const partial_function& fun) {
    return {new continuation_decorator(fun, m_impl)};
}


} // namespace cppa
