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
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# ##### END GPL LICENSE BLOCK #####

#Author:  Baltram
#Version: 0.5

import ntpath
import struct

import bpy
import mathutils

from bpy_extras.image_utils import load_image
from math import pi


VERSION_3DB = 1


class ChunkID:
    range = range(1, 6)
    material = 1
    texmap = 2
    node = 3
    mesh = 4
    skin = 5

class NodeType:
    mesh = 1
    bone = 2
    other = 3

class MaterialType:
    standard = 1
    multi = 2
    sub = 3

class MapType:
    diffuse = 2
    specular = 3
    normal = 9

class Node:
    def __init__(self):
        self.type = NodeType.other
        self.name = ''
        self.parentIndex = -1
        self.materialIndex = -1
        self.transform = 0

class Mesh:
    def __init__(self):
        self.nodeIndex = -1
        self.verts = []
        self.faces = []
        self.tverts = []
        self.tfaces = []
        self.vcolors = []
        self.materialIDs = []

class Material:
    def __init__(self):
        self.name = ''
        self.subMaterials = []

class TexMap:
    def __init__(self):
        self.materialIndex = -1
        self.subMaterialIndex = -1
        self.type = MapType.diffuse
        self.texturePath = ''

class Skin:
    def __init__(self):
        self.nodeIndex = -1
        self.boneIndices = []
        self.weightsPerVertex = []

class SceneData:
    def __init__(self):
        self.filePath = ''
        self.scale = 0.01
        self.diffuseOnly=True
        self.nodes = []
        self.meshes = []
        self.materials = []
        self.texMaps = []
        self.skins = []
        self.smoothAngle = -1.0


def unpack(format, buffer, dispose=False):
    while True:
        pos = format.find('z')
        if pos < 0:
            break
        asciiz_start = struct.calcsize(format[:pos])
        asciiz_len = buffer[0][asciiz_start:].find(b'\x00')
        format = '%s%dsx%s' % (format[:pos], asciiz_len, format[pos+1:])
    size = struct.calcsize(format)
    result = struct.unpack(format, buffer[0][:size])
    if dispose:
        buffer[0] = buffer[0][size:]
    return result


def as_index(i):
    return i - 1


def as_string(b):
    return b.decode('ascii')


def read_material_chunk(chunkData, scene):
    mat = Material()
    mat.name, type = unpack('<zI', chunkData)
    if type == MaterialType.sub:
        parentMatIndex = as_index(unpack('<zII', chunkData)[2])
        scene.materials[parentMatIndex].subMaterials.append(mat)
    else:
        scene.materials.append(mat)


def read_texmap_chunk(chunkData, scene):
    map = TexMap()
    map.materialIndex, map.subMaterialIndex, map.type, map.texturePath = unpack('<IIIz', chunkData)
    map.materialIndex = as_index(map.materialIndex)
    map.subMaterialIndex = as_index(map.subMaterialIndex)
    scene.texMaps.append(map)


def read_mesh_chunk(chunkData, scene):
    mesh = Mesh()
    mesh.nodeIndex = as_index(unpack('<I', chunkData, True)[0])
    vertCount, = unpack('<I', chunkData, True)
    mesh.verts = unpack('<%df' % (vertCount * 3), chunkData, True)
    faceCount, = unpack('<I', chunkData, True)
    mesh.faces = [i - 1 for i in unpack('<%dI' % (faceCount * 5), chunkData, True)]
    tvertCount, = unpack('<I', chunkData, True)
    if tvertCount != 0:
        mesh.tverts = unpack('<%df' % (tvertCount * 3), chunkData, True)
        mesh.tfaces = [i - 1 for i in unpack('<%dI' % (faceCount * 3), chunkData, True)]
    if vertCount == unpack('<I', chunkData, True)[0]:
        mesh.vcolors = unpack('<%dB' % (vertCount * 4), chunkData, True)
    scene.meshes.append(mesh)


