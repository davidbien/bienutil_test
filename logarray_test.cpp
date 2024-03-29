﻿
//          Copyright David Lawrence Bien 1997 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt).

// logarray_test.cpp : Direct test new LogArray impl.
// dbien
// 01APR2021

#include "bienutil_test.h"
#include "_logarray.h"
#include "_util.h"
#include <_simpbv.h>
#include <random>

std::string g_strProgramName;

namespace ns_LogarrayTestDirect
{
using namespace ns_bienutil_test;
__BIENUTIL_USING_NAMESPACE

BienutilTestEnvironment * vpxteBienutilTestEnvironment{nullptr};

using ::testing::Types;

// Type that stores a long but allocates it in dynamic memory.
// Makes sure that we delete all contained objects in a LogArray<>.
template < class t_TyTScalar >
class ScalarHolder
{
  typedef ScalarHolder _TyThis;
public:
  typedef t_TyTScalar _TyTScalar;

  ScalarHolder()
    : m_upScalar( make_unique< _TyTScalar >( _TyTScalar{} ) )
  {
  }
  ~ScalarHolder() = default;
  ScalarHolder( ScalarHolder const & _r )
    : m_upScalar( make_unique< _TyTScalar >( _r ) )
  {
  }
  ScalarHolder & operator =( ScalarHolder const & ) = default;
  ScalarHolder( ScalarHolder && ) = default;
  ScalarHolder & operator =( ScalarHolder && ) = default;
  ScalarHolder( _TyTScalar _t )
    : m_upScalar( make_unique< _TyTScalar >( _t ) )
  {
  }
  ScalarHolder & operator =( t_TyTScalar const & _rt )
  {
    *m_upScalar = _rt;
    return *this;
  }
  operator _TyTScalar () const
  {
    return *m_upScalar;
  }
  _TyTScalar operator ++( int )
  {
    return (*m_upScalar)++;
  }
protected:
  unique_ptr< _TyTScalar > m_upScalar;
};

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
  
  static const size_t s_knCountTestFixedBlocks = 13;
    // This defines the starting point size of each test on a given type.
  static const size_t s_knMaxCountTestElements = _TyLogArray::s_knElementsFixedBoundary + s_knCountTestFixedBlocks * ( 1ull << _TyLogArray::s_knPow2Max );
    // We will test all sizes equal to or less than this.
  void DoTestLogArray()
  {
#if 0
    _TyLogArray la( 2 );
    _TyT tCur{};
    la.ApplyContiguous( 0, la.GetSize(),
      [&tCur]( _TyT * _ptBegin, _TyT * _ptEnd )
      {
        std::generate_n( _ptBegin, _ptEnd - _ptBegin, 
          [&tCur]() { return tCur++; }
        );
      }
    );
    la._PvAllocEnd();
    //la.SetSize( 1 );
    la.Clear();
    // _TestSizedLogArray( la, true );
#else //0
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
#endif //0
  }
  // Since we are using size_t we similate the unconstructed element at the end, but the effect is exactly the same...
  void _TestSizedLogArray( _TyLogArray & _la, bool _fWithUnconstructedElementAtEnd )
  {
    _VerifyLogArray( _la );
    
    // Due to horrendous boundary conditions we want to exhaustively test removal from end scenarios:
    size_t nNewElements = _la.NElements();
    for ( size_t nNewElementsCur = _la.NElements(); nNewElementsCur != size_t(-1); --nNewElementsCur )
    {
      _TyLogArray laCopy( _la );
      if ( _fWithUnconstructedElementAtEnd )
      {
        laCopy._PvAllocEnd(); // This is a safe call because it doesn't assume anything is constructed.
        Assert( laCopy.NElements() + 1 == laCopy.NElementsAllocated() );
      }
      else
        Assert( laCopy.NElements() == laCopy.NElementsAllocated() );
      _VerifyLogArray( laCopy );
      laCopy.SetSize( nNewElementsCur );
      _VerifyLogArray( laCopy );
      // _VerifyLogArrayContigRange( laCopy ); - later.
      // Now reset the size to the original to test endpoints there:
      laCopy.SetSize( _la.GetSize() * 2 ); // Size it up to double the original for fun.
      size_t nMarkNewElements = nNewElementsCur;
      laCopy.ApplyContiguous( nNewElementsCur, laCopy.GetSize(), 
        [&nMarkNewElements]( _TyT * _ptBegin, _TyT * _ptEnd )
        {
          for ( _TyT * ptCur = _ptBegin; _ptEnd != ptCur; )
            *ptCur++ = nMarkNewElements++;
        }
      );
      _VerifyLogArray( laCopy );
    }
  }
  void DoTestLogArrayRemoval()
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
      
