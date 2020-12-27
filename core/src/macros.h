/*
 * nes-emulator:
 * Emulator for the Nintendo Entertainment System (R) Archetecture.
 * Written by and Copyright (C) 2020 Shingo OKAWA shingo.okawa.g.h.c@gmail.com
 * Trademarks are owned by their respect owners.
 */
#ifndef _NES_CORE_MACRO_SCOPE_H_
#define _NES_CORE_MACRO_SCOPE_H_

#if (defined(__pp_exceptions) \
     || defined(__EXCEPTIONS) \
     || defined(_CPPUNWIND)) \
  && !defined(NES_CORE_NOEXCEPTION)
#  define NES_CORE_THROW(exception) throw exception
#  define NES_CORE_TRY try
#  define NES_CORE_CATCH(exception) catch (exception)
#else
#  include <cstdlib>
#  define NES_CORE_THROW(exception) std::abort()
#  define NES_CORE_TRY if (true)
#  define NES_CORE_CATCH(exception) if (false)
#endif

#if !defined(NES_CORE_ASSERT)
#  include <cassert>
#  define NES_CORE_ASSERT(x) assert(x)
#endif

#if defined(NES_CORE_TESTS_PRIVATE)
#  define NES_CORE_PRIVATE_UNLESS_TESTED public
#else
#  define NES_CORE_PRIVATE_UNLESS_TESTED private
#endif

#endif  // ifndef _NES_CORE_MACRO_SCOPE_H_
