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

#Author:  Baltram
#Version: 1.1

import ntpath
import struct

import bpy
import mathutils

from functools import reduce
from math import pi, sqrt


VERSION_3DB = 1


class ChunkID:
    range = range(1, 6)
    material = 1
    texmap = 2
    node = 3
    mesh = 4
    skin = 5

minVersionPerChunkID = (-1, 1, 1, 1, 1, 1)

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


def as_index(i):
    return i + 1


def b_name(bone, armature):  # Create unique bone name
    return (bone if type(bone) == type('') else bone.name) + "##" + armature.name


def scaleless_matrix(m):
    pos, rot, scale = m.decompose()
    return mathutils.Matrix.Translation(pos) @ rot.to_matrix().to_4x4()


def write_string(file, s):
    s = s.encode('ascii', 'replace')
    file.write(struct.pack('<%usb' % len(s), s, 0))


def unselect_all():
    if bpy.context.mode != 'OBJECT':
        bpy.ops.object.mode_set(mode='OBJECT')
    bpy.ops.object.select_all(action='DESELECT')


def select_edit(o):
    unselect_all()
    bpy.context.view_layer.objects.active = o
    o.select_set(True)
    bpy.ops.object.mode_set(mode='EDIT')


def collect_objects(objects, nodes, nodeIndexPerName, materialObjects, meshes, armatures, scaleMatrix):
    dg = bpy.context.evaluated_depsgraph_get()
    for o in objects:
        nodeIndex = len(nodes)
        meshIndex = len(meshes)
        if o.type == 'MESH':
            nodes.append([o.name, NodeType.mesh, scaleless_matrix(scaleMatrix @ o.matrix_world), o.parent.name if o.parent else '', -1])
            nodeIndexPerName[o.name] = nodeIndex
            m = o.evaluated_get(dg).to_mesh(preserve_all_data_layers=True, depsgraph=dg).copy()
            m.transform(scaleMatrix @ o.matrix_world)
            materialObjects.update({mat.name:mat for mat in o.data.materials if mat})
            meshes.append([m, nodeIndex, [mat.name for mat in o.data.materials if mat]])
            for mod in o.modifiers:
                if mod.type == 'ARMATURE' and mod.object in objects:
                    select_edit(mod.object)
                    if mod.use_vertex_groups and len(o.data.vertices) == len(m.vertices) and len(mod.object.data.edit_bones) > 0:
                        armatures.append((mod.object, nodeIndex, o))
        elif o.type == 'EMPTY':
            nodes.append([o.name, NodeType.other, scaleless_matrix(scaleMatrix @ o.matrix_world), o.parent.name if o.parent else '', -1])
            nodeIndexPerName[o.name] = nodeIndex
        elif o.type == 'ARMATURE':
            select_edit(o)
            # In Blender, bones point to the front (+Y in Blender coordinates) whereas in Genome/3db they point to the right (+X in Blender coordinates)
            boneOrientationFix = mathutils.Matrix(((0,1,0,0),(-1,0,0,0),(0,0,1,0),(0,0,0,1))) # rotation by 270 degrees around the Z-axis
            for b in o.data.edit_bones:
                nodes.append([b.name, NodeType.bone, scaleless_matrix(scaleMatrix @ o.matrix_world @ b.matrix @ boneOrientationFix.inverted()), b_name(b.parent.name, o) if b.parent else '', -1])
                nodeIndexPerName[b_name(b.name, o)] = nodeIndex
                nodeIndex += 1
    for node in nodes:
        node[3] = nodeIndexPerName[node[3]] if node[3] in nodeIndexPerName else -1


def collect_materials(materials, materialObjects, meshes, nodes):
    globalMats = []
    globalMatInidces = {}
    usage = {name:0 for name in materialObjects}
    for m in meshes:
        for name in m[2]:
            usage[name] += 1
    for m in meshes:
        if [usage[name] for name in m[2]].count(1) < len(m[2]):  # Some materials not used exclusively by this mesh
            for name in m[2]:
                i = len(globalMats)
                if i == globalMatInidces.setdefault(name, i):
                    globalMats.append(materialObjects[name])
            m[2] = [globalMatInidces[name] for name in m[2]]
        elif len(m[2]) > 0:
            nodes[m[1]][4] = len(materials)
            materials.append(('mats_' + nodes[m[1]][0], [materialObjects[name] for name in m[2]]))
            m[2] = range(len(m[2]))
    for m in meshes:
        if len(m[2]) > 0 and nodes[m[1]][4] < 0:
            nodes[m[1]][4] = len(materials)
    if len(globalMats) > 0:
        materials.append(('mats_global', globalMats))


chunkOffset = 0
chunkCount = 0

def begin_chunk(file, chunkID):
    file.write(struct.pack('<3I', chunkID, minVersionPerChunkID[chunkID], 0))
    global chunkOffset
    chunkOffset = file.tell()