      _TestSizedLogArrayRemoval( la, true );
      _TestSizedLogArrayRemoval( la, false );
    }
  }
  void _TestSizedLogArrayRemoval( _TyLogArray & _la, bool _fWithUnconstructedElementAtEnd )
  {
    _VerifyLogArray( _la );
    // Up until 17 elements we check every potential permutation of removals.
    // After that it gets too slow to complete the unit test in any timely manner (like days at a certain point) so we
    //  produce random permuations of removals based upon first computing a percentage and then populating that percentage of
    //  removals in the bitvector randomly.
    static size_t kstCheckAllPermsLimit = 17;
    typedef _simple_bitvec< size_t > _TyBv;
    _TyBv bvRemove( _la.NElements() );
    if ( _la.NElements() <= kstCheckAllPermsLimit )
    {
      // Try to remove each potential bitvector of elements by incrementing the bitvector until it wraps around to all empty again.
      do
      {
        _TyLogArray laCopy(_la);
        if (_fWithUnconstructedElementAtEnd)
        {
          laCopy._PvAllocEnd(); // This is a safe call because it doesn't assume anything is constructed.
          Assert(laCopy.NElements() + 1 == laCopy.NElementsAllocated());
        }
        else
          Assert(laCopy.NElements() == laCopy.NElementsAllocated());
        _VerifyLogArray( laCopy );
        
        laCopy.RemoveBvElements( bvRemove );
        _VerifyLogArray( laCopy, bvRemove );
        bvRemove.increment();
      } 
      while( !bvRemove.empty() );
    }
    else
    {
      static const size_t kstNCheckPermutations = 100; // Check a hundred random permutations of removals.
      for ( size_t nCheck = kstNCheckPermutations; nCheck--;  )
      {
        size_t nPercentRemoved = vpxteBienutilTestEnvironment->GetRandomRanged( 1, 100 );
        for ( size_t nBitPop = 0; nBitPop < bvRemove.size(); ++nBitPop )
          bvRemove.setbit( nBitPop, vpxteBienutilTestEnvironment->FRandomPercentage( nPercentRemoved ) );
        _TyLogArray laCopy(_la);
        if (_fWithUnconstructedElementAtEnd)
        {
          laCopy._PvAllocEnd(); // This is a safe call because it doesn't assume anything is constructed.
          Assert(laCopy.NElements() + 1 == laCopy.NElementsAllocated());
        }
        else
          Assert(laCopy.NElements() == laCopy.NElementsAllocated());
        _VerifyLogArray( laCopy );
        laCopy.RemoveBvElements( bvRemove );
        _VerifyLogArray( laCopy, bvRemove );
      }
    }
  }
  void _VerifyLogArray( _TyLogArray const & _la )
  {
    // This moves through each element using the ElGet() method:
    _TyT nElValueCur = 0;
    size_t nElements = _la.NElements();
    for ( size_t nElCur = 0; nElCur < nElements; ++nElCur )
    {
      Assert( nElValueCur == _la.ElGet( nElCur ) );
      VerifyThrow( nElValueCur++ == _la.ElGet( nElCur ) );
    }
  }
  template < class t_TyBv >
  void _VerifyLogArray( _TyLogArray const & _la, t_TyBv const & _rbv )
  {
    // This moves through each element using the ElGet() method:
    _TyT nElValueCur = 0;
    size_t nElementsOrig = _rbv.size();
    Assert( _la.NElements() == ( nElementsOrig - _rbv.countsetbits() ) );
    VerifyThrow( _la.NElements() == ( nElementsOrig - _rbv.countsetbits() ) );
    size_t nElArrayCur = 0;
    for ( size_t nElBvCur = 0; nElBvCur < nElementsOrig; ++nElBvCur )
    {
      _TyT nValueOrig = nElValueCur++;
      if ( _rbv.isbitset( nElBvCur ) )
        continue;
      Assert( nValueOrig == _la.ElGet( nElArrayCur ) );
      VerifyThrow( nValueOrig == _la.ElGet( nElArrayCur ) );
      ++nElArrayCur;
    }
  }
  void _VerifyLogArrayContigRange( _TyLogArray const & _la )
  {
    // So we peruse every potential view of the log array and some erroneous ones and verify each:
    for ( size_t nViewStart = 0; nViewStart < _la.NElements() + 10; ++nViewStart )
    {
      for ( size_t nViewEnd = 0; nViewEnd < _la.NElements() + 10; ++nViewEnd )
      {
        bool fExpectFailure = ( nViewStart > nViewEnd ) || ( nViewEnd > _la.NElements() );
        try
        {
          _TyT nCurEl = nViewStart;
          _la.ApplyContiguous( nViewStart, nViewEnd,
            [&nCurEl]( const _TyT * _ptBegin, const _TyT * _ptEnd )
            {
              for ( const _TyT * ptCur = _ptBegin; _ptEnd != ptCur; ++ptCur )
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
  this->DoTestLogArray();
}
TYPED_TEST_P( TestLogArray, LogArrayTestRemoval )
{
  this->DoTestLogArrayRemoval();
}

REGISTER_TYPED_TEST_SUITE_P(
    TestLogArray,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    LogArrayTest1,
    LogArrayTestRemoval);

typedef LogArray< ScalarHolder< size_t >, 0, 3 > _TyLogArray_0_3;
typedef LogArray< ScalarHolder< size_t >, 1, 2 > _TyLogArray_1_2;
typedef LogArray< ScalarHolder< size_t >, 1, 4 > _TyLogArray_1_4;
typedef LogArray< ScalarHolder< size_t >, 2, 3 > _TyLogArray_2_3;
typedef LogArray< ScalarHolder< size_t >, 3, 3 > _TyLogArray_3_3;

typedef Types< _TyLogArray_1_2, _TyLogArray_3_3, _TyLogArray_1_4, _TyLogArray_2_3, _TyLogArray_0_3 > vTyLogArrayTestTypes;

INSTANTIATE_TYPED_TEST_SUITE_P(LogArrayTestInstance,    // Instance name
                               TestLogArray,             // Test case name
                               vTyLogArrayTestTypes);  // Type list

int _TryMain( int argc, char **argv )
{
  uint32_t u32Seed{0};
  if ( argc > 1 ) // We may get called with a random seed which can be used to repeat tests that found issues.
    VerifyThrowSz( 1 == sscanf( argv[1], "%u", &u32Seed ), "Unable to get a random seed uin32 from [%s]", argv[1] );
  (void)testing::AddGlobalTestEnvironment( vpxteBienutilTestEnvironment = new BienutilTestEnvironment( u32Seed ) );
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

