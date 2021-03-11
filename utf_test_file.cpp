
// utf_test_file.cpp
// UTF test using unittesting files. Also should give a vague idea of relative perf.
// dbien
// 09MAR2021

#include "bienutil_test.h"

std::string g_strProgramName;

namespace ns_UtfTestFile
{
__BIENUTIL_USING_NAMESPACE

class UtfFileTestEnvironment : public ::testing::Environment
{
  typedef UtfFileTestEnvironment _TyThis;
  typedef ::testing::Environment _TyBase;
public:
  // Don't initialize the vector to zero because apparently I optimize my testing... lol.
  typedef std::vector< uint8_t, default_init_allocator< uint8_t > > _TyVectorBuf;

  explicit UtfFileTestEnvironment( const char * _pszFileName )
    : m_strFileName( _pszFileName )
  {
  }
protected:
  void SetUp() override 
  {
    // 0) Check if the caller used a slash that doesn't match the OS slash and substitute.
    // 1) Open the file.
    // 2) Figure out the encoding of the file from the file name, but figure out any endianness from the BOM.
    // 3) If the endianness of the file doesn't match the endianness of the machine then we switch when reading the file.
    // 4) Read the file into a buffer which is then utitilized by the unit tests.
    const char kcSlash = ( string::npos == m_strFileName.find( TChGetFileSeparator<char>() ) ) ? TChGetOtherFileSeparator<char>() : TChGetFileSeparator<char>();
    VerifyThrowSz( FFileExists( m_strFileName.c_str() ), "Original test file[%s] doesn't exist.", m_strFileName.c_str() );
    size_t nPosTxtExt = m_strFileName.rfind( ".txt" );
    VerifyThrowSz( string::npos != nPosTxtExt, "File doesn't have an '.txt' extention [%s].", m_strFileName.c_str() );
    // Don't rely on BOM being present - sus the encoding from the file name - fail if no encoding present.
    static constexpr char rgcUTFPrefix[] = "UTF-";
    size_t nPosUTFPrefix = m_strFileName.rfind( rgcUTFPrefix );
    VerifyThrowSz( string::npos != nPosUTFPrefix, "File doesn't have an 'UTF-' prefix indicating type [%s].", m_strFileName.c_str() );
    size_t nUTF = 0;
    int iResultReadNum = IReadPositiveNum( &m_strFileName[ nPosUTFPrefix + StaticStringLen( rgcUTFPrefix ) ], -1, nUTF, false );
    VerifyThrowSz( !iResultReadNum && ( ( 8 == nUTF ) || ( 16 == nUTF ) || ( 32 == nUTF ) ), "Couldn't find an '8', '16' or '32' following the 'UTF-' prefix." );
    // Now if 16 or 32 then we expect a BOM on the file so we don't need to otherwise check for endianness.
    // A UTF-8 file may or may not have a BOM.
    size_t nPosPrevSlash = m_strFileName.rfind( kcSlash );
    VerifyThrowSz( string::npos != nPosPrevSlash, "Need full path to input file[%s] - couldn't find preceding slash.", m_strFileName.c_str() );
    VerifyThrowSz( nPosTxtExt > nPosPrevSlash+1, "uh that's not an acceptable file name [%s].", m_strFileName.c_str() );
    string strBaseFile("unittests");
    strBaseFile += TChGetFileSeparator<char>();
    strBaseFile += m_strFileName.substr( nPosPrevSlash+1, nPosTxtExt - ( nPosPrevSlash+1 ) );
    FileObj fo( OpenReadOnlyFile( m_strFileName.c_str() ) );
    VerifyThrowSz( fo.FIsOpen(), "Couldn't open file [%s]", m_strFileName.c_str() );
    uint8_t rgbyBOM[vknBytesBOM];
    size_t nbyLenghtBOM;
    int iResult = FileRead( fo.HFileGet(), rgbyBOM, vknBytesBOM, &nbyLenghtBOM );
    Assert( !iResult );
    Assert( nbyLenghtBOM == vknBytesBOM );
    VerifyThrowSz( !iResult && ( nbyLenghtBOM == vknBytesBOM ), "Unable to read [%lu] bytes from the file[%s].", vknBytesBOM, m_strFileName.c_str() );
    EFileCharacterEncoding efceEncoding = GetCharacterEncodingFromBOM( rgbyBOM, nbyLenghtBOM );
    VerifyThrowSz( ( efceFileCharacterEncodingCount != efceEncoding ) || ( 8 == nUTF ), "For UTF-16 and UTF-32 we reqiure a BOM on the unittest files so that we can tell little from big endian[%s].", m_strFileName.c_str() );
    EFileCharacterEncoding efceEncodingThisMachine = ( efceFileCharacterEncodingCount == efceEncoding ) ? ( efceEncoding = efceUTF8 ) : GetEncodingThisMachine( efceEncoding );
    EFileCharacterEncoding efceEncodingFromFile = ( 8 == nUTF ) ? efceUTF8 : ( ( 16 == nUTF ) ? GetEncodingThisMachine( efceUTF16LE ) : GetEncodingThisMachine( efceUTF32LE ) );
    VerifyThrowSz( ( efceEncodingThisMachine == efceEncodingFromFile ), "Encoding from BOM doesn't match encoding within the filename[%s].", m_strFileName.c_str() );

    size_t nbySizeFile = GetFileSizeFromHandle( fo.HFileGet() ) - nbyLenghtBOM;
    VerifyThrowSz( !!nbySizeFile, "Must have a non-zero length file to unit test squat." );
    VerifyThrowSz( !( nbySizeFile % ( nUTF / CHAR_BIT ) ), "File isn't an integral multiple of character size[%s].", m_strFileName.c_str() );
    _TyVectorBuf bufRead;
    bufRead.resize( nbySizeFile );
    // Now seek to the end of any BOM:
    (void)NFileSeekAndThrow( fo.HFileGet(), nbyLenghtBOM, vkSeekBegin );
    size_t nbyBytesRead;
    iResult = FileRead( fo.HFileGet(), &bufRead[0], nbySizeFile, &nbyBytesRead );
    VerifyThrowSz( !iResult && ( nbySizeFile == nbyBytesRead ), "Error reading data from file [%s].", m_strFileName.c_str() );
    if ( efceEncodingThisMachine != efceEncoding )
    {
      Assert( ( nUTF == 16 ) || ( nUTF == 32 ) );
      // Then must cast to switch endian.
      if ( nUTF == 16 )
        SwitchEndian( reinterpret_cast< char16_t * >( &bufRead[0] ), nbySizeFile / sizeof( char16_t ) );
      else
        SwitchEndian( reinterpret_cast< char32_t * >( &bufRead[0] ), nbySizeFile / sizeof( char32_t ) );
    }
    m_buf = std::move( bufRead );
    m_strBaseFileOutput= std::move( strBaseFile );
    m_nUTFOrig = nUTF;
  }
  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
    // Nothing to do in TearDown() - we want to leave the generated unit test files so that they can be analyzed if there are any issues.
  }
public:
  pair< const void *, size_t > GetPrPvSize()
  {
    return pair< const void *, size_t >( &m_buf[0], m_buf.size() );
  }
  string m_strFileName;
  string m_strBaseFileOutput; // base output file.
  _TyVectorBuf m_buf;
  size_t m_nUTFOrig{0};
};

