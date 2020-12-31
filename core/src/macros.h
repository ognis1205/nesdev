/*
 * NesDev:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NESDEV_CORE_MACROS_H_
#define _NESDEV_CORE_MACROS_H_

#if (defined(__pp_exceptions) \
     || defined(__EXCEPTIONS) \
     || defined(_CPPUNWIND)) \
  && !defined(NESDEV_CORE_NO_THROW_EXCEPTION)
#  define NESDEV_CORE_THROW(exception) throw exception
#  define NESDEV_CORE_TRY try
#  define NESDEV_CORE_CATCH(exception) catch (exception)
#else
#  include <cstdlib>
#  define NESDEV_CORE_THROW(exception) std::abort()
#  define NESDEV_CORE_TRY if (true)
#  define NESDEV_CORE_CATCH(exception) if (false)
#endif

#if !defined(NESDEV_CORE_ASSERT)
#  include <cassert>
#  define NESDEV_CORE_ASSERT(x) assert(x)
#endif

#if defined(NESDEV_CORE_TEST)
#  define NESDEV_CORE_PROTECTED_UNLESS_TESTED public
#  define NESDEV_CORE_PRIVATE_UNLESS_TESTED   public
#else
#  define NESDEV_CORE_PROTECTED_UNLESS_TESTED protected
#  define NESDEV_CORE_PRIVATE_UNLESS_TESTED   private
#endif

#endif  // ifndef _NESDEV_CORE_MACROS_H_
