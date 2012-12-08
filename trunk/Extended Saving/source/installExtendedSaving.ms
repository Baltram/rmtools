fn installExtendedSaving =
(
	local cName = "Extended Saving"
	local esVersion = 4
	
	while ( menuMan.findMenu cName ) != undefined do
	(
		global _requestVersion = true
		macros.run cName "startup"
		local queryString
		if _requestVersion > esVersion then queryString = "A newer"
		else if _requestVersion < esVersion then queryString = "An older"
		else queryString = "The same"
		queryString += " version of Extended Saving is already installed. You need to uninstall it first in order to do a reinstall. Continue?"
		global _requestVersion = undefined
		if ( querybox queryString title:cName ) == false then return 0
		macros.run cName "uninstall"
	)
	
	try
	(
		global createExtraInfoString
		global import3dbString
		global invokeOnceString
		global showAboutString
		global startupString
		global toggleString
		global toggleDefaultString
		global uninstallString
		execute createExtraInfoString
		execute import3dbString
		execute invokeOnceString
		execute showAboutString
		execute startupString
		execute toggleString
		execute toggleDefaultString
		execute uninstallString
	)
	catch ()
	
	local mainMenuBar = menuMan.getMainMenuBar()
	local esMenu = menuMan.createMenu cName
	
	local toggleItem = menuMan.createActionItem "toggle" cName
	local toggleDefaultItem = menuMan.createActionItem "toggleDefault" cName
	local separatorItem1 = menuMan.createSeparatorItem()
	local invokeOnceItem = menuMan.createActionItem "invokeOnce" cName
	local separatorItem2 = menuMan.createSeparatorItem()
	local import3dbIten = menuMan.createActionItem "import3db" cName
	local separatorItem3 = menuMan.createSeparatorItem()
	local uninstallItem = menuMan.createActionItem "uninstall" cName
	local showAboutItem = menuMan.createActionItem "showAbout" cName
	local startupItem = menuMan.createActionItem "startup" cName
	
	toggleItem.setUseCustomTitle true
	toggleDefaultItem.setUseCustomTitle true
	invokeOnceItem.setUseCustomTitle true
	import3dbIten.setUseCustomTitle true
	uninstallItem.setUseCustomTitle true
	showAboutItem.setUseCustomTitle true
	startupItem.setUseCustomTitle true
	
	invokeOnceItem.setTitle "Enable Once And Save"
	import3dbIten.setTitle "Import 3db File..."
	uninstallItem.setTitle "Uninstall"
	showAboutItem.setTitle "About"
	startupItem.setTitle ""
	
	esMenu.addItem toggleItem -1
	esMenu.addItem toggleDefaultItem -1
	esMenu.addItem separatorItem1 -1
	esMenu.addItem invokeOnceItem -1
	esMenu.addItem separatorItem2 -1
	esMenu.addItem import3dbIten -1
	esMenu.addItem separatorItem3 -1
	esMenu.addItem uninstallItem -1
	esMenu.addItem showAboutItem -1
	esMenu.addItem startupItem -1
	
	local esSubMenuItem = menuMan.createSubMenuItem cName esMenu
	global _requestNewInit = true
	macros.run cName "startup"
	mainMenuBar.addItem esSubMenuItem ( mainMenuBar.numItems() - 1 )
	menuMan.updateMenuBar()
	messagebox "Extended Saving has been successfully installed! You can turn it on and off using the new \"Extended Saving\" menu in the main menu bar." title:cName
	resetMaxFile #noPrompt
)