UtfFileTestEnvironment * vpufteUtfFileTestEnvironment{nullptr};

using ::testing::TestWithParam;
using ::testing::Bool;
using ::testing::Values;
using ::testing::Combine;

// TestUtfFile:
// First bool: We either are using ICU's conversion (false), or BIEN's conversion (true).
// Second bool: We are either converting the first possible conversion(false) or the last possible conversion(true).
class TestUtfFile : public ::testing::TestWithParam< std::tuple< bool, bool > >
{
  typedef TestUtfFile _TyThis;
  typedef ::testing::TestWithParam< std::tuple< bool > > _TyBase;
public:
protected:
  size_t m_nUtfConvTo;
  string m_strOutputFile;
  FileObj m_foOutputFile;
  bool m_fUseBIENConv{};
  bool m_fUseLastBit{};
  // Output will be in one of these:
  basic_string< char8_t > m_str8Converted;
  basic_string< char16_t > m_str16Converted;
  basic_string< char32_t > m_str32Converted;

  // SetUp() is run immediately before a test starts.
  void SetUp() override 
  {
    m_str8Converted.clear();
    m_str16Converted.clear();
    m_str32Converted.clear();
    std::tie(m_fUseBIENConv, m_fUseLastBit) = GetParam();
    // We will always write the output files in the endianness of the current machine.
    m_strOutputFile = vpufteUtfFileTestEnvironment->m_strBaseFileOutput;
    m_nUtfConvTo = m_fUseLastBit ? 32 : 8;
    if ( m_nUtfConvTo == vpufteUtfFileTestEnvironment->m_nUTFOrig )
      m_nUtfConvTo = 16;
    m_strOutputFile += "_to_UTF-";
    m_strOutputFile += to_string( m_nUtfConvTo );
    m_strOutputFile += m_fUseBIENConv ? "_BIEN.txt" : "_ICU.txt";
    FileObj foOut( CreateWriteOnlyFile( m_strOutputFile.c_str() ) );
    VerifyThrowSz( foOut.FIsOpen(), "Unable to open [%s] for writing.", m_strOutputFile.c_str() );
    m_foOutputFile.swap( foOut );
  }
  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
    Assert( !m_str8Converted.empty() + !m_str16Converted.empty() + !m_str32Converted.empty() == 1 );
    // We write the converted buffer to the opened file.
    void * pvWrite;
    size_t nbyWrite;
    if ( !m_str8Converted.empty() )
    {
      pvWrite = &m_str8Converted[0];
      nbyWrite = m_str8Converted.length();
    }
    else
    if ( !m_str16Converted.empty() )
    {
      pvWrite = &m_str16Converted[0];
      nbyWrite = m_str16Converted.length() * sizeof( char16_t );
    }
    else
    {
      pvWrite = &m_str32Converted[0];
      nbyWrite = m_str32Converted.length() * sizeof( char32_t );
    }
    VerifyThrowSz( !FileWrite( m_foOutputFile.HFileGet(), pvWrite, nbyWrite ), "Error writing to file [%s].", m_strOutputFile.c_str() );
  }
  void DoTest()
  {
    Assert( m_nUtfConvTo != vpufteUtfFileTestEnvironment->m_nUTFOrig );
    pair< const void *, size_t > prVpSz = vpufteUtfFileTestEnvironment->GetPrPvSize();
    if ( m_fUseBIENConv )
    {
      switch( m_nUtfConvTo )
      {
        case 8:
        {
          switch( vpufteUtfFileTestEnvironment->m_nUTFOrig )
          {
            case 16:
              ns_CONVBIEN::ConvertString( m_str8Converted, (const char16_t*)prVpSz.first, prVpSz.second );
            break;
            case 32:
              ns_CONVBIEN::ConvertString( m_str8Converted, (const char32_t*)prVpSz.first, prVpSz.second );
            break;
          }
        }
        break;
        case 16:
        {
          switch( vpufteUtfFileTestEnvironment->m_nUTFOrig )
          {
            case 8:
              ns_CONVBIEN::ConvertString( m_str16Converted, (const char8_t*)prVpSz.first, prVpSz.second );
            break;
            case 32:
              ns_CONVBIEN::ConvertString( m_str16Converted, (const char32_t*)prVpSz.first, prVpSz.second );
            break;
          }
        }
        break;
        case 32:
        {
          switch( vpufteUtfFileTestEnvironment->m_nUTFOrig )
          {
            case 8:
              ns_CONVBIEN::ConvertString( m_str32Converted, (const char8_t*)prVpSz.first, prVpSz.second );
            break;
            case 16:
              ns_CONVBIEN::ConvertString( m_str32Converted, (const char16_t*)prVpSz.first, prVpSz.second );
            break;
          }
        }
        break;
        default:
          VerifyThrow( false );
        break;
      }
    }
    else
    {
      switch( m_nUtfConvTo )
      {
        case 8:
        {
          switch( vpufteUtfFileTestEnvironment->m_nUTFOrig )
          {
            case 16:
              ns_CONVICU::ConvertString( m_str8Converted, (const char16_t*)prVpSz.first, prVpSz.second );
            break;
            case 32:
              ns_CONVICU::ConvertString( m_str8Converted, (const char32_t*)prVpSz.first, prVpSz.second );
            break;
          }
        }
        break;
        case 16:
        {
          switch( vpufteUtfFileTestEnvironment->m_nUTFOrig )
          {
            case 8:
              ns_CONVICU::ConvertString( m_str16Converted, (const char8_t*)prVpSz.first, prVpSz.second );
            break;
            case 32:
              ns_CONVICU::ConvertString( m_str16Converted, (const char32_t*)prVpSz.first, prVpSz.second );
            break;
          }
        }
        break;
        case 32:
        {
          switch( vpufteUtfFileTestEnvironment->m_nUTFOrig )
          {
            case 8:
              ns_CONVICU::ConvertString( m_str32Converted, (const char8_t*)prVpSz.first, prVpSz.second );
            break;
            case 16:
              ns_CONVICU::ConvertString( m_str32Converted, (const char16_t*)prVpSz.first, prVpSz.second );
            break;
          }
        }
        break;
        default:
          VerifyThrow( false );
        break;
      }
    }
  }
};
TEST_P( TestUtfFile, TestUtfFile ) 
{ 
  DoTest();
}
INSTANTIATE_TEST_SUITE_P( TestUtfFileSuite, TestUtfFile, Combine( Bool(), Bool() ) );

