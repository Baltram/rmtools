#include "mi_include_converters.h"

namespace
{
    MIInt CompareNodePointers( MILPCVoid a_pNodePtr1, MILPCVoid a_pNodePtr2 )
    {
        mCNode const * pNode1 = *static_cast< mCNode const * const * >( a_pNodePtr1 );
        mCNode const * pNode2 = *static_cast< mCNode const * const * >( a_pNodePtr2 );
        if ( pNode1->HasMesh() && pNode2->HasMesh() )
            return pNode2->GetMesh()->GetNumVerts() - pNode1->GetMesh()->GetNumVerts();
        return 0;
    }

    void WriteString( mCIOStreamBinary & a_streamDest, mCString const & a_strSource )
    {
        a_streamDest << ( MIU16 )( a_strSource.GetLength() ) << a_strSource;
    }
}

mEResult mCXlmshWriter::WriteXlmshFileData( mCScene const & a_sceneSource, mCIOStreamBinary & a_streamDest )
{
    mTArray< mCNode const * > arrNodes;
    for ( MIUInt u = 0, ue = a_sceneSource.GetNumNodes(); u != ue; ++u )
        if ( a_sceneSource.GetNodeAt( u )->HasMesh() )
            arrNodes.Add( a_sceneSource.GetNodeAt( u ) );
    if ( arrNodes.GetCount() == 0 )
        return MI_ERROR( mCConverterError, EBadScene, "The scene contains no mesh." ), mEResult_False;
    qsort( arrNodes.AccessBuffer(), arrNodes.GetCount(), sizeof( mCNode const * ), &CompareNodePointers );
    mCBox boxBoundary;
    for ( MIUInt u = arrNodes.GetCount(); u--; boxBoundary |= arrNodes[ u ]->GetMesh()->CalcExtents() );
    mCMaxRisenCoordShifter::GetInstance().ShiftVectorCoords( boxBoundary.AccessMin() );
    mCMaxRisenCoordShifter::GetInstance().ShiftVectorCoords( boxBoundary.AccessMax() );
    MIU16 const u16PropertySystemVersion = 83, u16PropertyVersion = 30, u16ResourceLodMeshVersion = 23;
    a_streamDest << ( MIU16 )( 1 ) << ( MIU8 )( 1 ) << ( MIU16 )( 1 ) << ( MIU8 )( 1 );
    WriteString( a_streamDest, "eCResourceMeshLoD_PS" );
    a_streamDest << ( MIU8 )( 1 ) << ( MIU16 )( 0 );
    a_streamDest << u16PropertySystemVersion  << u16PropertySystemVersion;
    MIUInt const uResourceLodMeshSizeOffset = a_streamDest.Tell();
    a_streamDest << ( MIU32 )( 0 ) << u16PropertyVersion << ( MIU32 )( 3 );
    WriteString( a_streamDest, "LoDScale" );
    WriteString( a_streamDest, "float" );
    a_streamDest << u16PropertyVersion << sizeof( MIFloat ) << 1.0f;
    WriteString( a_streamDest, "BoundingBox" );
    WriteString( a_streamDest, "bCBox" );
    a_streamDest << u16PropertyVersion << 24 << boxBoundary.GetMin() << boxBoundary.GetMax();
    WriteString( a_streamDest, "ResourcePriority" );
    WriteString( a_streamDest, "float" );
    a_streamDest << u16PropertyVersion << sizeof( MIFloat ) << 0.0f;
    a_streamDest << u16ResourceLodMeshVersion << ( MIU16 )( 1 ) << ( MIU16 )( 1 );
    a_streamDest << g_32( arrNodes.GetCount() );
    for ( MIUInt u = 0, ue = arrNodes.GetCount(); u != ue; ++u )
        WriteString( a_streamDest, arrNodes[ u ]->GetName() + ".xcmsh" );
    MIUInt const uOffsetEnd = a_streamDest.Tell();
    a_streamDest.Seek( uResourceLodMeshSizeOffset );
    a_streamDest << g_32( uOffsetEnd - uResourceLodMeshSizeOffset - 4 );
    a_streamDest.Seek( uOffsetEnd );
    return mEResult_Ok;
}
