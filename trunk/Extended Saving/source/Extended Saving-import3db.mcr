macroScript import3db category:"Extended Saving" tooltip:"Import 3db file..."
(
	local version3db = 1
	local file = undefined
	local nodes = #()
	local materials = #()
	local bonesCreated = false
	local smoothAngle
	local norm
	local boneMat = StandardMaterial name:"material_bones" opacity:30
	fn tell =
	(
		( ftell file )
	)
	fn skip count =
	(
		fseek file count #seek_cur
	)
	fn seek_ pos =
	(
		fseek file pos #seek_set
	)
	fn rFloat =
	(
		( readFloat file )
	)
	fn rU8 =
	(
		( readByte file #unsigned )
	)
	fn rI32 =
	(
		( readLong file )
	)			
	fn rString =
	(
		( readString file )
	)
	fn createBone n =
	(
		if ( classOf( n ) ) != Bone then
			return()
		createBone n.parent
		b = ( bonesys.createBone [ 0, 0, 0 ] [ norm, 0, 1e-6 ] [ 0, 0, 1 ] )
		b.name = n.name
		b.material = boneMat
		b.transform = n.transform
		b.parent = n.parent
		for c in n.children do
			c.parent = b
		nodes[ findItem nodes n ] = b
		delete n
	)
	fn createBones =
	(
		if bonesCreated == true then
			return()
		select nodes
		norm = ( length ( selection.min - selection.max ) ) / 60
		clearSelection()
		for n in nodes do
			createBone n
		for b in nodes do
		(
			if classOf( b ) != BoneGeometry then
				continue
			if b.children.count == 0 then
				b.length = 1e-6
			local l = if b.length < norm / 50 then ( if b.parent != undefined then ( if b.parent.length > 4 * norm then b.parent.length / 2 + 2 * norm else b.parent.length ) else 4 * norm ) else b.length
			b.width = b.height = 1.5 / ( 0.5 / norm + 4 / l )
			if b.length < b.width then
				b.length = b.width
		)
		bonesCreated = true
	)
	fn readMaterialChunk =
	(
		local matName = rString()
		local matType = rI32()
		case matType of
		(
			1: append materials ( standard name:matName showInViewport:true )
			2: append materials ( multimaterial  name:matName numsubs:0 )
			3: 
			(
				local multiMat = materials[ rI32() ]
				multiMat.materialList[ multiMat.materialList.count + 1 ] = ( standard name:matName showInViewport:true )
			)
		)
	)
	fn readTexMapChunk =
	(
		local matIndex = rI32()
		local subMatIndex = rI32()
		local mapType = rI32()
		local mapPath = rString()
		local destMaterial = materials[ matIndex ]
		if subMatIndex != 0 then
			destMaterial = destMaterial[ subMatIndex ]
		destMaterial.maps[ mapType ] = ( Bitmaptexture fileName:mapPath )
	)
	fn readNodeChunk =
	(
		local nodeDest
		local nodeType = rI32()
		local nodeName = rString()
		case nodeType of
		(
			1: nodeDest = ( Mesh name:nodeName )
			2: nodeDest = ( Bone name:nodeName )
			3: nodeDest = ( Dummy name:nodeName )
		)
		local parentIndex = rI32()
		local materialIndex = rI32()
		if parentIndex != 0 then
			nodeDest.parent = nodes[ parentIndex ]
		if ( materialIndex != 0 ) and ( classof( nodeDest ) == Editable_mesh ) then
			nodeDest.material = materials[ materialIndex ]
		local row1 = [ rFloat(), rFloat(), rFloat() ]
		skip 4
		local row2 = [ rFloat(), rFloat(), rFloat() ]
		skip 4
		local row3 = [ rFloat(), rFloat(), rFloat() ]
		skip 4
		local row4 = [ rFloat(), rFloat(), rFloat() ]
		nodeDest.transform = ( matrix3 row1 row2 row3 row4 )
		append nodes nodeDest
	)
	fn readMeshChunk =
	(
		local nodeDest = nodes[ rI32() ]
		local meshDest = TriMesh()
		local numVerts = rI32()
		setNumVerts meshDest numVerts
		for i = 1 to numVerts do
			setVert meshDest i [ rFloat(), rFloat(), rFloat() ]
		local numFaces = rI32()
		setNumFaces meshDest numFaces
		for i = 1 to numFaces do
		(
			setFace meshDest i [ rI32(), rI32(), rI32() ]
			setFaceMatID meshDest i ( rI32() )
			setFaceSmoothGroup meshDest i ( rI32() )
		)
		local numTVerts = rI32()
		if ( numTVerts != 0 ) then
		(
			setNumTVerts meshDest numTVerts
			for i = 1 to numTVerts do
				setTVert meshDest i [ rFloat(), rFloat(), rFloat() ]
			buildTVFaces meshDest false
			for i = 1 to numFaces do
				setTVFace meshDest i [ rI32(), rI32(), rI32() ]
		)
		local numVColors = rI32()
		if ( numVColors != 0 ) then
		(
			defaultVCFaces meshDest
			for i = 1 to numVColors do
			(
				local c = [ rU8(), rU8(), rU8() ] as color
				c.a = rU8()
				setVertColor meshDest i c
			)
		)
		transform meshDest ( inverse nodeDest.transform )
		if smoothAngle != -1.0 then
			meshop.autoSmooth meshDest meshDest.faces smoothAngle
		nodeDest.mesh = meshDest
	)
	fn readSkinChunk =
	(
		createBones()
		max modify mode
		local boneIDs = #()
		local weights = #()
		local n = nodes[ rI32() ]
		local s = Skin()
		addModifier n s
		modPanel.setCurrentObject s
		local numBones = rI32()
		for i = 1 to numBones do
			skinOps.addBone s nodes[ rI32() ] 0
		local numVerts = rI32()
		enableSceneRedraw()
		completeRedraw()
		disableSceneRedraw()
		for i = 1 to numVerts do
		(
			local numWeights = rI32()
			boneIDs.count = weights.count = numWeights
			for j = 1 to numWeights do
			(
				boneIDs[ j ] = rI32()
				weights[ j ] = rFloat()
			)
			skinOps.replaceVertexWeights s i boneIDs weights
		)
		max create mode
	)
	on execute do
	(
		nodes = #()
		materials = #()
		bonesCreated = false
		local fileName3db = getOpenFileName caption:"Import 3db" types:"Baltram's 3D format (*.3db)|*.3db|All Files (*.*)|*.*|"
		if ( fileName3db == undefined ) then
			return()
		try
		(
			file = fopen fileName3db "rb"
		)
		catch
		(
			messagebox "Error opening file." title:"Extended Saving"
			return()
		)
		if ( rString() != "3db" ) then
			return ( messagebox "Invalid 3db file." title:"Extended Saving" )
		smoothAngle = rFloat()
		local nChunks = rI32()
		local endChunkOffset = tell()
		disableSceneRedraw()
		try
		(
			undo off
			(
				for i = 1 to nChunks do
				(
					local chunkId = rI32()
					local minVersion = rI32()
					endChunkOffset = endChunkOffset + rI32() + 12
					if ( version3db >= minVersion ) then
					(
						case chunkId of
						(
							1: readMaterialChunk()
							2: readTexMapChunk()
							3: readNodeChunk()
							4: readMeshChunk()
							5: readSkinChunk()
						)
					)
					seek_ endChunkOffset
				)	
			)
		)
		catch
		(
			messagebox "Unexpected error when importig 3db data." title:"Extended Saving"
		)
		enableSceneRedraw()
		fclose file
	)
) 