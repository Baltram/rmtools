#ifndef MI_ERROR_H_INCLUDED
#define MI_ERROR_H_INCLUDED

enum mEResult
{
    mEResult_False      = MIFalse,
    mEResult_Ok         = MITrue,
    mEResult_ForceDWORD = MI_FORCE_DWORD
};
        
class mCError
{
public:
    enum
    {
        EGeneral,
        EForceDWORD = MI_FORCE_DWORD
    };
    class CProbe
    {
    public:
        CProbe( void );
       ~CProbe( void );
    private:
        mCError const * m_pLastError;
    };
public:
    template< typename T > static void            ClearAllErrors();
                           static void            ClearError( mCError const * a_pError ); 
    template< typename T > static void            Error( MIInt a_iErrorCode, MILPCChar a_pcText, MILPCChar a_pcFile, MIUInt a_uLine );
    template< typename T > static mCError const * GetLastError( void );
public:
    MILPCChar                    GetText( void ) const;
    MILPCChar                    GetFile( void ) const;
    MIUInt                       GetLine( void ) const;
    template< typename T > MIInt GetErrorCode( void ) const;
protected:
    virtual ~mCError( void );
protected:
    mCError( void );
private:
    mCError( mCError const & );
private:
    void Init( MIInt a_iErrorCode, MILPCChar a_pcText, MILPCChar a_pcFile, MIUInt a_uLine, mCError * a_pPrevious );
private:
    static mCError * s_pLastError;
private:
    MIInt     m_iErrorCode;
    MILPChar  m_pcText;
    MILPChar  m_pcFile;
    MIUInt    m_uLine;
    mCError * m_pPrevious;
};

#include "mi_error.inl"

#endif