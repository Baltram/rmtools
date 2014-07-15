----------------------------------------------
--										    --
-- 3DB Import/Export tools v1.0 for 3ds Max --
--   by Baltram (baltram-lielb@web.de)	    --
--										    --
----------------------------------------------

global lang3db = if ( ( menuman.findmenu "&Hilfe" ) != undefined ) or ( ( menuman.findmenu "Hilfe" ) != undefined ) then 1 else 0

global loc3db_3dbExport  = if lang3db == 0 then "3db Export"
                                           else "3db-Export"
global loc3db_3dbImport  = if lang3db == 0 then "3db Import"
                                           else "3db-Import"
global loc3db_3dbTools   = if lang3db == 0 then "3db Tools v1.0 for 3ds Max"
                                           else "3db Tools v1.0 für 3ds Max"
global loc3db_About      = if lang3db == 0 then "About"
                                           else "Über"
global loc3db_About_     = if lang3db == 0 then "About..."
                                           else "Über..."
global loc3db_Replace    = if lang3db == 0 then "Completely replace current scene?"
                                           else "Aktuelle Szene komplett ersetzen?"
global loc3db_CreateErr  = if lang3db == 0 then "Error creating file."
                                           else "Fehler beim Erstellen der Datei"
global loc3db_OpenErr    = if lang3db == 0 then "Error opening file."
                                           else "Fehler beim Öffnen der Datei"
global loc3db_Export3db  = if lang3db == 0 then "Export 3db"
                                           else "3db exportieren"
global loc3db_Export3db_ = if lang3db == 0 then "Export 3db..."
                                           else "3db exportieren..."
global loc3db_HideMenu   = if lang3db == 0 then "Hide this menu"
                                           else "Dieses Menü verbergen"
global loc3db_Import3db  = if lang3db == 0 then "Import 3db"
                                           else "3db importieren"
global loc3db_Import3db_ = if lang3db == 0 then "Import 3db..."
                                           else "3db importieren..."
global loc3db_HideGuide  = if lang3db == 0 then "The setting for hiding the menu is located in the '3db Tools' utility\nwhich can be shown by clicking on '3db Tools' in the dropdown list\nunder 'MaxScript' in the utilities tab (command panel, hammer symbol)."
                                           else "Die Einstellung zum Verbergen des Menüs kann im '3db Tools'-Utility\nvorgenommen werden. Dieses öffnet sich nach einem Klick auf \n'3db Tools' in der Dropdown-Liste unter 'MaxScript' im Utilities-Tab\n(Hammer-Symbol)."
global loc3db_FormatErr  = if lang3db == 0 then "Invalid 3db file."
                                           else "Ungültige 3db-Datei."
global loc3db_Date       = if lang3db == 0 then "July 15th 2014"
                                           else "15. Juli 2014"
global loc3db_ShowMenu   = if lang3db == 0 then "Show '3db' menu"
                                           else "'3db'-Menü anzeigen"
global loc3db_ExportErr  = if lang3db == 0 then "Unexpected error when exporting 3db data."
                                           else "Unerwarteter Fehler beim 3db-Export"
global loc3db_ImportErr  = if lang3db == 0 then "Unexpected error when importing 3db data."
                                           else "Unerwarteter Fehler beim 3db-Import"

