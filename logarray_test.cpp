// utf_test_direct.cpp : Direct test UTF conversions.
//

#include "bienutil_test.h"
#include "_logarray.h"
#include "_util.h"

std::string g_strProgramName;

namespace ns_LogarrayTestDirect
{
using namespace ns_bienutil_test;
__BIENUTIL_USING_NAMESPACE

BienutilTestEnvironment * vpxteBienutilTestEnvironment{nullptr};

using ::testing::Types;

// TestLogArray:
// Test my new impl of UTF encoding conversions vs. ICU's open source library impl.
template < class t_TyLogArray >
class TestLogArray : public BienutilTest
{
  typedef TestLogArray _TyThis;
  typedef BienutilTest _TyBase;
public:
  typedef t_TyLogArray _TyLogArray;
  typedef typename _TyLogArray::_TyT _TyT;
protected:
  // SetUp() is run immediately before a test starts.
  void SetUp() override 
  {
  }
  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
  }
  
  static const size_t s_knCountTestFixedBlocks = 13;
    // This defines the starting point size of each test on a given type.
  static const size_t s_knMaxCountTestElements = _TyLogArray::s_knElementsFixedBoundary + s_knCountTestFixedBlocks * ( 1 << _TyLogArray::s_knPow2Max );
    // We will test all sizes equal to or less than this.
  void DoTestLogArray()
  {
    size_t nElementsCur = 0;
    const size_t knElementsMax = s_knMaxCountTestElements;
    for ( ; nElementsCur != knElementsMax; ++nElementsCur )
    {
      _TyLogArray la( nElementsCur );
      _TyT tCur{};
      la.ApplyContiguous( 0, nElementsCur,
        [&tCur]( _TyT * _ptBegin, _TyT * _ptEnd )
        {
          std::generate_n( _ptBegin, _ptEnd - _ptBegin, 
            [&tCur]() { return tCur++; }
          );
        }
      );
      
      _TestSizedLogArray( la, true );
      _TestSizedLogArray( la, false );
    }
  }
  // Since we are using size_t we similate the unconstructed element at the end, but the effect is exactly the same...
  void _TestSizedLogArray( _TyLogArray & _la, bool _fWithUnconstructedElementAtEnd )
  {
    _VerifyLogArray( _la );
    if ( _fWithUnconstructedElementAtEnd )
    {
      _la._PbyAllocEnd();
      Assert( _la.NElements() + 1 == _la.NElementsAllocated() );
    }
    else
      Assert( _la.NElements() == _la.NElementsAllocated() );
    
    // Due to horrendous boundary conditions we want to exhaustively test removal from end scenarios:
    size_t nNewElements = _la.NElements();
    for (  )

  }
  void _VerifyLogArray( _TyLogArray const & _la )
  {
    // This moves through each element using the ElGet() method:
    _TyT nElValueCur = 0;
    size_t nElements = _la.NElements();
    for ( size_t nElCur = 0; nElCur < nElements; ++nElCur )
      Assert( nElValueCur++ == _la.ElGet( nElCur ) );
  }
  void _VerifyLogArrayContigRange( _TyLogArray const & _la )
  {
    // So we peruse every potential view of the log array and some erroneous ones and verify each:
    for ( size_t nViewStart = 0; nViewStart < _la.NElements() + 10; ++nViewStart )
    {
      for ( size_t nViewEnd = 0; nViewEnd < _la.NElements() + 10; ++nViewEnd )
      {
        bool fExpectFailure = ( nViewStart < nViewEnd ) || ( nViewStart > _la.NElements() );
        try
        {
          _TyT nCurEl = nViewStart;
          _la.ApplyContiguous( nViewStart, nViewEnd,
            [&nCurEl]( _TyT * _ptBegin, _TyT * _ptEnd )
            {
              for ( _TyT * ptCur = _ptBegin; _ptEnd != ptCur; ++ptCur )
                Assert( nCurEl++ == *ptCur );
            }
          );
          VerifyThrowSz( !fExpectFailure, "Expected failure but succeeded." );
        }
        catch( exception const & )
        {
          if ( !fExpectFailure )
            throw;
        }
      }
    }
  }
protected:
	bool m_fExpectFailure{false};
};

TYPED_TEST_SUITE_P( TestLogArray );

TYPED_TEST_P( TestLogArray, LogArrayTest1 )
{

}

REGISTER_TYPED_TEST_SUITE_P(
    TestLogArray,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    LogArrayTest1);

typedef LogArray< size_t, 0, 5 > _TyLogArray_0_5;
typedef LogArray< size_t, 1, 2 > _TyLogArray_1_2;
typedef LogArray< size_t, 1, 4 > _TyLogArray_1_4;
typedef LogArray< size_t, 2, 3 > _TyLogArray_2_3;
typedef LogArray< size_t, 4, 8 > _TyLogArray_4_8;
typedef LogArray< size_t, 4, 8 > _TyLogArray_3_3;

typedef Types< _TyLogArray_0_5, _TyLogArray_1_2, _TyLogArray_1_4, _TyLogArray_2_3, _TyLogArray_4_8, _TyLogArray_3_3 > vTyLogArrayTestTypes;

INSTANTIATE_TYPED_TEST_SUITE_P(LogArrayTestInstance,    // Instance name
                               TestLogArray,             // Test case name
                               vTyLogArrayTestTypes);  // Type list

int _TryMain( int argc, char **argv )
{
  return RUN_ALL_TESTS();
}
} // namespace ns_LogarrayTestDirect

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
    return ns_LogarrayTestDirect::_TryMain(argc, argv);
  }
  catch( const std::exception & rexc )
  {
    n_SysLog::Log( eslmtError, "%s: *** Exception: [%s]", g_strProgramName.c_str(), rexc.what() );
    fprintf( stderr, "%s: *** Exception: [%s]\n", g_strProgramName.c_str(), rexc.what() );      
    return -1;
  }
}

