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
  typedef t_TyT _TyT;
  typedef typename _TyAllocator::value_type value_type;
  typedef typename _TyAllocator::size_type size_type;
  typedef typename _TyAllocator::difference_type difference_type;
  typedef typename _TyAllocator::propagate_on_container_move_assignment propagate_on_container_move_assignment;

  ~AllocatorThrowMove() = default;
  AllocatorThrowMove() = default;
  AllocatorThrowMove( AllocatorThrowMove const & ) = default;
  AllocatorThrowMove( AllocatorThrowMove && _r ) noexcept( false )
    : m_alloc( std::move( _r.m_alloc ) )
  {
  }
  constexpr _TyT* allocate( std::size_t n )
  {
    return m_alloc.allocate( n );
  }
  constexpr void deallocate( _TyT* p, std::size_t n )
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
  typedef t_TyT _TyT;
  typedef typename _TyAllocator::value_type value_type;
  typedef typename _TyAllocator::size_type size_type;
  typedef typename _TyAllocator::difference_type difference_type;
  typedef typename _TyAllocator::propagate_on_container_move_assignment propagate_on_container_move_assignment;

  ~AllocatorThrowDestruct() noexcept( false )
  {
  }
  AllocatorThrowDestruct() = default;
  AllocatorThrowDestruct( AllocatorThrowDestruct const & ) = default;
  AllocatorThrowDestruct( AllocatorThrowDestruct && _r ) = default;
  constexpr _TyT* allocate( std::size_t n )
  {
    return m_alloc.allocate( n );
  }
  constexpr void deallocate( _TyT* p, std::size_t n )
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
  typedef t_TyT _TyT;
  typedef typename _TyAllocator::value_type value_type;
  typedef typename _TyAllocator::size_type size_type;
  typedef typename _TyAllocator::difference_type difference_type;
  typedef typename _TyAllocator::propagate_on_container_move_assignment propagate_on_container_move_assignment;

  ~AllocatorThrowMoveDestruct() noexcept( false )
  {
  }
  AllocatorThrowMoveDestruct() = default;
  AllocatorThrowMoveDestruct( AllocatorThrowMoveDestruct const & ) = default;
  AllocatorThrowMoveDestruct( AllocatorThrowMoveDestruct && _r ) noexcept( false )
    : m_alloc( std::move( _r.m_alloc ) )
  {
  }
  constexpr _TyT* allocate( std::size_t n )
  {
    return m_alloc.allocate( n );
  }
  constexpr void deallocate( _TyT* p, std::size_t n )
  {
    return m_alloc.deallocate( p, n );
  }
protected:
  _TyAllocator m_alloc; // The actual allocator.
};

// TSharedObjNoThrowDtor: Destructor doesn't throw.
class TSharedObjNoThrowDtor
{
  typedef TSharedObjNoThrowDtor _TyThis;
public:
  TSharedObjNoThrowDtor() = default;
  ~TSharedObjNoThrowDtor()  = default;
  string m_str{"01345678900134567890013456789001345678900134567890"}; // make sure to blow out string's internal buffer.
};

// TSharedObjThrowingDtor: Destructor might throw.
class TSharedObjThrowingDtor
{
  typedef TSharedObjThrowingDtor _TyThis;
public:
  TSharedObjThrowingDtor() = default;
  ~TSharedObjThrowingDtor() noexcept( false ) = default;
  string m_str{"01345678900134567890013456789001345678900134567890"}; // make sure to blow out string's internal buffer.
};

// TestSharedWeakPointer:
// Test my new impl of UTF encoding conversions vs. ICU's open source library impl.
template < class t_PrTySharedElAllocator >
class TestSharedWeakPointer : public BienutilTest
{
  typedef TestSharedWeakPointer _TyThis;
  typedef BienutilTest _TyBase;
public:
  typedef typename t_PrTySharedElAllocator::first_type _TySharedEl;
  typedef typename t_PrTySharedElAllocator::second_type _TySharedElAllocator;

protected:
  // SetUp() is run immediately before a test starts.
  void SetUp() override 
  {
  }
  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
  }
  
  void DoTestSharedWeakPointer()
  {
    // Let's try some stuff:
    typedef SharedStrongPtr< _TySharedEl, _TySharedElAllocator > _TySharedStrongPtr1;
    typedef SharedWeakPtr< _TySharedEl, _TySharedElAllocator > _TySharedWeakPtr1;
    _TySharedStrongPtr1 sp1( std::in_place_t::in_place_t() );
    _TySharedWeakPtr1 wp1( sp1 );

    typedef SharedStrongPtr< const _TySharedEl, _TySharedElAllocator > _TyConstSharedStrongPtr1;
    typedef SharedWeakPtr< const _TySharedEl, _TySharedElAllocator > _TyConstSharedWeakPtr1;
    _TyConstSharedStrongPtr1 spc1( sp1 );
    spc1 = wp1;
    _TyConstSharedWeakPtr1 wpc1( spc1 );

    typedef SharedStrongPtr< const volatile _TySharedEl, _TySharedElAllocator > _TyConstVolatileSharedStrongPtr1;
    typedef SharedWeakPtr< const volatile _TySharedEl, _TySharedElAllocator > _TyConstVolatileSharedWeakPtr1;
    _TyConstVolatileSharedStrongPtr1 spcv1( spc1 );
    spcv1 = wpc1;
    _TyConstVolatileSharedWeakPtr1 wpcv1( sp1 );
    wpcv1 = wpc1;


  }
protected:
	bool m_fExpectFailure{false};
};

TYPED_TEST_SUITE_P( TestSharedWeakPointer );

TYPED_TEST_P( TestSharedWeakPointer, SharedWeakPtrTest1 )
{
  this->DoTestSharedWeakPointer();
}

REGISTER_TYPED_TEST_SUITE_P(
    TestSharedWeakPointer,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    SharedWeakPtrTest1);

typedef std::pair< TSharedObjNoThrowDtor, allocator< char > > _TyPrTest01;
typedef std::pair< TSharedObjThrowingDtor, allocator< char > > _TyPrTest02;
typedef std::pair< TSharedObjNoThrowDtor, AllocatorThrowMove< char > > _TyPrTest03;
typedef std::pair< TSharedObjThrowingDtor, AllocatorThrowMove< char > > _TyPrTest04;

typedef Types<_TyPrTest01, _TyPrTest02, _TyPrTest03, _TyPrTest04 > vTySharedWeakPtrTestTypes;

INSTANTIATE_TYPED_TEST_SUITE_P(SharedWeakPtrTestInstance,    // Instance name
                               TestSharedWeakPointer,             // Test case name
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

