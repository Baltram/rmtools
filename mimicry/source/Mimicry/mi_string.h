#ifndef MI_STRING_H_INCLUDED
#define MI_STRING_H_INCLUDED

class mCString
{
public:
    enum
    {
        EMinStaticBufferSize = 8096
    };
public:
    explicit mCString( MIUInt a_uCount );
             mCString( MILPCChar a_pcText );
             mCString( MILPCChar a_pcText, MIUInt a_uCount );
             mCString( MIChar a_cChar, MIUInt a_uCount );
             mCString( MILPCChar a_pcText1, MILPCChar a_pcText2 );
             mCString( MILPCChar a_pcText1, MIUInt uLength1, MILPCChar a_pcText2, MIUInt uLength2 );
             mCString( mCString const & a_strText );
             mCString( void );
            ~mCString( void );
public:
    mCString & operator =         ( mCString const & a_strText );
    mCString & operator =         ( MIChar a_cChar );
    mCString & operator =         ( MILPCChar a_pcText );
    MIBool     operator ==        ( mCString const & a_strOther ) const;
    MIBool     operator ==        ( MILPCChar a_pcOther ) const;
    MIBool     operator !=        ( mCString const & a_strOther ) const;
    MIBool     operator !=        ( MILPCChar a_pcOther ) const;
    MIChar &   operator []        ( MIInt a_uIndex );
    MIChar     operator []        ( MIInt a_uIndex ) const;
    mCString   operator +         ( mCString const & a_strText ) const;
    mCString   operator +         ( MIChar a_cChar ) const;
    mCString   operator +         ( MILPCChar a_pcText ) const;
    MIBool     operator <         ( mCString const & a_strOther ) const;
    MIBool     operator <         ( MILPCChar a_pcOther ) const;
    MIBool     operator <=        ( mCString const & a_strOther ) const;
    MIBool     operator <=        ( MILPCChar a_pcOther ) const;
    MIBool     operator >         ( mCString const & a_strOther ) const;
    MIBool     operator >         ( MILPCChar a_pcOther ) const;
    MIBool     operator >=        ( mCString const & a_strOther ) const;
    MIBool     operator >=        ( MILPCChar a_pcOther ) const;
    mCString & operator +=        ( mCString const & a_strText );
    mCString & operator +=        ( MIChar a_cChar );
    mCString & operator +=        ( MILPCChar a_pcText );
public:
    static void     AssureStaticBufferSize( MIUInt a_uSize );
    static MILPChar AccessStaticBuffer( void );
    static MIUInt   GetStaticBufferSize( void );
    static MILPChar Stringize( MIChar a_cChar );
public:
    MILPChar   AccessText( void );
    mCString & Append( mCString const & a_strText ); 
    mCString & Append( MILPCChar a_pcText, MIUInt a_uLength = MI_DW_INVALID );
    void       Clear( void );
    MIInt      Compare( mCString const & a_strOther ) const;
    MIInt      Compare( MILPCChar a_pcOther ) const;
    MIInt      CompareNoCase( mCString const & a_strOther ) const;
    MIInt      CompareNoCase( MILPCChar a_pcOther ) const;
    MIBool     Contains( MIChar a_cChar ) const;
    MIBool     Contains( MILPCChar a_pcString ) const;
    MIUInt     Count( MILPCChar a_pcText ) const;
    MIUInt     Count( MIChar a_cChar ) const;
    MIUInt     FirstOf( MILPCChar a_pcText ) const;
    MIUInt     FirstOf( MIChar a_cChar ) const;
    mCString & Format( MILPCChar a_pcFormat, ... );  // Unsafe if resulting text is bigger than EMinStaticBufferSize.
    MIUInt     GetLength( void ) const;
    MILPCChar  GetText( void ) const;
    MIUInt     LastOf( MILPCChar a_pcText ) const;
    MIUInt     LastOf( MIChar a_cChar ) const;
    mCString   Left( MIUInt a_uCount ) const;
    mCString & Replace( MILPCChar a_pcText, MILPCChar a_pcNewText, MIUInt & a_uNumReplaced );
    mCString & Replace( MILPCChar a_pcText, MILPCChar a_pcNewText );
    mCString & Replace( MIChar a_cChar, MIChar a_cNewChar, MIUInt & a_uNumReplaced );
    mCString & Replace( MIChar a_cChar, MIChar a_cNewChar );
    mCString   Right( MIUInt a_uCount ) const;
    MIInt      Scan( MILPCChar a_pcFormat, ... ) const;
    void       SetText( mCString const & a_strText );
    void       SetText( MILPCChar a_pcText );
    void       SetText( MILPCChar a_pcText, MIUInt a_uCount );
    void       SetText( MIChar a_cChar, MIUInt a_uCount );
    void       Swap( mCString & a_strOther );
    mCString & ToLower( void );
    mCString & ToUpper( void );
    mCString & TrimLeft( MILPCChar a_pcChars );
    mCString & TrimLeft( MIChar a_cChar );
    mCString & TrimLeft( MIUInt a_uCount );
    mCString & TrimRight( MILPCChar a_pcChars );
    mCString & TrimRight( MIChar a_cChar );
    mCString & TrimRight( MIUInt a_uCount );
    mCString & VFormat( MILPCChar a_pcFormat, va_list a_Arguments );
    MIInt      VScan( MILPCChar a_pcFormat, va_list a_Arguments ) const;
private:
    static MILPChar  Alloc( MILPCChar a_pcText, MIUInt a_uLength = 0, MILPVoid a_Memory = 0 );
    static MILPChar  Alloc( MILPCChar a_pcText1, MIUInt a_uLength1, MILPCChar a_pcText2, MIUInt a_uLength2, MILPVoid a_Memory = 0 );
    static MILPCChar NextOf( MILPCChar & a_pcText, MILPCChar a_pcEndText, MIChar a_cChar );
    static MILPCChar NextOf( MILPCChar & a_pcText, MILPCChar a_pcEndText, MILPCChar a_pcString, MIUInt a_uStringLength );
private:
    static MILPChar s_pcBuffer;
    static MIUInt   s_uBufferSize;
private:
    MILPChar m_pcText;
};

MIBool   operator == ( MILPCChar a_pcText1, mCString const & a_strText2 );
MIBool   operator != ( MILPCChar a_pcText1, mCString const & a_strText2 );
mCString operator +  ( MIChar a_cLeft, mCString const & a_strRight );
mCString operator +  ( MILPCChar a_pcLeft, mCString const & a_strRight );
MIBool   operator <  ( MILPCChar a_pcText1, mCString const & a_strText2 );
MIBool   operator <= ( MILPCChar a_pcText1, mCString const & a_strText2 );
MIBool   operator >  ( MILPCChar a_pcText1, mCString const & a_strText2 );
MIBool   operator >= ( MILPCChar a_pcText1, mCString const & a_strText2 );

mCString   g_GetFileExt( mCString const & a_strFilePath );
mCString   g_GetFileName( mCString const & a_strFilePath );
mCString   g_GetFileNameNoExt( mCString const & a_strFilePath );
mCString   g_GetDirectoryPath( mCString const & a_strFilePath );
mCString   g_GetDirectoryName( mCString const & a_strFilePath );
mCString & g_ReplaceFileExt( mCString & a_strFilePath, mCString const & a_strNewExt );

#include "mi_string.inl"

#endif
