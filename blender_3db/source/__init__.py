# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

bl_info = {
    "name": "Baltram's 3D format",
    "author": "Baltram",
    "version": (1, 1),
    "blender": (2, 80, 0),
    "location": "File > Import-Export",
    "description": "Import-Export 3DB, meshes, uvs, materials, textures, vertex colors, bones, skinning",
    "warning": "",
    "category": "Import-Export"}

if "bpy" in locals():
    from importlib import reload
    if "import_3db" in locals():
        reload(import_3db)
    if "export_3db" in locals():
        reload(export_3db)


import bpy
from bpy.props import StringProperty, FloatProperty, BoolProperty, EnumProperty
from bpy_extras.io_utils import ImportHelper, ExportHelper


class Import3DB(bpy.types.Operator, ImportHelper):
    """Import from 3DB file format (.3db)"""
    bl_idname = "import_scene.3db"
    bl_label = 'Import 3DB'

    filename_ext = ".3db"
    filter_glob = StringProperty(default="*.3db", options={'HIDDEN'})

    scale: FloatProperty(
        name="Scale",
        description="Scale imported models (default 0.01)",
        min=0.0, max=1000.0,
        soft_min=0.0, soft_max=1000.0,
        default=0.01,
        )

    diffuseOnly: BoolProperty(
        name="Diffuse maps only",
        description="Don't import specular and normal maps",
        default=True,
        )

    def execute(self, context):
        from . import import_3db
        return import_3db.load(self, context, self.filepath, self.scale, self.diffuseOnly)


class Export3DS(bpy.types.Operator, ExportHelper):
    """Export to 3DB file format (.3db)"""
    bl_idname = "export_scene.3db"
    bl_label = 'Export 3DB'

    filename_ext = ".3db"
    filter_glob = StringProperty(default="*.3db", options={'HIDDEN'})

    scale: FloatProperty(
        name="Scale",
        description="Scale exported models (default 100.0)",
        min=0.0, max=1000.0,
        soft_min=0.0, soft_max=1000.0,
        default=100.0,
        )

    useSelection: BoolProperty(
        name="Selection Only",
        description="Export selected objects only",
        default=False,
        )

    def execute(self, context):
        from . import export_3db
        return export_3db.save(self, context, self.filepath, self.scale, self.useSelection)


def menu_func_export(self, context):
    self.layout.operator(Export3DS.bl_idname, text="Baltram's 3D format (.3db)")


def menu_func_import(self, context):
    self.layout.operator(Import3DB.bl_idname, text="Baltram's 3D format (.3db)")


def register():
    bpy.utils.register_class(Import3DB)
    bpy.utils.register_class(Export3DS)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(Import3DB)
    bpy.utils.unregister_class(Export3DS)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
