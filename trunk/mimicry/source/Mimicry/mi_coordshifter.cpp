#include "mi_include_converters.h"

mCCoordShifter::~mCCoordShifter( void )
{
}

mCMesh & mCCoordShifter::ShiftMeshCoords( mCMesh & a_meshDest )
{
    mCVec3 * pVerts = a_meshDest.AccessVerts();
    mCMaxFace * pFaces = a_meshDest.AccessFaces();
    mCVec3 * pTVerts = a_meshDest.AccessTVerts();
    mCFace * pTVFaces = a_meshDest.AccessTVFaces();
    mCVec3 * pVNormals = a_meshDest.AccessVNormals();
    mCFace * pVNFaces = a_meshDest.AccessVNFaces();
    mCVec3 * pVTangents = a_meshDest.AccessVTangents();
    mCFace * pVTFaces = a_meshDest.AccessVTFaces();
    for ( MIUInt u = a_meshDest.GetNumVerts(); u--; ShiftVectorCoords( pVerts[ u ] ) );
    for ( MIUInt u = a_meshDest.GetNumFaces(); u--; ShiftFaceCoords( pFaces[ u ] ) );
    if ( a_meshDest.HasTVFaces() )
    {
        for ( MIUInt u = a_meshDest.GetNumTVerts(); u--; ShiftTexCoords( pTVerts[ u ] ) );
        for ( MIUInt u = a_meshDest.GetNumFaces(); u--; ShiftFaceCoords( pTVFaces[ u ] ) );
    }
    if ( a_meshDest.HasVNFaces() )
    {
        for ( MIUInt u = a_meshDest.GetNumVNormals(); u--; ShiftVectorCoords( pVNormals[ u ] ) );
        for ( MIUInt u = a_meshDest.GetNumFaces(); u--; ShiftFaceCoords( pVNFaces[ u ] ) );
    }
    if ( a_meshDest.HasVTFaces() )
    {
        for ( MIUInt u = a_meshDest.GetNumVTangents(); u--; ShiftVectorCoords( pVTangents[ u ] ) );
        for ( MIUInt u = a_meshDest.GetNumFaces(); u--; ShiftFaceCoords( pVTFaces[ u ] ) );
    }
    return a_meshDest;
}

mCMaxRisenCoordShifter::~mCMaxRisenCoordShifter( void )
{
}

mCFace & mCMaxRisenCoordShifter::ShiftFaceCoords( mCFace & a_faceDest )
{
    g_swap( a_faceDest.AccessA(), a_faceDest.AccessC() );
    return a_faceDest;
}

mCMaxFace & mCMaxRisenCoordShifter::ShiftFaceCoords( mCMaxFace & a_faceDest )
{
    g_swap( a_faceDest.AccessA(), a_faceDest.AccessC() );
    return a_faceDest;
}

mCVec3 & mCMaxRisenCoordShifter::ShiftVectorCoords( mCVec3 & a_vecDest )
{
    g_swap( a_vecDest.AccessY(), a_vecDest.AccessZ() );
    return a_vecDest;
}

mCVec3 & mCMaxRisenCoordShifter::ShiftTexCoords( mCVec3 & a_vecDest )
{
    a_vecDest.AccessY() *= -1;
    return a_vecDest;
}