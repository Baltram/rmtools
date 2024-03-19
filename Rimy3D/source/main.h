#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "rimy3d.h"

enum CliOption
{
    CliOption_ExportTextures,
    CliOption_ExportNormals,
    CliOption_NormalsRecalcBySmoothingGroups,
    CliOption_NormalsRecalcByAngle,
    CliOption_NormalsKeepIfPossible,
    CliOption_ExportVertexColors,
    CliOption_BaseFile,
    CliOption_OnlyVertices,
    CliOption_AutoSkinDirectVertexMatching,
    CliOption_MeshIsConvex,
    CliOption_ExportMtl,
    CliOption_Gothic3dsFormat,
    CliOption_ExportSmoothingGroups,
    CliOption_LookUpTextures,
    CliOption_LookUpMaterials,
    CliOption_DefaultImageFileExt,
    CliOption_RemoveXmacCollisionMesh,
    CliOption_RemoveAscPrefixes,
    CliOption_Count
};

class ConsoleHelper: public QObject
{
    Q_OBJECT
public:
    ConsoleHelper( void );
   ~ConsoleHelper( void );
private slots:
    void onMessage( QString const & a_strText, Rimy3D::EMessage a_enuType );
private:
    bool m_bAttached;
};

#endif  // MAIN_H_INCLUDED
