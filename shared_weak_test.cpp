// shared_weak_test.cpp : Direct test new Shared/Weak Pointer impl.
// dbien
// 22JUL2021

#include "bienutil_test.h"
#include "_shwkptr.h"
#include <utility>
#include "_util.h"

std::string g_strProgramName;

namespace ns_SharedWeakPtrTestDirect
{
using namespace ns_bienutil_test;
__BIENUTIL_USING_NAMESPACE

BienutilTestEnvironment * vpxteBienutilTestEnvironment{nullptr};

using ::testing::Types;

// Create different allocators that might throw in various scenarios to test various scenarios:
template < class t_TyT >
class AllocatorThrowMove
{
  typedef AllocatorThrowMove _TyThis;
  typedef allocator< t_TyT > _TyAllocator; // the actual allocator.
public:
  typedef typename _TyAllocator::value_type value_type;
  typedef typename _TyAllocator::size_type size_type;
  typedef typename _TyAllocator::difference_type difference_type;
  typedef typename _TyAllocator::propagate_on_container_move_assignment propagate_on_container_move_assignment;

  ~AllocatorThrowMove() = default;
  AllocatorThrowDestruct( AllocatorThrowMove const & ) = default;
  AllocatorThrowMove( AllocatorThrowMove && _r ) noexcept( false )
    : m_alloc( std::move( _r.m_alloc ) )
  {
  }
  [[nodiscard]] constexpr T* allocate( std::size_t n )
  {
    return m_alloc.allocate( n );
  }
  constexpr void deallocate( T* p, std::size_t n )
  {
    return m_alloc.deallocate( p, n );
  }
protected:
  _TyAllocator m_alloc; // The actual allocator.
};
template < class t_TyT >
class AllocatorThrowDestruct
{
  typedef AllocatorThrowDestruct _TyThis;
  typedef allocator< t_TyT > _TyAllocator; // the actual allocator.
public:
  typedef typename _TyAllocator::value_type value_type;
  typedef typename _TyAllocator::size_type size_type;
  typedef typename _TyAllocator::difference_type difference_type;
  typedef typename _TyAllocator::propagate_on_container_move_assignment propagate_on_container_move_assignment;

  ~AllocatorThrowDestruct() noexcept( false )
  {
  }
  AllocatorThrowDestruct( AllocatorThrowDestruct const & ) = default;
  AllocatorThrowDestruct( AllocatorThrowDestruct && _r ) = default;
  [[nodiscard]] constexpr T* allocate( std::size_t n )
  {
    return m_alloc.allocate( n );
  }
  constexpr void deallocate( T* p, std::size_t n )
  {
    return m_alloc.deallocate( p, n );
  }
protected:
  _TyAllocator m_alloc; // The actual allocator.
};
template < class t_TyT >
class AllocatorThrowMoveDestruct
{
  typedef AllocatorThrowMoveDestruct _TyThis;
  typedef allocator< t_TyT > _TyAllocator; // the actual allocator.
public:
  typedef typename _TyAllocator::value_type value_type;
  typedef typename _TyAllocator::size_type size_type;
  typedef typename _TyAllocator::difference_type difference_type;
  typedef typename _TyAllocator::propagate_on_container_move_assignment propagate_on_container_move_assignment;

  ~AllocatorThrowMoveDestruct() noexcept( false )
  {
  }
  AllocatorThrowMoveDestruct( AllocatorThrowMoveDestruct const & ) = default;
  AllocatorThrowMoveDestruct( AllocatorThrowMoveDestruct && _r ) noexcept( false )
    : m_alloc( std::move( _r.m_alloc ) )
  {
  }
  [[nodiscard]] constexpr T* allocate( std::size_t n )
  {
    return m_alloc.allocate( n );
  }
  constexpr void deallocate( T* p, std::size_t n )
  {
    return m_alloc.deallocate( p, n );
  }
protected:
  _TyAllocator m_alloc; // The actual allocator.
};

// TSharedObj1: Destructor doesn't throw.
class TSharedObj1
{
  typedef TSharedObj1 _TyThis;
  typedef SharedObjectBase< true > _TyBase;
public:
  TSharedObj1() = default;
  ~TSharedObj1()  = default;
  string m_str{"01345678900134567890013456789001345678900134567890"}; // make sure to blow out string's internal buffer.
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
    SharedWeakPtr< _TySharedEl > spse;
    spse.emplace();
    SharedWeakPtr< const _TySharedEl > spcse;
    spcse.emplace();
    spcse = spse;
    SharedWeakPtr< volatile _TySharedEl > spvse;
    spvse.emplace();
    spvse = spse;
    SharedWeakPtr< const volatile _TySharedEl > spcvse( spcse );
    spcvse = spcse;
    spcvse = spvse;
    spcvse = spse;
    // To test compiling these just uncomment them one by one and compile the unit test... I know, annoying.
    // spse = spcvse; // this should fail to compile.
    // spse = spcse; // this should fail to compile.
    // spcse = spcvse; // this should fail to compile.

    SharedWeakPtr< _TySharedElDerived > spseDerived;
    spse = spseDerived;
    spcse = spseDerived;
    spvse = spseDerived;
    spcvse = spseDerived;
    
    // spseDerived = spse; // this should fail to compile.
    spse.template emplaceDerived< _TySharedElDerived >();
    // This should fail to compile because we are creating a const _TySharedElDerived into a non-const container.
    // spse.template emplaceDerived< const _TySharedElDerived >();

    // in-place construct a derived object:
    SharedWeakPtr< const _TySharedEl > spcseDerived( in_place_type< _TySharedElDerived > );
  }
protected:
	bool m_fExpectFailure{false};
};

TYPED_TEST_SUITE_P( TestSharedObj );

TYPED_TEST_P( TestSharedObj, SharedWeakPtrTest1 )
{
  this->DoTestSharedObj();
}

REGISTER_TYPED_TEST_SUITE_P(
    TestSharedObj,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    SharedWeakPtrTest1);

typedef std::pair< TSharedObj1, TSharedObj1 > _TyPrTest01;
typedef std::pair< TSharedObj1, TDerivedObj1 > _TyPrTest02;

typedef Types<_TyPrTest01, _TyPrTest02 > vTySharedWeakPtrTestTypes;

INSTANTIATE_TYPED_TEST_SUITE_P(SharedWeakPtrTestInstance,    // Instance name
                               TestSharedObj,             // Test case name
                               vTySharedWeakPtrTestTypes);  // Type list

int _TryMain( int argc, char **argv )
{
  return RUN_ALL_TESTS();
}
} // namespace ns_SharedWeakPtrTestDirect

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
    return ns_SharedWeakPtrTestDirect::_TryMain(argc, argv);
  }
  catch( const std::exception & rexc )
  {
    n_SysLog::Log( eslmtError, "%s: *** Exception: [%s]", g_strProgramName.c_str(), rexc.what() );
    fprintf( stderr, "%s: *** Exception: [%s]\n", g_strProgramName.c_str(), rexc.what() );      
    return -1;
  }
}