def read_node_chunk(chunkData, scene):
    node = Node()
    data = unpack('<IzII16f', chunkData)
    node.type = data[0]
    node.name = data[1]
    node.parentIndex = as_index(data[2])
    node.materialIndex = as_index(data[3])
    node.transform = mathutils.Matrix((data[4:8], data[8:12],
                                       data[12:16], data[16:20])).transposed()
    scene.nodes.append(node)


def read_skin_chunk(chunkData, scene):
    skin = Skin()
    skin.nodeIndex = as_index(unpack('<I', chunkData, True)[0])
    boneCount, = unpack('<I', chunkData, True)
    skin.boneIndices = [as_index(i) for i in unpack('<%dI' % boneCount, chunkData, True)]
    vertCount, = unpack('<I', chunkData, True)
    skin.weightsPerVertex = [None] * vertCount
    for i in range(vertCount):
        weightCount, = unpack('<I', chunkData, True)
        skin.weightsPerVertex[i] = []
        for j in range(weightCount):
            boneIndex, weight = unpack('<If', chunkData, True)
            skin.weightsPerVertex[i].append((as_index(boneIndex), weight))
    scene.skins.append(skin)


def create_node(nodeData, scale, mesh=None):
    if nodeData.type == NodeType.bone:
        return
    node = bpy.data.objects.new(as_string(nodeData.name), mesh)
    bpy.context.scene.objects.link(node)
    node.matrix_world = nodeData.transform
    if node.data != None:
        node.data.transform(node.matrix_local.inverted())
    node.matrix_world = mathutils.Matrix.Scale(scale, 4) * node.matrix_world
    return node


