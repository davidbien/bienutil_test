
//          Copyright David Lawrence Bien 1997 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt).

// shared_obj_test.cpp : Direct test new SharedPtr impl.
// dbien
// 16APR2021

#include "bienutil_test.h"
#include "shared_obj.h"
#include <utility>
#include "_util.h"

std::string g_strProgramName;

namespace ns_SharedPtrTestDirect
{
using namespace ns_bienutil_test;
__BIENUTIL_USING_NAMESPACE

BienutilTestEnvironment * vpxteBienutilTestEnvironment{nullptr};

using ::testing::Types;

// TSharedObj1: Destructor doesn't throw.
class TSharedObj1 : public SharedObjectBase< true >
{
  typedef TSharedObj1 _TyThis;
  typedef SharedObjectBase< true > _TyBase;
public:
  TSharedObj1()
  {
  }
  ~TSharedObj1() override
  {
  }
  string m_str{"01345678900134567890013456789001345678900134567890"}; // make sure to blow out string's internal buffer.
};

class TDerivedObj1 : public TSharedObj1
{
  typedef TDerivedObj1 _TyThis;
  typedef TSharedObj1 _TyBase;
public:
  TDerivedObj1()
  {
  }
  ~TDerivedObj1() override
  {
  }
};

// TestSharedObj:
// Test my new impl of UTF encoding conversions vs. ICU's open source library impl.
template < class t_PrTySharedEls >
class TestSharedObj : public BienutilTest
{
  typedef TestSharedObj _TyThis;
  typedef BienutilTest _TyBase;
public:
  typedef typename t_PrTySharedEls::first_type _TySharedEl;
  typedef typename t_PrTySharedEls::second_type _TySharedElDerived;

protected:
  // SetUp() is run immediately before a test starts.
  void SetUp() override 
  {
  }
  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
  }
  
  void DoTestSharedObj()
  {
    // Let's try some stuff:
    SharedPtr< _TySharedEl > spse;
    spse.emplace();
    SharedPtr< const _TySharedEl > spcse;
    spcse.emplace();
    spcse = spse;
    SharedPtr< volatile _TySharedEl > spvse;
    spvse.emplace();
    spvse = spse;
    SharedPtr< const volatile _TySharedEl > spcvse( spcse );
    spcvse = spcse;
    spcvse = spvse;
    spcvse = spse;
    // To test compiling these just uncomment them one by one and compile the unit test... I know, annoying.
    // spse = spcvse; // this should fail to compile.
    // spse = spcse; // this should fail to compile.
    // spcse = spcvse; // this should fail to compile.

    SharedPtr< _TySharedElDerived > spseDerived;
    spse = spseDerived;
    spcse = spseDerived;
    spvse = spseDerived;
    spcvse = spseDerived;
    
    // spseDerived = spse; // this should fail to compile.
    spse.template emplaceDerived< _TySharedElDerived >();
    // This should fail to compile because we are creating a const _TySharedElDerived into a non-const container.
    // spse.template emplaceDerived< const _TySharedElDerived >();

    // in-place construct a derived object:
    SharedPtr< const _TySharedEl > spcseDerived( in_place_type< _TySharedElDerived > );
  }
protected:
	bool m_fExpectFailure{false};
};

TYPED_TEST_SUITE_P( TestSharedObj );

TYPED_TEST_P( TestSharedObj, SharedPtrTest1 )
{
  this->DoTestSharedObj();
}

REGISTER_TYPED_TEST_SUITE_P(
    TestSharedObj,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    SharedPtrTest1);

typedef std::pair< TSharedObj1, TSharedObj1 > _TyPrTest01;
typedef std::pair< TSharedObj1, TDerivedObj1 > _TyPrTest02;

typedef Types<_TyPrTest01, _TyPrTest02 > vTySharedPtrTestTypes;

INSTANTIATE_TYPED_TEST_SUITE_P(SharedPtrTestInstance,    // Instance name
                               TestSharedObj,             // Test case name
                               vTySharedPtrTestTypes);  // Type list

int _TryMain( int argc, char **argv )
{
  return RUN_ALL_TESTS();
}
} // namespace ns_SharedPtrTestDirect

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
    return ns_SharedPtrTestDirect::_TryMain(argc, argv);
  }
  catch( const std::exception & rexc )
  {
    n_SysLog::Log( eslmtError, "%s: *** Exception: [%s]", g_strProgramName.c_str(), rexc.what() );
    fprintf( stderr, "%s: *** Exception: [%s]\n", g_strProgramName.c_str(), rexc.what() );      
    return -1;
  }
}