def end_chunk(file):
    offset = file.tell()
    file.seek(chunkOffset - 4)
    file.write(struct.pack('<I', offset - chunkOffset))
    file.seek(offset)
    global chunkCount
    chunkCount += 1


def write_texmap_chunk(file, image_name, type, matIndex, subIndex):
    if type == -1:
        return
    begin_chunk(file, ChunkID.texmap)
    file.write(struct.pack('<III', as_index(matIndex), as_index(subIndex), type))
    write_string(file, image_name)
    end_chunk(file)


def write_material_chunk(file, material, matIndex, isSub=False, subIndex=-1):
    begin_chunk(file, ChunkID.material)
    subMaterials = material[1]
    if len(subMaterials) > 1:
        write_string(file, material[0])
        file.write(struct.pack('<I', MaterialType.multi))
        end_chunk(file)
        for i, sub in enumerate(subMaterials):
            write_material_chunk(file, ('', [sub]), matIndex, True, i)
    else:
        write_string(file, subMaterials[0].name)
        file.write(struct.pack('<I', MaterialType.sub if isSub else MaterialType.standard))
        if isSub:
            file.write(struct.pack('<I', as_index(matIndex)))
        end_chunk(file)
        texture_nodes = [node for node in subMaterials[0].node_tree.nodes if node.type == 'TEX_IMAGE'] if subMaterials[0].node_tree else []
        for node in texture_nodes:
            for link in node.outputs[0].links:
                if link.to_socket.name == 'Base Color':
                    write_texmap_chunk(file, node.image.name, MapType.diffuse, matIndex, subIndex)
                elif link.to_socket.name == 'Normal':
                    write_texmap_chunk(file, node.image.name, MapType.normal, matIndex, subIndex)
                elif link.to_socket.name == 'Specular':
                    write_texmap_chunk(file, node.image.name, MapType.specular, matIndex, subIndex)


def write_node_chunk(file, node):
    name, type, matrix, parentIndex, materialIndex = node
    begin_chunk(file, ChunkID.node)
    file.write(struct.pack('<I', type))
    write_string(file, name)
    file.write(struct.pack('<II', as_index(parentIndex), as_index(materialIndex)))
    file.write(struct.pack('<16f', *[f for v in matrix.transposed() for f in v]))
    end_chunk(file)


