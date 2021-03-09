// bienutil_test.cpp : Defines the entry point for the application.
//

#include "bienutil_test.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;
	return 0;
}

std::string g_strProgramName;

namespace vs_bienutil_test
{
__XMLP_USING_NAMESPACE

class BienutilTestEnvironment : public testing::Environment
{
  typedef BienutilTestEnvironment _TyThis;
  typedef ::testing::Environment _TyBase;
public:
  explicit BienutilTestEnvironment()
  {
  }
protected:
  void SetUp() override 
	{
	}
  

  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
    // Nothing to do in TearDown() - we want to leave the generated unit test files so that they can be analyzed if there are any issues.
  }
public:
};

BienutilTestEnvironment * vpxteBienutilTestEnvironment{nullptr};

using ::testing::TestWithParam;
using ::testing::Bool;
using ::testing::Values;
using ::testing::Combine;

// TestUtfConversions:
// Test my new impl of UTF encoding conversions vs. ICU's open source library impl.
class TestUtfConversions : public BienutilTest
{
  typedef TestUtfConversions _TyThis;
  typedef BienutilTest _TyBase;
public:
protected:
  // SetUp() is run immediately before a test starts.
  void SetUp() override 
  {
  }
  // TearDown() is invoked immediately after a test finishes.
  void TearDown() override 
  {
  }

// Test all character types.
	typedef basic_string< char > _TyStrChar;
	typedef basic_string< wchar_t > _TyStrWChar;
	typedef basic_string< char32_t > _TyStrChar32;
	typedef basic_string< char16_t > _TyStrChar16;
	typedef basic_string< char8_t > _TyStrChar8;