def build_scene(scene):

    #Create Blender materials
    materials = {}
    for i in range(len(scene.materials)):
        mat = scene.materials[i]
        for j in range(len(mat.subMaterials)):
            materials[i<<16 | j&0xFFFF] = bpy.data.materials.new(as_string(mat.subMaterials[j].name))
        if len(mat.subMaterials) == 0:
            materials[i<<16 | -1&0xFFFF] = bpy.data.materials.new(as_string(mat.name))
    for key in materials:
        materials[key].specular_intensity = 0

    #Load images
    images = {}
    for texMap in scene.texMaps:
        images[texMap.texturePath] = None
    for path in images:
        images[path] = load_image(as_string(path), ntpath.dirname(scene.filePath))

    #Create Blender textures
    for texMap in scene.texMaps:
        if scene.diffuseOnly and texMap.type != MapType.diffuse:
            continue
        mat = materials[texMap.materialIndex << 16 | texMap.subMaterialIndex & 0xFFFF]
        tex = bpy.data.textures.new(ntpath.basename(as_string(texMap.texturePath)), type='IMAGE')
        if images[texMap.texturePath]:
            tex.image = images[texMap.texturePath]
        slot = mat.texture_slots.add()
        slot.texture = tex
        slot.texture_coords = 'UV'
        slot.use_map_color_diffuse = texMap.type == MapType.diffuse
        slot.use_map_specular = texMap.type == MapType.specular
        slot.use_map_normal = texMap.type == MapType.normal

    #Identify bones
    def check_parents(boneIndices, i):
        if scene.nodes[i].type == NodeType.mesh:
            return False
        if i in boneIndices:
            return True
        if scene.nodes[i].parentIndex >= 0 and check_parents(boneIndices, scene.nodes[i].parentIndex):
            boneIndices.append(i)
            scene.nodes[i].type = NodeType.bone
            return True
        return False
    if len(scene.skins) == 1:
        boneIndices = scene.skins[0].boneIndices
        for i in range(len(scene.nodes)):
            if scene.nodes[i].type == NodeType.bone:
                if i not in boneIndices:
                    boneIndices.append(i)
    else:
        for nodeData in scene.nodes:
            if nodeData.type == NodeType.bone:
                nodeData.type = NodeType.other
    for skinData in scene.skins:
        for i in skinData.boneIndices:
            nodeData = scene.nodes[i]
            nodeData.type = NodeType.bone
            while nodeData.parentIndex >= 0:
                if nodeData.parentIndex not in skinData.boneIndices:
                    skinData.boneIndices.append(nodeData.parentIndex)
                nodeData = scene.nodes[nodeData.parentIndex]
                nodeData.type = NodeType.bone
        for i in range(len(scene.nodes)):
            check_parents(skinData.boneIndices, i)

    #Create Blender Empties
    nodes = [None] * len(scene.nodes)
    for i in range(len(scene.nodes)):
        if scene.nodes[i].type == NodeType.other:
            nodes[i] = create_node(scene.nodes[i], scene.scale)

    #Create Blender meshes
    meshes = []
    for meshData in scene.meshes:
        nodeData = scene.nodes[meshData.nodeIndex]
        name = as_string(nodeData.name)
        verts = [meshData.verts[i:i+3] for i in range(0, len(meshData.verts), 3)]
        faces = [meshData.faces[i:i+3] for i in range(0, len(meshData.faces), 5)]
        matIDs = [meshData.faces[i+3] for i in range(0, len(meshData.faces), 5)]
        tverts = [meshData.tverts[i:i+2] for i in range(0, len(meshData.tverts), 3)]
        tfaceIndices = meshData.tfaces[:]
        #eekadoodle
        for i in range(len(faces)):
            if faces[i][2] == 0:
                faces[i] = [0] + faces[i][:2]
                if len(tverts):
                    ti = tfaceIndices
                    ti[i*3 + 0], ti[i*3 + 1], ti[i*3 + 2] = ti[i*3 + 2], ti[i*3 + 0], ti[i*3 + 1]
        mesh = bpy.data.meshes.new(name)
        mesh.from_pydata(verts, [], faces)
        mesh.polygons.foreach_set('material_index', matIDs)
        if len(meshData.vcolors) != 0:
            vcolors = [c / 255.0 for c in meshData.vcolors]
            mesh.vertex_colors.new(name = 'rgb')
            mesh.vertex_colors.new(name = 'alpha')
            fverts = [i for f in faces for i in f]
            mesh.vertex_colors[0].data.foreach_set('color', [val for i in fverts for val in vcolors[i*4:i*4 + 3]])
            mesh.vertex_colors[1].data.foreach_set('color', [val for i in fverts for val in [vcolors[i*4 + 3]]*3])
        if len(tverts):
            mesh.uv_textures.new()
            mesh.uv_layers[-1].data.foreach_set('uv', [coord for i in tfaceIndices for coord in tverts[i]])
        if nodeData.materialIndex >= 0:
            subs = scene.materials[nodeData.materialIndex].subMaterials
            for i in range(len(subs)):
                mesh.materials.append(materials[nodeData.materialIndex << 16 | i & 0xFFFF])
            if len(subs) == 0:
                mesh.materials.append(materials[nodeData.materialIndex << 16 | -1 & 0xFFFF])
        if scene.smoothAngle >= 0.0:
            mesh.auto_smooth_angle = scene.smoothAngle / 180.0 * pi
            mesh.use_auto_smooth = True
        mesh.update()
        mesh.validate()
        meshes.append(create_node(nodeData, scene.scale, mesh))
        nodes[meshData.nodeIndex] = meshes[-1]

    #Link objects
    for i in range(len(nodes)):
        parentIndex = scene.nodes[i].parentIndex
        if nodes[i] and parentIndex >= 0 and nodes[parentIndex]:
            transform = nodes[i].matrix_world
            nodes[i].parent = nodes[parentIndex]
            nodes[i].matrix_world = transform

    #Create Blender armatures
    scale = mathutils.Matrix.Scale(scene.scale, 4)
    for skinData in scene.skins:
        mesh = None
        for i in range(len(scene.meshes)):
            if scene.meshes[i].nodeIndex == skinData.nodeIndex:
                mesh = meshes[i]
        defaultBoneLength = (mesh.matrix_world * (mathutils.Vector(mesh.bound_box[0]) -
                                                  mathutils.Vector(mesh.bound_box[6]))).length / 40 / scene.scale
        boneLengths = [defaultBoneLength] * len(scene.nodes)
        for i in skinData.boneIndices:
            nodeData = scene.nodes[i]
            if nodeData.parentIndex < 0:
                continue
            parentNodeData = scene.nodes[nodeData.parentIndex]
            relativePos = (parentNodeData.transform.inverted() * nodeData.transform).translation
            if relativePos.x / relativePos.length > 0.999:
                boneLengths[nodeData.parentIndex] = relativePos.x
        name = 'Armature_' + mesh.name
        armatureObject = bpy.data.objects.new(name, bpy.data.armatures.new(name))
        armatureObject.show_x_ray = True
        bpy.context.scene.objects.link(armatureObject)

        if bpy.context.mode != 'OBJECT':
            bpy.ops.object.mode_set(mode = 'OBJECT')
        bpy.ops.object.select_all(action='DESELECT')
        bpy.context.scene.objects.active = armatureObject
        armatureObject.select = True
        bpy.ops.object.mode_set(mode='EDIT')

        armature = mesh.modifiers.new("Armature", "ARMATURE")
        armature.object = armatureObject
        armature.use_vertex_groups = True
        mesh.parent = armatureObject
        vertexGroups = [None] * len(scene.nodes)
        for i in skinData.boneIndices:
            nodeData = scene.nodes[i]
            bone = armatureObject.data.edit_bones.new(as_string(nodeData.name))
            bone.head = scale * nodeData.transform.translation
            bone.tail = (scale * (nodeData.transform)) * mathutils.Vector((boneLengths[i], 0, 0))
            vertexGroups[i] = mesh.vertex_groups.new(as_string(scene.nodes[i].name))
            nodes[i] = bone
        for i in range(len(skinData.weightsPerVertex)):
            weights = skinData.weightsPerVertex[i]
            for boneIndex, weight in weights:
                vertexGroups[skinData.boneIndices[boneIndex]].add([i], weight, "REPLACE")
        for i in skinData.boneIndices:
            if scene.nodes[i].parentIndex >= 0:
                nodes[i].parent = nodes[scene.nodes[i].parentIndex]

        bpy.ops.object.mode_set(mode = 'OBJECT')
        bpy.ops.object.select_all(action='DESELECT')

    bpy.context.scene.update()