def write_mesh_chunk(file, mesh):
    mesh, nodeIndex, tmp = mesh

    # Split polygons into triangles
    faceCount = 0
    loopPerVertex = [0] * len(mesh.vertices)
    for p in mesh.polygons:
        faceCount += (len(p.vertices) - 2)
    faceMatIDs = [0] * faceCount
    faceVertIndices = [0] * (faceCount * 3)
    faceLoopIndices = [0] * (faceCount * 3)
    i = 0
    for p in mesh.polygons:
        verts = p.vertices
        loops = p.loop_indices
        for j in range(1, len(verts) - 1):
            faceVertIndices[i]     = verts[0]
            faceLoopIndices[i]     = loops[0]
            faceVertIndices[i + 1] = verts[j]
            faceLoopIndices[i + 1] = loops[j]
            faceVertIndices[i + 2] = verts[j + 1]
            faceLoopIndices[i + 2] = loops[j + 1]
            faceMatIDs[i // 3] = p.material_index
            i += 3
        for v, l in zip(verts, loops):
            loopPerVertex[v] = l

    # Collect unique TVerts
    def round_uv(uv):
        return round(uv[0], 6), round(uv[1], 6)
    tverts = []
    faceTVertIndices = [0] * (faceCount * 3)
    layer = mesh.uv_layers.active
    if layer:
        tvertIndicesPerUV = {}
        uniqueUVCount = 0
        uvPerLoop = layer.data
        for i, l in enumerate(faceLoopIndices):
            uv = uvPerLoop[l].uv
            index = tvertIndicesPerUV.get(round_uv(uv))
            if index == None:
                index = tvertIndicesPerUV[round_uv(uv)] = uniqueUVCount
                tverts.append(((uv[0]), (uv[1]), 0.0))
                uniqueUVCount += 1
            faceTVertIndices[i] = index

    # Find color vertex channels:
    channelCount = len(mesh.vertex_colors)
    rgbChannel = 0 if 0 < channelCount else None
    alphaChannel = 1 if 1 < channelCount else None
    for i, channel in enumerate(mesh.vertex_colors):
        if channel.name.lower() == 'rgb':
            rgbChannel = i
            if alphaChannel == i:
                alphaChannel = 0
        if channel.name.lower() == 'alpha':
            alphaChannel = i
            if rgbChannel == i:
                rgbChannel = 1 if 1 < channelCount else None

    # Compose vertex colors:
    hasVertexColors = rgbChannel != None or alphaChannel != None
    vertexColors = [255] * (len(mesh.vertices) * 4 if hasVertexColors else 0)
    if rgbChannel != None:
        for i, l in enumerate(loopPerVertex):
            color = mesh.vertex_colors[rgbChannel].data[l].color
            for j in range(3):
                vertexColors[i * 4 + j] = round(color[j] * 255)
    if alphaChannel != None:
        for i, l in enumerate(loopPerVertex):
            color = mesh.vertex_colors[alphaChannel].data[l].color
            vertexColors[i * 4 + 3] = round((color[0] + color[1] + color[2]) / 3 * 255)

    begin_chunk(file, ChunkID.mesh)
    file.write(struct.pack('<I', as_index(nodeIndex)))
    file.write(struct.pack('<I', len(mesh.vertices)))

    for v in mesh.vertices:
       file.write(struct.pack('<fff', *(f for f in v.co)))
    file.write(struct.pack('<I', faceCount))
    for i in range(faceCount):
        j = i * 3
        file.write(struct.pack('<IIIII', faceVertIndices[j] + 1, faceVertIndices[j + 1] + 1, faceVertIndices[j + 2] + 1, as_index(faceMatIDs[i]), 0))

    file.write(struct.pack('<I', len(tverts)))
    if len(tverts):
        for uvw in tverts:
            file.write(struct.pack('<fff', *uvw))
        for i in faceTVertIndices:
            file.write(struct.pack('<I', i + 1))

    file.write(struct.pack('<I', len(mesh.vertices) if hasVertexColors else 0))
    for c in vertexColors:
        file.write(struct.pack('<B', c))

    end_chunk(file)


def write_skin_chunk(file, armature, nodeIndexPerName):
    armature, nodeIndex, node = armature
    select_edit(armature)
    bonePerGroup = [None] * len(node.vertex_groups)  # Bone index per vertex group index
    nodePerBone = []                                 # Node index per bone index
    for i, group in enumerate(node.vertex_groups):
        if b_name(group.name, armature) in nodeIndexPerName:
            bonePerGroup[i] = len(nodePerBone)
            nodePerBone.append(nodeIndexPerName[b_name(group.name, armature)])
    if len(nodePerBone) == 0:
        nodePerBone.append(nodeIndexPerName[b_name(armature.data.edit_bones[0].name, armature)])
    weightsPerVertex = ([(bonePerGroup[g.group], g.weight) for g in v.groups if bonePerGroup[g.group] != None] for v in node.data.vertices)

    begin_chunk(file, ChunkID.skin)
    file.write(struct.pack('<II', as_index(nodeIndex), len(nodePerBone)))
    for index in nodePerBone:
        file.write(struct.pack('<I', as_index(index)))
    file.write(struct.pack('<I', len(node.data.vertices)))
    for weights in weightsPerVertex:
        total = reduce(lambda x,y:(None, x[1] + y[1]), weights, (None, 0.0))[1]
        weights = [(t[0], t[1] / total) for t in weights] if total else [(0, 1.0)]  # Make sure the total of weights is 1.0 and each vert has at least one weight
        file.write(struct.pack('<I', len(weights)))
        for t in weights:
            file.write(struct.pack('<If', as_index(t[0]), t[1]))
    end_chunk(file)


def save(operator, context, filePath='', scale=100.0, useSelection=False):

    print('exporting 3DB: %r' % (filePath))
    scene = bpy.context.scene
    scaleMatrix = mathutils.Matrix.Scale(scale, 4)

    if bpy.context.mode != 'OBJECT':
        bpy.ops.object.mode_set(mode='OBJECT')
    if useSelection:
        objects = [ob for ob in scene.objects if ob.visible_get() and ob.select_get()]
    else:
        objects = [ob for ob in scene.objects if ob.visible_get()]

    nodeIndexPerName = {} # name:index in nodes array
    nodes = []            # [name, type, matrix, parentIndex, materialIndex]
    materialObjects = {}  # name:material object
    meshes = []           # [mesh, node index, matnamelist]
    armatures = []        # (armature object, node index, node)
    materials = []        # (name, materialObjects)

    collect_objects(objects, nodes, nodeIndexPerName, materialObjects, meshes, armatures, scaleMatrix)
    collect_materials(materials, materialObjects, meshes, nodes)

    file = open(filePath, 'w+b')
    write_string(file, '3db')
    file.write(struct.pack('<fI', -1.0, 0))

    global chunkCount
    chunkCount = 0
    for i, mat in enumerate(materials):
        write_material_chunk(file, mat, i)
    for node in nodes:
        write_node_chunk(file, node)
    for mesh in meshes:
        write_mesh_chunk(file, mesh)
    for armature in armatures:
        write_skin_chunk(file, armature, nodeIndexPerName)

    unselect_all()
    file.seek(8)
    file.write(struct.pack('<I', chunkCount))
    file.close()

    return {'FINISHED'}