	void TestUtf32CodePoint( char32_t _utf32 )
	{
		// We generally never expect failure. A throw from a test should indicate failure every time.
		try
		{
			_TryTestUtf32CodePoint( _utf32 );
			VerifyThrowSz( !m_fExpectFailure, "Expected failure but succeeded." );
		}
		catch( std::exception const & )
		{
			if ( !m_fExpectFailure )
				throw; // Then a real failure - the infrastructure will catch this.
		}
	}
	template < class t_TyChar >
	void _ConvICU( char32_t _utf32, basic_string< t_TyChar > & _rstr )
	{
		ns_CONVICU::ConvertString( _rstr, &_utf32, 1 );
	}
	template < class t_TyChar >
	void _ConvBIEN( char32_t _utf32, basic_string< t_TyChar > & _rstr, char16_t _utf16ReplacementChar = vkutf16ReplacementCharDefault )
	{
		ns_CONVBIEN::ConvertString( _rstr, &_utf32, 1, _utf16ReplacementChar );
	}
	template < class t_TyChar >
	void _ConvBackICU( const basic_string< t_TyChar > & _rstr, char32_t _utf32 )
	{
		basic_string< char32_t > str32;
		ns_CONVICU::ConvertString( str32, &_rstr[0], _rstr.length() );
		VerifyThrowSz( str32[0] == _utf32, "Conversion back failed for ICU." );
	}
	template < class t_TyChar >
	void _ConvBackBIEN( const basic_string< t_TyChar > & _rstr, char32_t _utf32 )
	{
		basic_string< char32_t > str32;
		ns_CONVBIEN::ConvertString( str32, &_rstr[0], _rstr.length() );
		VerifyThrowSz( str32[0] == _utf32, "Conversion back failed for BIEN." );
	}
	template < class t_TyCharLeft, class t_TyCharRight >
	void _ConvExpectICU( basic_string< t_TyCharLeft > const & _rstrLeft, basic_string< t_TyCharRight > const & _rstrRight )
	{
		basic_string< t_TyCharLeft > rstrLeftResult;
		ns_CONVICU::ConvertString( rstrLeftResult, &_rstrRight[0], _rstrRight.length() );
		VerifyThrowSz( rstrLeftResult == _rstrLeft, "Left and right didn't match for ICU." )
	}
	template < class t_TyCharLeft, class t_TyCharRight >
	void _ConvExpectBIEN( basic_string< t_TyCharLeft > const & _rstrLeft, basic_string< t_TyCharRight > const & _rstrRight )
	{
		basic_string< t_TyCharLeft > rstrLeftResult;
		ns_CONVBIEN::ConvertString( rstrLeftResult, &_rstrRight[0], _rstrRight.length() );
		VerifyThrowSz( rstrLeftResult == _rstrLeft, "Left and right didn't match for BIEN." )
	}
	template < class t_TyChar >
	void _TestSame( char32_t _utf32, basic_string< t_TyChar > * _pstrResult = nullptr )
	{
		typedef basic_string< t_TyChar > _TyStr;
		_TyStr strICU;
		_ConvICU( _utf32, strICU );
		_TyStr strBIEN;
		if ( !_pstrResult )
			_pstrResult = &strBIEN;
		_ConvBIEN( _utf32, _pstrResult );
		VerifyThrowSz( strICU == *_pstrResult, "No match for UTF32[0x%x] between ICU and BIEN impl.", _utf32 );
	}
	template < class t_TyChar >
	void _TestError( char32_t _utf32 )
	{
		// When we aren't converting we don't check for erroneous chararacters... by design at this point.
		bool fExpectFailure = !TAreSameSizeTypes_v< t_TyChar, char32_t >;
		Assert( FInvalidUTF32( _utf32 ) );
		try
		{
			typedef basic_string< t_TyChar > _TyStr;
			_TyStr strBIEN;
			_ConvBIEN( _utf32, strBIEN, vkutf16ReplacementCharError );
			VerifyThrowSz( !fExpectFailure, "We expected to fail but didn't." );
		}
		catch( std::exception const & )
		{
			if ( !fExpectFailure )
				throw; // we didn't expect to fail - fail the unit test.
		}
	}
	template < class t_TyChar >
	void _TestConvertBack( basic_string< t_TyChar > const & _rstr, char32_t _utf32 )
	{
		_ConvBackICU( _rstr, _utf32 );
		_ConvBackBIEN( _rstr, _utf32 );
	}
	template < class t_TyCharLeft, class t_TyCharRight >
	void _TestConvertBoth( basic_string< t_TyCharLeft > const & _rstrLeft, basic_string< t_TyCharRight > const & _rstrRight )
	{
		_ConvExpectICU( _rstrLeft, _rstrRight );
		_ConvExpectBIEN( _rstrLeft, _rstrRight );
		_ConvExpectICU( _rstrRight, _rstrLeft );
		_ConvExpectBIEN( _rstrRight, _rstrLeft );
	}
	void _TryTestUtf32CodePoint( char32_t _utf32 )
	{
		// Regardless of input codepoint both impls should have the result, even for invalid codepoints, 
		//	since by default we use the same replacement char we should be able to just compare.
		_TyStrChar strChar;
		_TestSame< char >( _utf32, &strChar  );
		_TyStrWChar strWChar;
		_TestSame< wchar_t >( _utf32, &strWChar );
		_TyStrChar32 strChar32;
		_TestSame< char32_t >( _utf32, &strChar32 );
		_TyStrChar16 strChar16;
		_TestSame< char16_t >( _utf32, &strChar16 );
		_TyStrChar8 strChar8;
		_TestSame< char8_t >( _utf32, &strChar8 );
		// If we are an invalid code point then we want to test two things:
		// 1) That we correctly substitute the replacement char when that is requested.
		// 2) That we correctly error out when the "error replacement char" is requested.
		if ( FInvalidUTF32( _utf32 ) )
		{
			// Test that when we ask for an error we get one:
			_TestError< char >( _utf32 );
			_TestError< wchar_t >( _utf32 );
			_TestError< char32_t >( _utf32 );
			_TestError< char16_t >( _utf32 );
			_TestError< char8_t >( _utf32 );
		}
		else
		{
			// Now convert back to utf32 from each type:
			_TestConvertBack( strChar, _utf32 );
			_TestConvertBack( strWChar, _utf32 );
			_TestConvertBack( strChar32, _utf32 );
			_TestConvertBack( strChar16, _utf32 );
			_TestConvertBack( strChar8, _utf32 );
			// Now test conversions between results - each does forward and backward.
			_TestConvertBoth( strChar, strWChar );
			_TestConvertBoth( strChar, strChar32 );
			_TestConvertBoth( strChar, strChar16 );
			_TestConvertBoth( strChar, strChar8 );
			_TestConvertBoth( strWChar, strChar32 );
			_TestConvertBoth( strWChar, strChar16 );
			_TestConvertBoth( strWChar, strChar8 );
			_TestConvertBoth( strChar32, strChar16 );
			_TestConvertBoth( strChar32, strChar8 );
			_TestConvertBoth( strChar16, strChar8 );
		}
	}
protected:
	bool m_fExpectFailure{false};
};

TEST_F( TestUtfConversions, UtfTest1 )
{
	// Move through all UTF32 codepoints and:
	// 1) Convert to both UTF16 and UTF8 strings.
	// 2) Ensure that bad strings end up being substituted appropriately or erroring out.
	// 3) Convert the UTF16 and UTF8 strings back to UTF32.
	// 4) Convert the UTF16 strings to UTF8.
	// 5) Convert the UTF8 strings to UTF32.
	// This will test all valid and invalid UTF32 values and every valid UTF16 and UTF8 value.
	// We'll 


}

int _TryMain( int argc, char **argv )
{
  if ( argc > 1 ) // We may get called by cmake infrastructure gtest_discover_tests_impl - in that case we should just return RUN_ALL_TESTS().
  {
    //VerifyThrowSz( argc > 1, "Missing file name for unit test file." );
    // We should be running in the executable output directory and we should have a subdirectory created by the build already called "unittests".
    VerifyThrowSz( FDirectoryExists( "unittests" ), "We expect to find a directory called 'unittests' in the build directory." );
    // We expect a fully qualified path name for our file - but the test environment ends up verifying that.
    (void)testing::AddGlobalTestEnvironment( vpxteBienutilTestEnvironment = new BienutilTestEnvironment( argv[1] ) );
  }
  return RUN_ALL_TESTS();
}

} // namespace vs_bienutil_test