macroScript export3db category:"3db Tools" tooltip:"Export 3db..."
(
	local version3db = 1
	local file = undefined
	local nodes = #()
	local materials = #()
	local chunkOffset = 0
	local chunkCount = 0
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
	fn wFloat f =
	(
		( writeFloat file f )
	)
	fn wU8 u =
	(
		( writeByte file u #unsigned )
	)
	fn wI32 i =
	(
		( writeLong file i )
	)			
	fn wString s =
	(
		( writeString file s )
	)
	fn isMesh node =
	(
		local class = ( classOf node )
		return if class == Editable_mesh or class == Editable_Poly or class == Editable_Patch or class == PolyMeshObject then true else false
	)
	fn addChildrenToNodeArray parentNode =
	(
		for c in parentNode.children do
		(
			append nodes c
			addChildrenToNodeArray c
		)
	)
	fn setUpNodesArray =
	(
		nodes = #()
		addChildrenToNodeArray rootNode
	)
	fn setUpMaterialArray =
	(
		materials = #()
		for n in nodes do
			if n.material != undefined then
				if ( findItem materials n.material ) == 0 then
					append materials n.material
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
			if boneIndex != 0 then
				boneArrFinal[ i ] = boneArr[ boneIndex ]
			else for o in nodes do
				if o.name == boneName then
					boneArrFinal[ i ] = o
		)
		return boneArrFinal
	)
	fn beginChunk type =
	(
		wI32 type
		wI32 1 -- minimum 3db version
		wI32 0 -- size
		chunkOffset = tell()
	)
	fn endChunk =
	(
		local endOffset = tell()
		seek_ ( chunkOffset - 4 )
		wI32 ( endOffset - chunkOffset )
		seek_ endOffset
		chunkCount += 1
	)
	fn writeTexMapChunk map type iMat iSub =
	(
		beginChunk 2
		wI32 iMat
		wI32 iSub
		wI32 type
		wString map.filename
		endChunk()
	)
	fn writeMaterialChunk mat iMat iSub =
	(
		beginChunk 1
		wString mat.name
		local isMulti = ( classOf mat ) == Multimaterial
		wI32 ( if iSub != 0 then 3 else ( if isMulti then 2 else 1 ) )
		if iSub != 0 then
			wI32 iMat
		endChunk()
		if isMulti and iSub == 0 then
		(
			for i = 1 to ( getNumSubMtls mat ) do
				writeMaterialChunk ( getSubMtl mat i ) iMat i
		)
		else if not isMulti then
		(
			try ( if mat.diffuseMap != undefined and ( classOf mat.diffuseMap ) == Bitmaptexture then writeTexMapChunk mat.diffuseMap 2 iMat iSub ) catch ()
			try ( if mat.specularMap != undefined and ( classOf mat.specularMap ) == Bitmaptexture then writeTexMapChunk mat.specularMap 3 iMat iSub ) catch ()
			try ( if mat.bumpMap != undefined and ( classOf mat.bumpMap ) == Bitmaptexture then writeTexMapChunk mat.bumpMap 9 iMat iSub ) catch ()
		)
	)
	fn writeNodeChunk node iParent iMat =
	(
		beginChunk 3
		local class = ( classOf node )
		local type = if class == Bone or class == BoneGeometry then 2 else ( if ( isMesh node ) then 1 else 3 )
		wI32 type
		wString node.name
		wI32 iParent
		wI32 iMat
		local t = node.transform
		for i = 1 to 4 do
		(
			for j = 1 to 3 do
				wFloat t[ i ][ j ]
			wFloat ( if i == 4 then 1.0 else 0.0 )
		)
		endChunk()
	)
	fn writeMeshChunk node nodeIndex =
	(
		local m = snapshotAsMesh node
		if ( meshop.getMapSupport m 0 ) then meshop.deleteIsoMapVertsAll m
		local numVerts = m.numverts
		local numFaces = m.numfaces
		local numTVerts = m.numtverts
		local numVColors = m.numcpvverts
		local vColors = #()
		local vColorIndices = #()
		if numVColors != 0 then
		(
			vColors.count = numVColors
			for i = 1 to numVColors do
				vColors[ i ] = ( getVertColor m i )
			if ( ( meshop.getMapSupport m -2 ) and ( meshop.getNumMapVerts m -2 ) == numVColors ) then
				for i = 1 to numVColors do
					vColors[ i ].alpha = 255 * ( meshop.getMapVert m -2 i ).x
			vColorIndices.count = numVerts
			for i = 1 to numFaces do
			(
				cf = ( getVCFace m i )
				f = ( getFace m i )
				for j = 1 to 3 do
					vColorIndices[ f[ j ] ] = cf[ j ]
			)
		)
		beginChunk 4
		wI32 nodeIndex
		wI32 numVerts
		for i = 1 to numVerts do
		(
			local v = getVert m i
			for j = 1 to 3 do
				wFloat v[ j ]
		)
		wI32 numFaces
		for i = 1 to numFaces do
		(
			local f = ( getFace m i )
			for j = 1 to 3 do
				wI32 f[ j ]
			wI32 ( getFaceMatID m i )
			wI32 ( getFaceSmoothGroup m i )
		)
		wI32 numTVerts
		if numTVerts != 0 then
		(
			for i = 1 to numTVerts do
			(
				local t = ( getTVert m i )
				for j = 1 to 3 do
					wFloat t[ j ]
			)
			for i = 1 to numFaces do
			(
				local tf = ( getTVFace m i )
				for j = 1 to 3 do
					wI32 tf[ j ]
			)
		)
		if numVColors != 0 then
		(
			wI32 numVerts
			for i = 1 to numVerts do
			(
				c = vColors[ vColorIndices[ i ] ]
				wU8 c.r
				wU8 c.g
				wU8 c.b
				wU8 c.alpha
			)
		)
		else
			wI32 0
		endChunk()
	)
	fn writeSkinChunk node nodeIndex =
	(
		local tempNode = undefined
		local s = node.modifiers[ 1 ]
		if node.modifiers[ 1 ] != node.skin then
		(
			tempNode = copy node
			for i = tempNode.modifiers.count to 1 by -1 do
				if ( classOf tempNode.modifiers[ i ] ) != Skin then
					deleteModifier tempNode i
			s = tempNode.skin
		)
		setCommandPanelTaskMode mode:#modify
		modPanel.setCurrentObject s
		local bones = getSkinBoneArray s
		local numVerts = skinOps.GetNumberVertices s
		beginChunk 5
		wI32 nodeIndex
		wI32 bones.count
		for i = 1 to bones.count do
			wI32 ( findItem nodes bones[ i ] )
		wI32 numVerts
		for i = 1 to numVerts do
		(
			local numWeights = skinOps.GetVertexWeightCount s i
			wI32 numWeights
			for j = 1 to numWeights do
			(
				wI32 ( skinOps.GetVertexWeightBoneID s i j )
				wFloat ( skinOps.GetVertexWeight s i j )
			)
		)
		endChunk()
		if tempNode != undefined then
			delete tempNode
	)
	on execute do
	(
		chunkCount = 0
		local fileName3db = getSaveFileName caption:loc3db_Export3db types:"Baltram's 3D format (*.3db)|*.3db|All Files (*.*)|*.*|"
		if fileName3db == undefined then
			return()
		try
			file = fopen fileName3db "wb"
		catch
			return( messagebox loc3db_CreateErr title:loc3db_3dbExport )
		local sel = getCurrentSelection()
		local panelTaskMode = getCommandPanelTaskMode()
		disableSceneRedraw()
		subobjectlevel = 0
		try
		(
			undo off
			(
				in coordsys world
				(
					setUpNodesArray()
					setUpMaterialArray()
					wString "3db"
					wFloat -1.0
					wI32 0 -- chunk count
					for i = 1 to materials.count do
						writeMaterialChunk materials[ i ] i 0
					for i = 1 to nodes.count do
						writeNodeChunk nodes[ i ] ( findItem nodes nodes[ i ].parent ) ( findItem materials nodes[ i ].material )
					for i = 1 to nodes.count do
						if ( isMesh nodes[ i ] ) then
							writeMeshChunk nodes[ i ] i
					for i = 1 to nodes.count do
					(
						try
						(
							if ( isMesh nodes[ i ] ) and nodes[ i ].skin != undefined then
								writeSkinChunk nodes[ i ] i
						)
						catch ()
					)
				)
			)
			seek_ 8
			wI32 chunkCount
		)
		catch
		(
			messagebox loc3db_ExportErr title:loc3db_3dbExport
		)
		fclose file
		setCommandPanelTaskMode mode:panelTaskMode
		select sel
		enableSceneRedraw()
		nodes = materials = #()
	)
)

