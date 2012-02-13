#include <cstdio>
#include "..\..\mimicry\source\Mimicry.h"

void WaitForEnterKey( MIBool a_bPrintDefaultMessage = MITrue )
{
    if ( a_bPrintDefaultMessage )
        printf( "Press ENTER to continue... " );
    for ( MIChar c = 0; ( c != '\n' ) && ( c != EOF ); c = getchar() );
}

int main( int argc, char * argv[] )
{
    mCString const strFileName = g_GetFileNameNoExt( argv[ 0 ] ).ToLower();
    MIBool const bPause = strFileName.FirstOf( "nopause" ) == MI_DW_INVALID;

    MILPCChar pcHelpText = "gmax2ase v0.1 by Baltram <baltram-lielb@web.de> (February 9th 2012)\n"
                           "gmax2ase converts .gmax files to .ase files. Only .gmax files that were created with Extended Saving enabled are compatible.\n"
                           "Usage: Simply drop a .gmax file onto the program icon.\n"
                           "For making the program exit right after the conversion, change the .exe file name so it contains the character string \"nopause\".\n";

    mCString strAseFilePath = "";
    for ( ; ; )
    {
        if ( ( argc != 2 ) || ( ( mCString( "safety" ) + argv[ 1 ] ).Right( 5 ).ToLower() != ".gmax" ) )
        {
            if ( mCString( argv[ 1 ] ) == "-?" )
                printf( pcHelpText );
            else
                printf( "Error: This program expects a .gmax file as a (single) parameter. Drop one onto the program icon to start properly.\n" );
            break;
        }

        mCMaxFileStream gmaxStream;
        if ( gmaxStream.Open( argv[ 1 ] ) == mEResult_False )
            break;

        strAseFilePath = mCString( argv[ 1 ] ).TrimRight( ( MIUInt ) 5 ) + ".ase";
        mCFileStream aseStream;
        if ( aseStream.Open( strAseFilePath, mEFileOpenMode_Write ) == mEResult_False )
            break;
        strAseFilePath = "";

        mCScene sceneGmax;
        if ( mCMaxReader::ReadInMaxFileData( sceneGmax, gmaxStream ) == mEResult_False )
            break;
        gmaxStream.Close();

        if ( mCAseWriter::WriteAseFileData( sceneGmax, aseStream, mCAseWriter::SOptions() ) == mEResult_False )
            break;
        aseStream.Close();

        printf( "Conversion successfully completed!\n" );
        break;
    }
    if ( mCError::GetLastError< mCError >() )
        printf( ( mCString( "Error: " ) + mCError::GetLastError< mCError >()->GetText() + "\n" ).GetText() );
    if ( strAseFilePath != "" )
        remove( strAseFilePath.GetText() );

    if ( bPause )
        WaitForEnterKey();
    return 0;
}