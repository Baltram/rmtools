fn getFileAsString filePath =
(
	local fileStream = openFile filePath
	local result = ( readDelimitedString fileStream "~" )
	try ( close fileStream ) catch ()
	return result
)

fn run =
(
	local fileBasePath = ( getSavePath caption:"Please specify your Extended Saving\\source directory" ) + "\\"
	resetMaxFile #noPrompt
	persistent global createExtraInfoString = ( getFileAsString ( fileBasePath + "Extended Saving-createExtraInfo.mcr" ) )
	persistent global import3dbString = ( getFileAsString ( fileBasePath + "Extended Saving-import3db.mcr" ) )
	persistent global invokeOnceString = ( getFileAsString ( fileBasePath + "Extended Saving-invokeOnce.mcr" ) )
	persistent global installExtendedSavingString = ( getFileAsString ( fileBasePath + "installExtendedSaving.ms" ) ) + "\n\ninstallExtendedSaving()"
	persistent global showAboutString = ( getFileAsString ( fileBasePath + "Extended Saving-showAbout.mcr" ) )
	persistent global startupString = ( getFileAsString ( fileBasePath + "Extended Saving-Startup.mcr" ) )
	persistent global toggleString = ( getFileAsString ( fileBasePath + "Extended Saving-toggle.mcr" ) )
	persistent global toggleDefaultString = ( getFileAsString ( fileBasePath + "Extended Saving-toggleDefault.mcr" ) )
	persistent global uninstallString = ( getFileAsString ( fileBasePath + "Extended Saving-uninstall.mcr" ) )
	callbacks.addScript #filePostOpen "execute installExtendedSavingString" id:#Extended_Saving_Installation persistent:true
	saveMaxFile ( fileBasePath + "\\..\\installExtendedSaving.gmax" )
	resetMaxFile #noPrompt
)

run()