macroScript showAbout category:"3db Tools" tooltip:"About..."
(
	rollout roAbout loc3db_About width:150
	(
		label lbName "" align:#center
		label lbDate "" align:#center
		hyperLink hlMe "by Baltram" align:#center address:"mailto:baltram-lielb@web.de" color:( color 0 0 170 ) hoverColor:( color 170 0 0 )
		on roAbout open do
		(
			lbName.caption = loc3db_3dbTools
			lbDate.caption = loc3db_Date
		)
	)
	on execute do
	(
		createDialog roAbout
	)
)

macroScript hideMenu category:"3db Tools" tooltip:"Hide 3db menu"
(
	on execute do
	(
		if ( enable3dbMenu == undefined ) then
		(
			menuman.unRegisterMenu ( menuman.findmenu "3db" )
			menuMan.updateMenuBar()
		)
		else
			messagebox loc3db_HideGuide title:"3db Tools"
	)
)

macroScript import3db category:"3db Tools" tooltip:"Import 3db..."
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
		if ( classOf n ) != Bone then
			return()
		createBone n.parent
		b = ( bonesys.createBone [ 0, 0, 0 ] [ norm, 0, 1e-6 ] [ 0, 0, 1 ] )
		b.width = 123
		b.name = n.name
		b.material = boneMat
		b.transform = n.transform
		b.parent = n.parent
		for c in n.children do
			c.parent = b
		nodes[ findItem nodes n ] = b
		delete n
	)
	fn adjustLength b =
	(
		if ( classOf b ) != BoneGeometry or b.width != 123 then
			return()
		for c in b.children do
			adjustLength c
		b.length = 1e-6
		for c in b.children do
		(
			p = in coordsys parent c.pos
			if ( ( abs p.y ) < 1e-3 ) and ( ( abs p.z ) < 1e-3 ) then
				b.length = p.x
		)
		local l = if b.length < norm / 50 then ( if b.parent != undefined and b.parent.width != b.parent.length then ( if b.parent.length > 4 * norm then b.parent.length / 2 + 2 * norm else b.parent.length ) else 4 * norm ) else b.length
		b.width = b.height = 1.5 / ( 0.5 / norm + 4 / l )
		if b.length < b.width then
			b.length = b.width
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
			adjustLength( b )
		bonesCreated = true
	)
	fn readMaterialChunk =
	(
		local matName = rString()
		local matType = rI32()
		case matType of
		(
			1: append materials ( standard name:matName showInViewport:true )
			2:
			(
				append materials ( multimaterial name:matName numsubs:0 )
				if materials[ materials.count ].numsubs == 1 then
					materials[ materials.count ].material1.name = "defaultmat"
			)
			3:
			(
				local multiMat = materials[ rI32() ]
				local i = multiMat.numsubs + 1
				if multiMat.numsubs == 1 and multiMat.material1.name == "defaultmat" then
					i = 1
				multiMat.materialList[ i ] = ( standard name:matName showInViewport:true )
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
		if ( materialIndex != 0 ) and ( ( classOf nodeDest ) == Editable_mesh ) then
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
		if numTVerts != 0 then
		(
			setNumTVerts meshDest numTVerts
			for i = 1 to numTVerts do
				setTVert meshDest i [ rFloat(), rFloat(), rFloat() ]
			buildTVFaces meshDest false
			for i = 1 to numFaces do
				setTVFace meshDest i [ rI32(), rI32(), rI32() ]
		)
		local numVColors = rI32()
		if numVColors != 0 then
		(
			defaultVCFaces meshDest
			meshop.defaultMapFaces meshDest -2
			for i = 1 to numVColors do
			(
				local c = [ rU8(), rU8(), rU8() ] as color
				local a = ( rU8() ) / 255.0
				setVertColor meshDest i c
				meshop.setMapVert meshDest -2 i [ a, a, a ]
			)
		)
		if smoothAngle != -1.0 then
			meshop.autoSmooth meshDest meshDest.faces smoothAngle
		nodeDest.mesh = meshDest
		nodeDest.objectoffsetpos = ( inverse nodeDest.transform ).translationPart
		nodeDest.objectoffsetrot = ( inverse nodeDest.transform ).rotationPart
		nodeDest.objectoffsetscale = ( inverse nodeDest.transform ).scalePart
	)
	fn readSkinChunk =
	(
		createBones()
		local boneIDs = #()
		local weights = #()
		local newBoneIndices = #()
		local boneNames = #()
		local n = nodes[ rI32() ]
		local s = Skin()
		addModifier n s
		setCommandPanelTaskMode mode:#modify
		modPanel.setCurrentObject s
		local numBones = rI32()
		for i = 1 to numBones do
		(
			b = nodes[ rI32() ]
			append newBoneIndices b.name
			skinOps.addBone s b 0
		)
		local numVerts = rI32()
		modPanel.setCurrentObject s
		for i = 1 to numBones do
			append boneNames ( skinops.GetBoneName s i 0 )
		for i = 1 to numBones do
			newBoneIndices[ i ] = ( findItem boneNames newBoneIndices[ i ] )
		for i = 1 to numVerts do
		(
			local numWeights = rI32()
			boneIDs.count = weights.count = numWeights
			for j = 1 to numWeights do
			(
				boneIDs[ j ] = newBoneIndices[ rI32() ]
				weights[ j ] = rFloat()
			)
			skinOps.replaceVertexWeights s i boneIDs weights
		)
	)
	on execute do
	(
		nodes = #()
		materials = #()
		bonesCreated = false
		local fileName3db = getOpenFileName caption:loc3db_Import3db types:"Baltram's 3D format (*.3db)|*.3db|All Files (*.*)|*.*|"
		if fileName3db == undefined then
			return()
		try
		(
			file = fopen fileName3db "rb"
		)
		catch
		(
			messagebox loc3db_OpenErr title:loc3db_3dbImport
			return()
		)
		if rString() != "3db" then
			return ( messagebox loc3db_FormatErr title:loc3db_3dbImport )
		if queryBox loc3db_Replace title:loc3db_3dbImport then
			max file new
		smoothAngle = rFloat()
		local nChunks = rI32()
		local endChunkOffset = tell()
		local sel = getCurrentSelection()
		local panelTaskMode = getCommandPanelTaskMode()
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
					if version3db >= minVersion then
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
				createBones()
				for i = 1 to materials.count do
				(
					n = getNumSubMtls materials[ i ]
					if n != 0 then
					(
						for j = 1 to n do
						(
							( getSubMtl materials[ i ] j ).showInViewport = true
						)
					)
					else
						materials[ i ].showInViewport = true
				)
			)
		)
		catch
		(
			messagebox loc3db_ImportErr title:loc3db_3dbImport
		)
		setCommandPanelTaskMode mode:panelTaskMode
		select sel
		enableSceneRedraw()
		fclose file
		nodes = materials = #()
	)
)

