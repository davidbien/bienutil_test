#pragma once

//          Copyright David Lawrence Bien 1997 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt).

// bienutil_test.h
// dbien

#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifndef NDEBUG
    #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
    #define DBG_NEW new
#endif
#else //!WIN32
#define DBG_NEW new
#endif //!WIN32

#include <memory>
#include <exception>
#include <optional>
#include <random>
typedef std::allocator< char >	_TyDefaultAllocator;
#include "_compat.h"
#include "_heapchk.h"
#include "syslogmgr.inl"
#include "_compat.inl"
#include "icu_strutil.h"
#include "gtest/gtest.h"

namespace ns_bienutil_test
{
  __BIENUTIL_USING_NAMESPACE

// BienutilTestEnvironment:
// We allow a random seed to be stored in this file so that we can replay scenarios for some unit testing.
class BienutilTestEnvironment : public ::testing::Environment
{
  typedef BienutilTestEnvironment _TyThis;
  typedef ::testing::Environment _TyBase;
public:
  explicit BienutilTestEnvironment()
  {
  }
  explicit BienutilTestEnvironment( uint32_t _u32Seed )
    : m_optu32RandSeed( _u32Seed )
  {
  }
protected:
  void SetUp() override 
	{
    // Set up the random seed either from a random devide or from a given seed.
    // Output the value of the seed to stdout so that the user knows the value to replicate any issues found.
    if ( !m_optu32RandSeed || !*m_optu32RandSeed )
    {
      // Setup the random seed randomly.
      std::random_device rd;
      m_optu32RandSeed = rd();
    }
    // Seed either randomly or with a value passed to the unit test allowing reproduction of a failed unit test.
    m_reRandomEngine.seed( *m_optu32RandSeed );
	}
  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
    // Nothing to do in TearDown() - we want to leave the generated unit test files so that they can be analyzed if there are any issues.
  }
public:

  size_t GetRandomRanged( size_t _nBegin, size_t _nEndIncl )
  {
    std::uniform_int_distribution< size_t > dist( _nBegin, _nEndIncl );
    return dist( m_reRandomEngine );
  }
  bool FRandomPercentage( size_t _nPerc )
  {
    Assert( _nPerc <= 100 );
    if ( !_nPerc )
      return false;
    if ( _nPerc >= 100 )
      return true;
    return _nPerc >= m_distPercent( m_reRandomEngine );
  }

  // If the application is invoked with a seed then it is set into this.
  std::optional< uint32_t > m_optu32RandSeed;
  std::default_random_engine m_reRandomEngine; // We seed this with the generated seed and then use it for random number generation.
  std::uniform_int_distribution< size_t > m_distPercent{ 1, 100 }; // utility to produce a percentage.
};

class BienutilTest : public testing::Test
{
  typedef BienutilTest _TyThis;
  typedef ::testing::Test _TyBase;
protected:
  void SetUp() override 
  { 
  }
  void TearDown() override 
  {
  }
};

} // namespace ns_bienutil_test
