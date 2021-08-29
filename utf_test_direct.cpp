
//          Copyright David Lawrence Bien 1997 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt).

// utf_test_direct.cpp : Direct test UTF conversions.
//

#include "bienutil_test.h"
#include "_util.h"

std::string g_strProgramName;

namespace ns_UtfTestDirect
{
using namespace ns_bienutil_test;
__BIENUTIL_USING_NAMESPACE

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
		VerifyThrowSz( ( 1 == str32.length() ) && ( str32[0] == _utf32 ), "Conversion back failed for ICU." );
	}
	template < class t_TyChar >
	void _ConvBackBIEN( const basic_string< t_TyChar > & _rstr, char32_t _utf32 )
	{
		basic_string< char32_t > str32;
		ns_CONVBIEN::ConvertString( str32, &_rstr[0], _rstr.length() );
		VerifyThrowSz( ( 1 == str32.length() ) && ( str32[0] == _utf32 ), "Conversion back failed for BIEN." );
	}
	template < class t_TyCharLeft, class t_TyCharRight >
	void _ConvExpectICU( basic_string< t_TyCharLeft > const & _rstrLeft, basic_string< t_TyCharRight > const & _rstrRight )
	{
		basic_string< t_TyCharLeft > rstrLeftResult;
		ns_CONVICU::ConvertString( rstrLeftResult, &_rstrRight[0], _rstrRight.length() );
		VerifyThrowSz( rstrLeftResult == _rstrLeft, "Left and right didn't match for ICU." );
	}
	template < class t_TyCharLeft, class t_TyCharRight >
	void _ConvExpectBIEN( basic_string< t_TyCharLeft > const & _rstrLeft, basic_string< t_TyCharRight > const & _rstrRight )
	{
		basic_string< t_TyCharLeft > rstrLeftResult;
		ns_CONVBIEN::ConvertString( rstrLeftResult, &_rstrRight[0], _rstrRight.length() );
		VerifyThrowSz( rstrLeftResult == _rstrLeft, "Left and right didn't match for BIEN." );
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
		_ConvBIEN( _utf32, *_pstrResult );
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

	template < class t_TyChar >
	void _PiecewiseConvertAllInvalidUTF32( const t_TyChar * _pc, size_t _nLen ) const
	{
		const t_TyChar * pcEnd = _pc + _nLen;
		size_t nthTimeThrough = 0;
		for ( const t_TyChar * pcCur = _pc; pcEnd != pcCur; ++nthTimeThrough )
		{
			char32_t u32;
			char32_t * pcDest = &u32;
			const t_TyChar * pcNext = PCConvertString( pcCur, ( pcEnd - pcCur ), pcDest, 1 );
			VerifyThrow( !!pcNext );
			Assert( pcNext != pcCur ); // must advance loop.
			Assert( vkutf16ReplacementCharDefault == u32 );
			pcCur = pcNext;
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
	// We'll still need to test invalid UTF16 and UTF8 values.
	for ( char32_t utf32 = 1; utf32 <= 0x10ffff; ++utf32 )
	{
		TestUtf32CodePoint( utf32 );
	}
	// Choose a smattering of things over the maximum UTF32 character.
	char32_t rgutf32Invalid[] = { 0x10ffff + 1, 0x10fffff, 0xffffffff };
	const char32_t * putf32InvalidCur = rgutf32Invalid; 
	const char32_t * const putf32InvalidEnd = putf32InvalidCur + DimensionOf( rgutf32Invalid );
	for ( ; putf32InvalidEnd != putf32InvalidCur; ++putf32InvalidCur )
	{
		TestUtf32CodePoint( *putf32InvalidCur );
	}

	// Now let's try some specific "known to be bogus" sequences and assert that they produce all replacement characters:
	// From: 3.4  Concatenation of incomplete sequences.
	// Have to use uint8_t because when it's a string the compiler decides to muck with the data... weird.
	static const uint8_t rgcBogus_3_4_IncompleteSequences[] = { 0xC0, 0xE0, 0x80, 0xF0, 0x80, 0x80, 0xF8, 0x80, 0x80, 0x80, 0xFC, 0x80, 0x80, 0x80, 0x80, 0xDF, 0xEF, 0xBF, 0xF7, 0xBF, 0xBF, 0xFB, 0xBF, 0xBF, 0xBF, 0xFD, 0xBF, 0xBF, 0xBF, 0xBF };
	// We will piecewise convert these so we can easily find the spot where it doesn't produce the right thing.
	_PiecewiseConvertAllInvalidUTF32( (const char8_t*)rgcBogus_3_4_IncompleteSequences, DimensionOf( rgcBogus_3_4_IncompleteSequences ) );
}

int _TryMain( int argc, char **argv )
{
  return RUN_ALL_TESTS();
}
} // namespace ns_UtfTestDirect

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
    return ns_UtfTestDirect::_TryMain(argc, argv);
  }
  catch( const std::exception & rexc )
  {
    n_SysLog::Log( eslmtError, "%s: *** Exception: [%s]", g_strProgramName.c_str(), rexc.what() );
    fprintf( stderr, "%s: *** Exception: [%s]\n", g_strProgramName.c_str(), rexc.what() );      
    return -1;
  }
}