def load(operator, context, filePath='', scale=0.01, diffuseOnly=True):
    print('importing 3DB: %r' % (filePath))
    file = open(filePath, 'rb')
    scene = SceneData()
    scene.filePath = filePath
    scene.scale = scale
    scene.diffuseOnly = diffuseOnly

    if file.read(4) != b'3db\x00':
        print('\tFatal Error:  Not a valid 3db file: %r' % filePath)
        file.close()
        return
    scene.smoothAngle, chunkCount = unpack('<fI', [file.read(8)])

    for i in range(chunkCount):
        chunkID, minVersion, size = unpack('<III', [file.read(12)])
        if chunkID not in ChunkID.range or VERSION_3DB < minVersion:
            file.seek(size, 1)
            continue
        chunkData = [file.read(size)]
        if chunkID == ChunkID.material:
            read_material_chunk(chunkData, scene)
        elif chunkID == ChunkID.texmap:
            read_texmap_chunk(chunkData, scene)
        elif chunkID == ChunkID.node:
            read_node_chunk(chunkData, scene)
        elif chunkID == ChunkID.mesh:
            read_mesh_chunk(chunkData, scene)
        elif chunkID == ChunkID.skin:
            read_skin_chunk(chunkData, scene)

    build_scene(scene)
    bpy.context.scene.update()
    file.close()
    return {'FINISHED'}
