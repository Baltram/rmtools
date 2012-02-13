macroScript invokeOnce category:"Extended Saving" tooltip:"Invoke Once"
(
	on execute do
	(
		global _extendedSavingEnabled
		local oldState = _extendedSavingEnabled
		global _extendedSavingEnabledOverride = oldState
		_extendedSavingEnabled = true
		max file saveas
		global _extendedSavingEnabledOverride = undefined
		_extendedSavingEnabled = oldState
	)
) 