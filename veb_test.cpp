
//          Copyright David Lawrence Bien 1997 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt).

// veb_test.cpp : Direct test Van Emde Boas tree implementation.
// dbien
// 22AUG2021

#include "bienutil_test.h"
#include "_util.h"
#include "_vebtree.h"
#include <_simpbv.h>
#include <random>

#error Work in progress

std::string g_strProgramName;

namespace ns_VebTreeTestDirect
{
using namespace ns_bienutil_test;
__BIENUTIL_USING_NAMESPACE

BienutilTestEnvironment * vpxteBienutilTestEnvironment{nullptr};

using ::testing::Types;

// Since we can't use both parameterized testing and "typed test suite" testing at the same time we integrate
//   the parameters into the types.
template < class t_TyVebWithParameters, uint64_t t_knElements, uint64_t t_knPercentagePopulated, uint64_t t_knIterations >
struct VebWithParameters
{
  typedef t_TyVebWithParameters _TyVebWithParameters;
  static const uint64_t s_knElements = t_knElements;
  static const uint64_t s_knPercentagePopulated = t_knPercentagePopulated;
  static const uint64_t s_knIterations = t_knIterations;
};

// TestVebTree:
// Test my new impl of UTF encoding conversions vs. ICU's open source library impl.
template < class t_TyVebWithParameters >
class TestVebTree : public BienutilTest
{
  typedef TestVebTree _TyThis;
  typedef BienutilTest _TyBase;
public:
  typedef t_TyVebWithParameters _TyVebWithParameters;
  typedef typename _TyVebWithParameters::_TyT _TyT;
  typedef _simple_bitvec< size_t > _TyBitvec;
protected:
  // SetUp() is run immediately before a test starts.
  void SetUp() override 
  {
  }
  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
  }

  void DoTestVebTree()
  {
  }

protected:
	bool m_fExpectFailure{false};
};

TYPED_TEST_SUITE_P( TestVebTree );

TYPED_TEST_P( TestVebTree, VebTreeTest1 )
{
  this->DoTestVebTree();
}

REGISTER_TYPED_TEST_SUITE_P(
    TestVebTree,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    VebTreeTest1);

typedef VebTree< ScalarHolder< size_t >, 0, 3 > _TyVebWithParameters_0_3;
typedef VebTree< ScalarHolder< size_t >, 1, 2 > _TyVebWithParameters_1_2;
typedef VebTree< ScalarHolder< size_t >, 1, 4 > _TyVebWithParameters_1_4;
typedef VebTree< ScalarHolder< size_t >, 2, 3 > _TyVebWithParameters_2_3;
typedef VebTree< ScalarHolder< size_t >, 3, 3 > _TyVebWithParameters_3_3;

typedef Types< _TyVebWithParameters_1_2, _TyVebWithParameters_3_3, _TyVebWithParameters_1_4, _TyVebWithParameters_2_3, _TyVebWithParameters_0_3 > vTyVebWithParametersTestTypes;

INSTANTIATE_TYPED_TEST_SUITE_P(VebTreeTestInstance,    // Instance name
                               TestVebTree,             // Test case name
                               vTyVebWithParametersTestTypes);  // Type list

int _TryMain( int argc, char **argv )
{
  uint32_t u32Seed{0};
  if ( argc > 1 ) // We may get called with a random seed which can be used to repeat tests that found issues.
    VerifyThrowSz( 1 == sscanf( argv[1], "%u", &u32Seed ), "Unable to get a random seed uin32 from [%s]", argv[1] );
  (void)testing::AddGlobalTestEnvironment( vpxteBienutilTestEnvironment = new BienutilTestEnvironment( u32Seed ) );
  return RUN_ALL_TESTS();
}
} // namespace ns_VebTreeTestDirect

int main( int argc, char **argv )
{
#ifdef WIN32
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif //WIN32
  __BIENUTIL_USING_NAMESPACE

  g_strProgramName = argv[0];
  ::testing::InitGoogleTest(&argc, argv);
  try
  {
    return ns_VebTreeTestDirect::_TryMain(argc, argv);
  }
  catch( const std::exception & rexc )
  {
    n_SysLog::Log( eslmtError, "%s: *** Exception: [%s]", g_strProgramName.c_str(), rexc.what() );
    fprintf( stderr, "%s: *** Exception: [%s]\n", g_strProgramName.c_str(), rexc.what() );      
    return -1;
  }
}

