macroScript Startup category:"Extended Saving" tooltip:"Ignore Me"
(
	local isInitialized = false
	local removePersistentsString = "\t
		try ( persistents.remove #_extendedSaveData ) catch ()
		try ( persistents.remove #_extendedSavingEnabled ) catch ( global _extendedSavingEnabled = false )
		try ( persistents.remove #_extendedSavingEnabledOverride ) catch ()
		global _extendedSaveData = undefined
		global _extendedSavingEnabledOverride = undefined"
	local postNewString = "\t
		global _extendedSavingEnabled = ( ( menuman.findMenu \"Extended Saving By Default\" ) != undefined )"
	local postResetString = postNewString
	local postOpenString = "\t
		global _extendedSavingEnabledOverride
		global _extendedSavingEnabled
		if ( classOf _extendedSavingEnabledOverride ) == BooleanClass then _extendedSavingEnabled = _extendedSavingEnabledOverride" + removePersistentsString
	local postOpenPersistentString = "\t
		if ( menuMan.findMenu \"Extended Saving\" ) == undefined then
		(" + removePersistentsString + "\t
			global _extendedSavingEnabled = undefined
		)
		callbacks.removeScripts #filePostOpen id:#Extended_Saving_Persistent"
	local preMergeString = "\t
		global _extendedSavingEnabled
		global _extendedSavingEnabledPreMerge = _extendedSavingEnabled"
	local postMergeString = postOpenString + "\t
		global _extendedSavingEnabledPreMerge
		global _extendedSavingEnabled = _extendedSavingEnabledPreMerge
		global _extendedSavingEnabledPreMerge = undefined"
	local preSaveString = "\t
		global _extendedSavingEnabled
		if _extendedSavingEnabled == true then
		(
			global _requestPersistentCallback = true
			macros.run \"Extended Saving\" \"startup\"
			macros.run \"Extended Saving\" \"createExtraInfo\"
			persistent global _extendedSaveData
			persistent global _extendedSavingEnabled
			persistent global _extendedSavingEnabledOverride
		)
		else
		(" + removePersistentsString + "\t
		)"
	local postSaveString = "\t
		global _extendedSavingEnabled
		if _extendedSavingEnabled == true then
		(" + postOpenString + "\t
			callbacks.removeScripts #filePostOpen id:#Extended_Saving_Persistent
			global _extendedSaveMemoryTotal
			if ( classOf _extendedSaveMemoryTotal ) == Integer then
			(
				_extendedSaveMemoryTotal = ( _extendedSaveMemoryTotal as float ) / 1024 / 1000
				local fileSizeIncrease = ( floor ( _extendedSaveMemoryTotal * 10 / 2.25 ) ) / 10
				local minimumScripterMemory = ( floor ( _extendedSaveMemoryTotal * 10 * 15 / 14 + 18 ) ) / 10
				local currentHeapSize = ( floor ( ( heapSize as float ) / 1024 / 1000 * 10 ) ) / 10
				local fileSize = 0
				try
				(
					local sceneFileStream = fopen ( maxFilePath + maxFileName ) \"rb\"
					fseek sceneFileStream 0 #seek_end
					fileSize = ( ( ftell sceneFileStream ) as float ) / 1024 / 1000
					fileSize = ( floor ( fileSize * 10 ) ) / 10
					fclose sceneFileStream
				)
				catch ()
				local errorString = \"Error: Not enough scripter memory--please increase it in the settings.\\n\\n\" +
				                    \"The file \" + maxFileName + \" (\" + ( fileSize as string ) + \"MB) has been saved ordinarily. \" +
									\"Extended Saving would raise the file size by approximately \" + ( fileSizeIncrease as string ) + \"MB. \" +
									\"To handle such amount of data at least \" + ( minimumScripterMemory as string ) + \"MB of scripter memory are required. \" +
									\"The current setting is at \" + ( currentHeapSize as string ) + \"MB.\\n\\n\" +
									\"To resolve the problem, navigate (in the main GMax menu bar) to 'Customize' > 'Preferences...' > 'MAXScript' > 'Initial heap allocation'. \" +
									\"Adjust the value there to \" + ( minimumScripterMemory as string ) + \"MB or more, then restart GMax. \" +
									\"Bear in mind that scripter memory utilizes your RAM!\"
				messagebox errorString title:\"Extended Saving\"
				_extendedSaveMemoryTotal = undefined
			)
		)
		setSaveRequired false"
	fn initialize =
	(
		callbacks.removeScripts id:#Extended_Saving
		callbacks.addScript #filePostOpen postOpenString id:#Extended_Saving
		callbacks.addScript #filePreMerge preMergeString id:#Extended_Saving
		callbacks.addScript #filePostMerge postMergeString id:#Extended_Saving
		callbacks.addScript #filePreSave preSaveString id:#Extended_Saving
		callbacks.addScript #filePostSave postSaveString id:#Extended_Saving
		callbacks.addScript #systemPostNew postNewString id:#Extended_Saving
		callbacks.addScript #systemPostReset postResetString id:#Extended_Saving
		if maxFileName == "" then execute postNewString else execute postOpenString
		isInitialized = true
	)
	on execute do
	(	
		global _requestPersistentCallback
		global _requestNewInit
		global _requestVersion
		if _requestNewInit == true then
		(
			global _requestNewInit = undefined
			isInitialized = false
			initialize()
		)
		if _requestPersistentCallback == true then
		(
			global _requestPersistentCallback = undefined
			callbacks.addScript #filePostOpen postOpenPersistentString id:#Extended_Saving_Persistent persistent:true
		)
		if _requestVersion == true then
		(
			_requestVersion = 5
		)
	)
	on isVisible do
	(
		if isInitialized != true then initialize()
		return false
	)
)