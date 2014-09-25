//          Copyright Oliver Kowalke 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_FIBER_MANAGER_H
#define BOOST_FIBERS_FIBER_MANAGER_H

#include <boost/assert.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/config.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/lock_types.hpp> 
#include <boost/utility.hpp>

#include <boost/fiber/algorithm.hpp>
#include <boost/fiber/detail/config.hpp>
#include <boost/fiber/detail/convert.hpp>
#include <boost/fiber/detail/fiber_base.hpp>
#include <boost/fiber/detail/main_fiber.hpp>
#include <boost/fiber/detail/spinlock.hpp>
#include <boost/fiber/detail/waiting_queue.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

# if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable:4251 4275)
# endif

namespace boost {
namespace fibers {

struct fiber_manager : private noncopyable
{
    fiber_manager() BOOST_NOEXCEPT;

    virtual ~fiber_manager() BOOST_NOEXCEPT;

    typedef detail::waiting_queue   wqueue_t;

    scoped_ptr< sched_algorithm >               def_algo;
    sched_algorithm                         *   sched_algo;
    wqueue_t                                    wqueue;
    bool                                        preserve_fpu;
    chrono::high_resolution_clock::duration     wait_interval;
    detail::main_fiber                          main_fiber;
    detail::fiber_base                      *   active_fiber;
};

void fm_resume_( detail::fiber_base *);

void fm_set_sched_algo( sched_algorithm *);

void fm_spawn( detail::fiber_base *);

void fm_priority( detail::fiber_base *, int) BOOST_NOEXCEPT;

void fm_wait_interval( chrono::high_resolution_clock::duration const&) BOOST_NOEXCEPT;
template< typename Rep, typename Period >
void fm_wait_interval( chrono::duration< Rep, Period > const& wait_interval) BOOST_NOEXCEPT
{ fm_wait_interval( wait_interval); }

chrono::high_resolution_clock::duration fm_wait_interval() BOOST_NOEXCEPT;

void fm_join( detail::fiber_base *);

detail::fiber_base * fm_active() BOOST_NOEXCEPT;

void fm_run();

void fm_wait( unique_lock< detail::spinlock > &);

bool fm_wait_until( chrono::high_resolution_clock::time_point const&,
                    unique_lock< detail::spinlock > &);

template< typename Clock, typename Duration >
bool fm_wait_until( chrono::time_point< Clock, Duration > const& timeout_time_,
                    unique_lock< detail::spinlock > & lk)
{
    chrono::high_resolution_clock::time_point timeout_time(
            detail::convert_tp( timeout_time_) );
    return fm_wait_until( timeout_time, lk);
}

template< typename Rep, typename Period >
bool fm_wait_for( chrono::duration< Rep, Period > const& timeout_duration,
                  unique_lock< detail::spinlock > & lk)
{
    return wait_until( chrono::high_resolution_clock::now() + timeout_duration, lk);
}

void fm_yield();

chrono::high_resolution_clock::time_point fm_next_wakeup();

void fm_migrate( detail::fiber_base *);

bool fm_preserve_fpu_();
void  fm_preserve_fpu_( bool);

}}

# if defined(BOOST_MSVC)
# pragma warning(pop)
# endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_FIBER_MANAGER_H
