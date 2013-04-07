macroScript createExtraInfo category:"Extended Saving" tooltip:"Ignore Me"
(
	local memoryTotal = 0
	local iNode = 0
	local nodes = #()
	local materials = #()
	local materialChunkPattern = #( 1, 1, #material, "", false, 0, undefined, undefined, undefined )
	local nodeChunkPattern = #( 1, 1, #node, "", 0, 0, undefined, undefined, false )
	local meshChunkPattern = #( 1, 1, #mesh, 0, undefined, undefined, undefined, undefined, undefined, undefined, undefined, undefined )
	local skinChunkPattern = #( 1, 1, #skin, 0, undefined, undefined, undefined, undefined )
	local floatSize = 29
	local intSize = 29
	local point3Size = 37
	local colorSize = 41
	fn notEnoughMemory =
	(
		global _extendedSaveData = undefined
		global _extendedSaveMemoryTotal = memoryTotal
		return undefined
	)
	fn memoryCheck sizeIncr =
	(
		memoryTotal += sizeIncr
		return ( memoryTotal * 15 / 14 + 1600000 < heapSize )
	)
	fn getSkinBoneArray skinMod =
	(
		local numBones = skinOps.GetNumberBones skinMod
		local dependsArr = ( refs.dependsOn skinMod )
		local boneArr = for o in dependsArr where ( isKindOf o node ) collect o
		local boneNames = #()
		local boneArrFinal = #()
		boneNames.count = boneArr.count
		for i = boneArr.count to 1 by -1 do boneNames[ i ] = boneArr[ i ].name
		boneArrFinal.count = numBones
		for i = 1 to numBones do
		(
			local boneName = ( skinOps.GetBoneName skinMod i 1 )
			local boneIndex = ( findItem boneNames boneName )
			if boneIndex != 0 then boneArrFinal[ i ] = boneArr[ boneIndex ]
			else for o in nodes do if o.name == boneName then boneArrFinal[ i ] = o
		)
		return boneArrFinal
	)
	fn addChildrenToNodeArray parentNode =
	(
		for c in parentNode.children do
		(
			nodes[ iNode += 1 ] = c
			addChildrenToNodeArray c
		)
	)
	fn setUpNodesArray =
	(
		nodes.count = objects.count
		iNode = 0
		addChildrenToNodeArray rootNode
		nodes.count = iNode
	)
	fn setUpMaterialArray =
	(
		local i = 0
		materials.count = nodes.count
		for n in nodes do
		(
			if n.material != undefined then
				if ( findItem materials n.material ) == 0 then materials[ i += 1 ] = n.material
		)
		materials.count = i
	)
	fn buildMaterialChunk mt =
	(
		local result = copy materialChunkPattern #noMap
		local isMultiMaterial = ( ( classOf mt ) == Multimaterial )
		local i = 6
		result[ 4 ] = mt.name
		result[ 5 ] = isMultiMaterial
		if isMultiMaterial == true then
		(
			result.count = i + mt.materialList.count
			for m in mt.materialList do result[ i += 1 ] = ( buildMaterialChunk m )
		)
		else
		(
			try ( if mt.diffuseMap != undefined and ( classOf mt.diffuseMap ) == Bitmaptexture then result[ i += 1 ] = #( "diffuse", mt.diffuseMap.filename ) ) catch ()
			try ( if mt.specularMap != undefined and ( classOf mt.specularMap ) == Bitmaptexture then result[ i += 1 ] = #( "specular", mt.specularMap.filename ) ) catch ()
			try ( if mt.bumpMap != undefined and ( classOf mt.bumpMap ) == Bitmaptexture then result[ i += 1 ] = #( "bump", mt.bumpMap.filename ) ) catch ()
			result.count = i
		)
		result[ 6 ] = i - 6
		return result
	)
	fn buildNodeChunk nd =
	(
		local result = copy nodeChunkPattern #noMap
		result[ 4 ] = nd.name
		result[ 5 ] = ( findItem nodes nd.parent )
		result[ 6 ] = ( findItem materials nd.material )
		result[ 7 ] = nd.position
		result[ 8 ] = nd.transform
		result[ 9 ] = ( ( classOf nd ) == Bone ) or ( ( classOf nd ) == BoneGeometry )
		return result
	)
	fn buildMeshChunk nd =
	(
		local ndClass = ( classOf nd )
		if ( ndClass != Editable_mesh ) and ( ndClass != Editable_Poly ) and ( ndClass != Editable_Patch ) then throw ""
		local result = copy meshChunkPattern #noMap
		local m = snapshotAsMesh nd
		if ( meshop.getMapSupport m 0 ) then meshop.deleteIsoMapVertsAll m
		local verts = #()
		local faces = #()
		local faceMatIds = #()
		local faceSgs = #()
		local tVerts = #()
		local tVFaces = #()
		local vColors = #()
		local vCFaces = #()
		local numVerts = m.numverts
		local numFaces = m.numfaces
		local numTVerts = m.numtverts
		local numVColors = m.numcpvverts
		memoryCheck ( point3Size * numTVerts )
		if numTVerts != 0 then memoryCheck ( point3Size * numFaces )
		memoryCheck ( colorSize * numVColors )
		if numVColors != 0 then memoryCheck ( point3Size * numFaces )
		memoryCheck ( ( point3Size + intSize + intSize ) * numFaces )
		if ( memoryCheck ( point3Size * numVerts ) ) == false then return 0
		verts.count = numVerts;
		for i = 1 to numVerts do verts[ i ] = ( getVert m i )
		faces.count = numFaces
		faceMatIds.count = numFaces
		faceSgs.count = numFaces
		for i = 1 to numFaces do
		(
			faces[ i ] = ( getFace m i )
			faceMatIds[ i ] = ( getFaceMatID m i )
			faceSgs[ i ] = ( getFaceSmoothGroup m i )
		)
		if numTVerts != 0 then
		(
			tVerts.count = numTVerts
			for i = 1 to numTVerts do tVerts[ i ] = ( getTVert m i )
			tVFaces.count = numFaces
			for i = 1 to numFaces do tVFaces[ i ] = ( getTVFace m i )
		)
		if numVColors != 0 then
		(
			vColors.count = numVColors
			for i = 1 to numVColors do vColors[ i ] = ( getVertColor m i )
			if ( ( meshop.getMapSupport m -2 ) and ( meshop.getNumMapVerts m -2 ) == numVColors ) then for i = 1 to numVColors do vColors[ i ].alpha = 255 * ( meshop.getMapVert m -2 i ).x
			vCFaces.count = numFaces
			for i = 1 to numFaces do vCFaces[ i ] = ( getVCFace m i )
		)
		result[ 4 ] = ( findItem nodes nd )
		result[ 5 ] = verts
		result[ 6 ] = faces
		result[ 7 ] = faceMatIds
		result[ 8 ] = faceSgs
		result[ 9 ] = tVerts
		result[ 10 ] = tVFaces
		result[ 11 ] = vColors
		result[ 12 ] = vCFaces
		return result
	)
	fn buildSkinChunk nd =
	(
		if nd.modifiers[ 1 ] != nd.skin then
		(
			subObjectLevel = 0
			if panelTaskMode == #modify then
				deselect nd
			local tempNode = copy nd
			deleteModifier tempNode tempNode.skin
			for i = nd.modifiers.count to 1 by -1 do if ( classOf nd.modifiers[ i ] ) != Skin then deleteModifier nd i
			referenceReplace nd tempNode
			delete tempNode
		)
		local s = nd.modifiers[ 1 ]
		setCommandPanelTaskMode mode:#modify
		modPanel.setCurrentObject s
		local result = copy skinChunkPattern #noMap
		local skinBones = getSkinBoneArray s
		local numBones = skinBones.count
		local nodeIndexPerBone = #()
		local vertexIndices = #()
		local boneIndices = #()
		local weights = #()
		local numVerts = skinOps.GetNumberVertices s
		local numWeights = 0
		local numWeightsSoFar = 0
		for i = 1 to numVerts do numWeights += ( skinOps.GetVertexWeightCount s i )
		memoryCheck ( ( floatSize + intSize + intSize ) * numWeights )
		if ( memoryCheck ( intSize * numBones ) ) == false then return 0
		nodeIndexPerBone.count = numBones
		for i = 1 to numBones do nodeIndexPerBone[ i ] = ( findItem nodes skinBones[ i ] )
		vertexIndices.count = numWeights
		boneIndices.count = numWeights
		weights.count = numWeights
		for i = 1 to numVerts do
		(
			local numWeightsPerVert = skinOps.GetVertexWeightCount s i
			for j = 1 to numWeightsPerVert do
			(
				numWeightsSoFar += 1
				vertexIndices[ numWeightsSoFar ] = i
				boneIndices[ numWeightsSoFar ] = skinOps.GetVertexWeightBoneID s i j
				weights[ numWeightsSoFar ] = skinOps.GetVertexWeight s i j
			)
		)
		result[ 4 ] = ( findItem nodes nd )
		result[ 5 ] = nodeIndexPerBone
		result[ 6 ] = vertexIndices
		result[ 7 ] = boneIndices
		result[ 8 ] = weights
		return result
	)
	on execute do
	(
		local sel = getCurrentSelection()
		local panelTaskMode = getCommandPanelTaskMode()
		disableSceneRedraw()
		undo off
		(
			in coordsys world
			(
				memoryTotal = 0
				nodes = #()
				materials = #()
				setUpNodesArray()
				setUpMaterialArray()
				global _extendedSaveData = #()
				_extendedSaveData.count = materials.count + nodes.count * 3
				local i = 0
				for m in materials do _extendedSaveData[ i += 1 ] = ( buildMaterialChunk m )
				for n in nodes do _extendedSaveData[ i += 1 ] = ( buildNodeChunk n )
				for n in nodes do try ( _extendedSaveData[ i += 1 ] = ( buildMeshChunk n ) ) catch ()
				for n in nodes do try ( _extendedSaveData[ i += 1 ] = ( buildSkinChunk n ) ) catch ()
				if ( memoryCheck 0 ) == false then return ( notEnoughMemory() )
				_extendedSaveData.count = i
			)
		)
		setCommandPanelTaskMode mode:panelTaskMode
		select sel
		enableSceneRedraw()
	)
) 