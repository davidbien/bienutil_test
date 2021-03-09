#pragma once

// bienutil_test.h

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
typedef std::allocator< char >	_TyDefaultAllocator;
#include "_compat.h"
#include "_heapchk.h"

namespace vs_bienutil_test
{

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

} // namespace vs_bienutil_test