fn enable3dbMenu enabled =
(
	local menuName = "3db"
	menu_3db = ( menuman.findmenu menuName )
	if enabled == "?" then
		return ( menu_3db != undefined )
	if enabled == ( menu_3db != undefined ) then
		return()
	if enabled == true then
	(
		local mainMenuBar = menuMan.getMainMenuBar()
		menu_3db = menuMan.createMenu menuName
		local import3dbItem = menuMan.createActionItem "import3db" "3db Tools"
		local export3dbItem = menuMan.createActionItem "export3db" "3db Tools"
		local separatorItem1 = menuMan.createSeparatorItem()
		local hideMenuItem = menuMan.createActionItem "hideMenu" "3db Tools"
		local separatorItem2 = menuMan.createSeparatorItem()
		local showAboutItem = menuMan.createActionItem "showAbout" "3db Tools"
		for m in #( import3dbItem, export3dbItem, hideMenuItem, showAboutItem ) do
			m.setUseCustomTitle true
		import3dbItem.setTitle loc3db_Import3db_
		export3dbItem.setTitle loc3db_Export3db_
		hideMenuItem.setTitle loc3db_HideMenu
		showAboutItem.setTitle loc3db_About_
		for i in #( import3dbItem, export3dbItem, separatorItem1, hideMenuItem, separatorItem2, showAboutItem ) do
			menu_3db.addItem i -1
		local menu_3db_sub = menuMan.createSubMenuItem menuName menu_3db
		mainMenuBar.addItem menu_3db_sub ( mainMenuBar.numItems() - 1 )
	)
	else
		menuman.unRegisterMenu ( menu_3db )
	menuMan.updateMenuBar()
)

