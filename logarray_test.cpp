// logarray_test.cpp : Direct test new LogArray impl.
// dbien
// 01APR2021

#include "bienutil_test.h"
#include "_logarray.h"
#include "_util.h"
#include <random>

std::string g_strProgramName;

namespace ns_LogarrayTestDirect
{
using namespace ns_bienutil_test;
__BIENUTIL_USING_NAMESPACE

BienutilTestEnvironment * vpxteBienutilTestEnvironment{nullptr};

__XMLP_USING_NAMESPACE

class XmlpTestEnvironment : public testing::Environment
{
  typedef XmlpTestEnvironment _TyThis;
  typedef testing::Environment _TyBase;
public:
  typedef std::pair< EFileCharacterEncoding, bool > _TyKeyEncodingBOM;
  typedef map< _TyKeyEncodingBOM, string > _TyMapTestFiles;
  explicit XmlpTestEnvironment( const char * _pszFileName )
    : m_strFileNameOrig( _pszFileName )
  {
  }
protected:
  void SetUp() override 
  {
    // 0) Check if the caller used a slash that doesn't match the OS slash and substitute.
    // 1) Open the file.
    // 2) Figure out the encoding of the file. Also check if it is a failure scenario.
    // 3) Create all potential encodings of the file - i.e. UTF32, UTF16, and UTF8 big and little endian.
    //  a) Write each encoding with and without a BOM.
    // 4) Write all encodings in a unittest directory in the output.
    // 5) List all file names in this environment object in m_rgFileNamesTestDir.
    using namespace filesystem;
    path pathFileOrig( m_strFileNameOrig );
    VerifyThrowSz( exists( pathFileOrig ), "Original test file[%s] doesn't exist.", m_strFileNameOrig.c_str() );
    VerifyThrowSz( pathFileOrig.has_parent_path(), "Need full path to input file[%s] - couldn't find parent path.", m_strFileNameOrig.c_str() );
    VerifyThrowSz( pathFileOrig.has_extension() && ( pathFileOrig.extension() == ".xml" ), "File doesn't have an '.xml' extention [%s].", m_strFileNameOrig.c_str() );
    VerifyThrowSz( pathFileOrig.has_stem(), "No base filename in[%s].", m_strFileNameOrig.c_str() );
    path pathBaseFile( "unittests" );
    m_pathStemOrig = pathFileOrig.stem();
    pathBaseFile /= m_pathStemOrig;
    FileObj fo( OpenReadOnlyFile( m_strFileNameOrig.c_str() ) );
    VerifyThrowSz( fo.FIsOpen(), "Couldn't open file [%s]", m_strFileNameOrig.c_str() );
    uint8_t rgbyBOM[vknBytesBOM];
    size_t nbyLenghtBOM;
    int iResult = FileRead( fo.HFileGet(), rgbyBOM, vknBytesBOM, &nbyLenghtBOM );
    Assert( !iResult );
    Assert( nbyLenghtBOM == vknBytesBOM ); // The smallest valid xml file is 4 bytes... "<t/>".
    VerifyThrowSz( nbyLenghtBOM == vknBytesBOM, "Unable to read [%lu] bytes from the file[%s].", vknBytesBOM, m_strFileNameOrig.c_str() );
    m_fExpectFailure = ( string::npos != m_strFileNameOrig.find("FAIL") ); // simple method for detecting expected failures.
    EFileCharacterEncoding efceEncoding = efceFileCharacterEncodingCount;
    if ( !iResult && ( nbyLenghtBOM == vknBytesBOM ) )
      efceEncoding = GetCharacterEncodingFromBOM( rgbyBOM, nbyLenghtBOM );
    bool fEncodingFromBOM = ( efceFileCharacterEncodingCount != efceEncoding );
    if ( !fEncodingFromBOM )
    {
      // Since we know we are opening XML we can use an easy heuristic to determine the encoding:
      efceEncoding = DetectEncodingXmlFile( rgbyBOM, vknBytesBOM );
      Assert( efceFileCharacterEncodingCount != efceEncoding ); // Unless the source isn't XML the above should succeed.
      VerifyThrowSz( efceFileCharacterEncodingCount != efceEncoding, "Unable to sus encoding for the file[%s].", m_strFileNameOrig.c_str() );
      nbyLenghtBOM = 0;
    }
    
    size_t grfNeedFiles = ( 1 << ( 2 * efceFileCharacterEncodingCount ) ) - 1;
    m_grfFileOrig = ( ( 1 << efceEncoding ) << ( size_t(fEncodingFromBOM) * efceFileCharacterEncodingCount ) );
    grfNeedFiles &= ~m_grfFileOrig;
    while( grfNeedFiles )
    {
      // This isn't as efficient as it could be but it feng shuis...
      size_t stGenerate = _bv_get_clear_first_set( grfNeedFiles );
      bool fWithBOM = stGenerate >= efceFileCharacterEncodingCount;
      EFileCharacterEncoding efceGenerate = EFileCharacterEncoding( stGenerate % efceFileCharacterEncodingCount );
      // Name the new file with the base of the original XML file:
      path pathConvertedFile( pathBaseFile );
      pathConvertedFile += "_";
      pathConvertedFile += PszCharacterEncodingShort( efceGenerate );
      if ( fWithBOM )
        pathConvertedFile += "BOM";
      pathConvertedFile += ".xml";
      (void)NFileSeekAndThrow( fo.HFileGet(), nbyLenghtBOM, vkSeekBegin );
      ConvertFileMapped( fo.HFileGet(), efceEncoding, pathConvertedFile.string().c_str(), efceGenerate, fWithBOM );
      VerifyThrow( m_mapFileNamesTestDir.insert( _TyMapTestFiles::value_type( _TyKeyEncodingBOM( efceGenerate, fWithBOM ), pathConvertedFile.string() ) ).second );
    }
    // Now copy the original to the output directory - don't modify the name so we know it's the original.
    path pathConvertedFile( pathBaseFile );
    pathConvertedFile += ".xml";
    ConvertFileMapped( fo.HFileGet(), efceEncoding, pathConvertedFile.string().c_str(), efceEncoding, fEncodingFromBOM );
    VerifyThrow( m_mapFileNamesTestDir.insert( _TyMapTestFiles::value_type( _TyKeyEncodingBOM( efceEncoding, fEncodingFromBOM ), pathConvertedFile.string() ) ).second );

    // Create a directory for the base file - sans extension - this is where the output files for the given unit test will go.
    std::error_code ec;
    (void)create_directory( pathBaseFile, ec );
    VerifyThrowSz( !ec, "Unable to create unittest output directory [%s] ec.message()[%s].", pathBaseFile.string().c_str(), ec.message().c_str() );
    m_pathBaseFile = std::move( pathBaseFile );
  }

  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
    // Nothing to do in TearDown() - we want to leave the generated unit test files so that they can be analyzed if there are any issues.
  }
public:
  filesystem::path PathCreateUnitTestOutputDirectory() const
  {
    using namespace filesystem;
    const ::testing::TestInfo* const test_info =
    ::testing::UnitTest::GetInstance()->current_test_info();
    path pathSuite = m_pathBaseFile;
    pathSuite /= path( g_strProgramName ).stem(); // since we have multiple unit test files now.
    pathSuite /= test_info->test_suite_name();
    path pathTest = test_info->name();
    path::iterator itTestNum = --pathTest.end();
    pathSuite += "_";
    pathSuite += itTestNum->c_str();
    std::error_code ec;
    (void)create_directories( pathSuite, ec );
    VerifyThrowSz( !ec, "Unable to create unittest output directory[%s] ec.message()[%s].", pathSuite.string().c_str(), ec.message().c_str() );
    return pathSuite;
  }
  // Return the filename for the output file for the given bit.
  filesystem::path GetNextFileNameOutput( filesystem::path const & _rpathOutputDir, size_t & _rgrfBitOutput, const _TyMapTestFiles::value_type *& _rpvtGoldenFile ) const
  {
    filesystem::path pathOutputFile( _rpathOutputDir );
    size_t stGenerate = _bv_get_clear_first_set( _rgrfBitOutput );
    bool fWithBOM = stGenerate >= efceFileCharacterEncodingCount;
    EFileCharacterEncoding efceEncoding = EFileCharacterEncoding( stGenerate % efceFileCharacterEncodingCount );
    _TyMapTestFiles::const_iterator citFile = m_mapFileNamesTestDir.find( _TyKeyEncodingBOM(efceEncoding, fWithBOM ) );
    _rpvtGoldenFile = &*citFile;
    VerifyThrow( m_mapFileNamesTestDir.end() != citFile ); // unexpected.
    pathOutputFile /= m_pathStemOrig;
    if ( ( 1ull << stGenerate ) != m_grfFileOrig )
    {
      pathOutputFile += "_";
      pathOutputFile += PszCharacterEncodingShort( efceEncoding );
      if ( fWithBOM )
        pathOutputFile += "BOM";
    }
    pathOutputFile += ".xml";
    return pathOutputFile;
  }
  void CompareFiles( filesystem::path const & _rpathOutputFile, const _TyMapTestFiles::value_type * _pvtGoldenFile )
  {
    // Map both files and compare the memory - should match byte for byte. If unit tests have extra whitespace in markup (i.e. between attribute declarations)
    //  then the files may not match. It's important to structure the unit tests so that they don't have extra markup whitespace. All non-markup whitespace
    //  should be duplicated correctly - i.e CHARDATA in between tags and other elements.
    size_t nbyOutput, nbyGolden;
    FileMappingObj fmoOutput( _FmoOpen( _rpathOutputFile, nbyOutput ) );
    FileMappingObj fmoGolden( _FmoOpen( _pvtGoldenFile->second, nbyGolden ) );
    // Move through even if the files don't match in size to find the first byte where they don't match for diag purposes.
    // Can't map zero size files so we know we don't have any...
    const uint8_t * pbyOutputCur = fmoOutput.Pby();
    const uint8_t * const pbyOutputEnd = pbyOutputCur + nbyOutput;
    const uint8_t * pbyGoldenCur = fmoGolden.Pby();
    const uint8_t * const pbyGoldenEnd = pbyGoldenCur + nbyGolden;
    for ( ; ( pbyOutputEnd != pbyOutputCur ) && ( pbyGoldenEnd != pbyGoldenCur ); ++pbyOutputCur, ++pbyGoldenCur )
    {
      Assert( *pbyOutputCur == *pbyGoldenCur );
      VerifyThrowSz( *pbyOutputCur == *pbyGoldenCur, "Mismatch at byte number [%lu] outputfile[%s] goldenfile[%s].", ( pbyOutputCur - fmoOutput.Pby() ), 
         _rpathOutputFile.string().c_str(), _pvtGoldenFile->second.c_str() );
    }
  }
  FileMappingObj _FmoOpen( filesystem::path const & _rpathFile, size_t & _rnbySize )
  {
    FileObj fo( OpenReadOnlyFile( _rpathFile.string().c_str() ) );
    VerifyThrowSz( fo.FIsOpen(), "Unable to open file [%s].", _rpathFile.string().c_str() );
    FileMappingObj fmo( MapReadOnlyHandle( fo.HFileGet(), &_rnbySize ) );
    VerifyThrowSz( fmo.FIsOpen(), "Couldn't map file [%s]", _rpathFile.string().c_str() );
    return fmo;
  }
   
};

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

typedef LogArray< ScalarHolder< size_t >, 0, 5 > _TyLogArray_0_5;
typedef LogArray< ScalarHolder< size_t >, 1, 2 > _TyLogArray_1_2;
typedef LogArray< ScalarHolder< size_t >, 1, 4 > _TyLogArray_1_4;
typedef LogArray< ScalarHolder< size_t >, 2, 3 > _TyLogArray_2_3;
typedef LogArray< ScalarHolder< size_t >, 3, 3 > _TyLogArray_3_3;

typedef Types< _TyLogArray_1_2, _TyLogArray_3_3, _TyLogArray_1_4, _TyLogArray_2_3/*, _TyLogArray_0_5*/ > vTyLogArrayTestTypes;

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

