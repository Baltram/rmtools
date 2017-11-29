// All credit goes to id-daemon at zenhax.com
// https://zenhax.com/viewtopic.php?f=17&t=1901

#include <cstdio>
#include "Windows.h"

void WaitForEnter( char const * a_pcMessage = 0 )
{
    if ( a_pcMessage )
        printf( "%s\n", a_pcMessage );
    printf( "Press ENTER to exit... " );
    for ( char c = 0; c != '\n' && c != EOF; c = getchar() );
}

int main( int argc, char * argv[] )
{
    if ( argc < 2 )
        return WaitForEnter( "Start the conversion by dragging a .fsb file onto the executable." ), 1;

    HINSTANCE hinstLib;
    hinstLib = LoadLibrary( "fmod64.dll" );
    if ( hinstLib == NULL )
        return WaitForEnter( "Error: fmod64.dll missing (can be found in the folder ELEX\\system)" ), 1;

    int ( *pFMOD_System_Create )( void ** );
    *( FARPROC * ) ( &pFMOD_System_Create ) = GetProcAddress( hinstLib, "FMOD_System_Create" );

    int ( *pFMOD_System_Init )( void *, int, unsigned int, void * );
    *( FARPROC * ) ( &pFMOD_System_Init ) = GetProcAddress( hinstLib, "FMOD_System_Init" );

    int ( *pFMOD_System_CreateSound )( void *, char *, unsigned int, void *, void ** );
    *( FARPROC * ) ( &pFMOD_System_CreateSound ) = GetProcAddress( hinstLib, "FMOD_System_CreateSound" );

    int ( *pFMOD_Sound_GetNumSubSounds )( void *, int * );
    *( FARPROC * ) ( &pFMOD_Sound_GetNumSubSounds ) = GetProcAddress( hinstLib, "FMOD_Sound_GetNumSubSounds" );

    int ( *pFMOD_Sound_GetSubSound )( void *, int, void ** );
    *( FARPROC * ) ( &pFMOD_Sound_GetSubSound ) = GetProcAddress( hinstLib, "FMOD_Sound_GetSubSound" );

    int ( *pFMOD_Sound_SeekData )( void *, unsigned int );
    *( FARPROC * ) ( &pFMOD_Sound_SeekData ) = GetProcAddress( hinstLib, "FMOD_Sound_SeekData" );

    int ( *pFMOD_Sound_GetDefaults )( void *, float *, int * );
    *( FARPROC * ) ( &pFMOD_Sound_GetDefaults ) = GetProcAddress( hinstLib, "FMOD_Sound_GetDefaults" );

    int ( *pFMOD_Sound_GetFormat )( void *, void *, void *, int *, int * );
    *( FARPROC * ) ( &pFMOD_Sound_GetFormat ) = GetProcAddress( hinstLib, "FMOD_Sound_GetFormat" );

    int ( *pFMOD_Sound_GetLength )( void *, unsigned int *, unsigned int );
    *( FARPROC * ) ( &pFMOD_Sound_GetLength ) = GetProcAddress( hinstLib, "FMOD_Sound_GetLength" );

    int ( *pFMOD_Sound_GetName )( void *, char *, int );
    *( FARPROC * ) ( &pFMOD_Sound_GetName ) = GetProcAddress( hinstLib, "FMOD_Sound_GetName" );

    int ( *pFMOD_Sound_ReadData )( void *, void *, unsigned int, unsigned int * );
    *( FARPROC * ) ( &pFMOD_Sound_ReadData ) = GetProcAddress( hinstLib, "FMOD_Sound_ReadData" );

    if ( !pFMOD_System_Create ||
         !pFMOD_System_Init ||
         !pFMOD_System_CreateSound ||
         !pFMOD_Sound_GetNumSubSounds ||
         !pFMOD_Sound_GetSubSound ||
         !pFMOD_Sound_SeekData ||
         !pFMOD_Sound_GetDefaults ||
         !pFMOD_Sound_GetFormat ||
         !pFMOD_Sound_GetLength ||
         !pFMOD_Sound_GetName ||
         !pFMOD_Sound_ReadData )
        return WaitForEnter( "Error: Unsupported fmod64.dll version" ), 1;

    void * pSystem = 0;
    void * pSound = 0;
    if ( pFMOD_System_Create( &pSystem ) != 0 ||
         pFMOD_System_Init( pSystem, 32, 0, 0 ) != 0 )
        return WaitForEnter( "Error: FMOD system initialization failed" ), 1;
         
    if ( pFMOD_System_CreateSound( pSystem, argv[ 1 ], 0x4080, 0, &pSound ) != 0 )
        return WaitForEnter( "Error: Unsupported sound file" ), 1;

    int iResult = 0;
    int iNumSubSounds = 0;
    iResult |= pFMOD_Sound_GetNumSubSounds( pSound, &iNumSubSounds );

    for ( int i = 0; i < iNumSubSounds; ++i )
    {
        void * pSubSound = 0;
        iResult |= pFMOD_Sound_GetSubSound( pSound, i, &pSubSound );
        iResult |= pFMOD_Sound_SeekData( pSubSound, 0 );

        float fFrequency = 0;
        iResult |= pFMOD_Sound_GetDefaults( pSubSound, &fFrequency, 0 );

        int iNumChannels = 0;
        int iNumBitsPerSample = 0;
        iResult |= pFMOD_Sound_GetFormat( pSubSound, 0, 0, &iNumChannels, &iNumBitsPerSample );

        unsigned int uLength = 0;
        iResult |= pFMOD_Sound_GetLength( pSubSound, &uLength, 2 );

        char arrName[ 255 ];
        iResult |= pFMOD_Sound_GetName( pSubSound, arrName, sizeof( arrName ) );

        unsigned int uNumBytesPerSample = ( iNumBitsPerSample + 7 ) / 8;
        unsigned int uFrameSize = iNumChannels * uNumBytesPerSample;
        unsigned int uDataSize = uLength * uFrameSize;
        unsigned int uReadSize = 0;
        void * pBuffer = malloc( uDataSize );
        iResult |= pFMOD_Sound_ReadData( pSubSound, pBuffer, uDataSize, &uReadSize );
        if ( iResult )
            return WaitForEnter( "Error: Failed to retrieve sound format and data" ), 1;

        char arrFilePath[ 1024 ];
        char arrFileName[ 255 ];
        char arrDirPath[ 1024 ];
        if ( 0 != _splitpath_s( argv[ 1 ], arrFilePath, sizeof( arrFilePath ), arrDirPath, sizeof( arrDirPath ), arrFileName, sizeof( arrFileName ), 0, 0 ) )
            return WaitForEnter( "Error: Failed to determine output file path" ), 1;

        strcat_s( arrFilePath, sizeof( arrFilePath ), arrDirPath );
        if ( arrName[ 0 ] == 0 )
        {
            strcat_s( arrFilePath, sizeof( arrFilePath ), arrFileName );
            if ( iNumSubSounds > 1 )
                sprintf_s( arrFilePath + strnlen_s( arrFilePath, sizeof( arrFilePath ) ), sizeof( arrFilePath ) - strnlen_s( arrFilePath, sizeof( arrFilePath ) ), "_%u", i );
        }
        else
            strcat_s( arrFilePath, sizeof( arrFilePath ), arrName );
        strcat_s( arrFilePath, sizeof( arrFilePath ), ".wav" );

        FILE * pFile = 0;
        pFile = fopen( arrFilePath, "wb" );
        if ( !pFile )
        {
            printf( "Error: Could not write to %s\n", arrFilePath );
            return WaitForEnter(), 1;
        }

        unsigned int uTemp;
        unsigned int uFrequency = fFrequency;
        fwrite( "RIFF", 4, 1, pFile );
        fwrite( &( uTemp = uDataSize + 36 ), 4, 1, pFile );
        fwrite( "WAVE", 4, 1, pFile );
        fwrite( "fmt ", 4, 1, pFile );
        fwrite( &( uTemp = 16 ), 4, 1, pFile );
        fwrite( &( uTemp = 1 ), 2, 1, pFile );
        fwrite( &iNumChannels, 2, 1, pFile );
        fwrite( &uFrequency, 4, 1, pFile );
        fwrite( &( uTemp = uFrequency * uFrameSize ), 4, 1, pFile );
        fwrite( &uFrameSize, 2, 1, pFile );
        fwrite( &iNumBitsPerSample, 2, 1, pFile );
        fwrite( "data", 4, 1, pFile );
        fwrite( &uDataSize, 4, 1, pFile );
        fwrite( pBuffer, uReadSize, 1, pFile );
        fclose( pFile );
    }
    return 0;
}