utility tools_3d "3db Tools"
(
	group ""
	(
		label label1 ""
		label label2 ""
		hyperlink link1 "by Baltram" align:#center address:"mailto:baltram-lielb@web.de" color:(color 0 0 170) hoverColor:(color 170 0 0)
	)
	group ""
	(
		button buttonImport ""
		button buttonExport ""
	)
	group ""
	(
		checkbox checkboxShowMenu "SSSSShow '3db' menu" checked:true
	)
	on checkboxShowMenu changed arg do
	(
		enable3dbMenu arg
	)
	on buttonImport pressed do
	(
		macros.run "3db Tools" "import3db"
	)
	on buttonExport pressed do
	(
		macros.run "3db Tools" "export3db"
	)
	on tools_3d open do
	(
		checkboxShowMenu.checked = ( enable3dbMenu "?" )
		label1.caption = loc3db_3dbTools
		label2.caption = loc3db_Date
		buttonImport.caption = loc3db_Import3db_
		buttonExport.caption = loc3db_Export3db_
		checkboxShowMenu.caption = loc3db_ShowMenu
	)
)

if heapSize < 25600000 then
	heapSize += ( 25600000 - heapSize )

if menuMan.registerMenuContext 0x12049633 then
	enable3dbMenu true
else
(
	menu_3db = menuman.findmenu "3db"
	if menu_3db != undefined and ( menu_3db.getItem 1 ).getTitle() != loc3db_Import3db_ then
	(
		enable3dbMenu false
		enable3dbMenu true
	)
)