int _TryMain( int argc, char **argv )
{
  if ( argc > 1 ) // We may get called by cmake infrastructure gtest_discover_tests_impl - in that case we should just return RUN_ALL_TESTS().
  {
    //VerifyThrowSz( argc > 1, "Missing file name for unit test file." );
    // We should be running in the executable output directory and we should have a subdirectory created by the build already called "unittests".
    VerifyThrowSz( FDirectoryExists( "unittests" ), "We expect to find a directory called 'unittests' in the build directory." );
    // We expect a fully qualified path name for our file - but the test environment ends up verifying that.
    (void)testing::AddGlobalTestEnvironment( vpufteUtfFileTestEnvironment = new UtfFileTestEnvironment( argv[1] ) );
  }
  return RUN_ALL_TESTS();
}

} // namespace ns_UtfTestFile

int main( int argc, char **argv )
{
  __BIENUTIL_USING_NAMESPACE

  g_strProgramName = argv[0];
  testing::InitGoogleTest(&argc, argv);
  try
  {
    return ns_UtfTestFile::_TryMain(argc, argv);
  }
  catch( const std::exception & rexc )
  {
    n_SysLog::Log( eslmtError, "%s: *** Exception: [%s]", g_strProgramName.c_str(), rexc.what() );
    fprintf( stderr, "%s: *** Exception: [%s]\n", g_strProgramName.c_str(), rexc.what() );      
    return -1;
  }
}
