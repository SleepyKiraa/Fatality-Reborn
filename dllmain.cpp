#include "plugin_sdk/plugin_sdk.hpp"
#include "Xerath.h"

PLUGIN_NAME("Fatality Reborn")
SUPPORTED_CHAMPIONS(champion_id::Xerath);

PLUGIN_API bool on_sdk_load(plugin_sdk_core* plugin_sdk_good)
{
	DECLARE_GLOBALS(plugin_sdk_good);
	switch (myhero->get_champion())
	{
	case champion_id::Xerath:
		xerath::load();
		console->print("[Fatality] Xerath Loaded!");
		myhero->print_chat(0, "<font color=\"#ff00cb\">[Fatality]</font> <font color=\"#ee00cb\">Xerath Loaded!</font>");
		return true;

	default:
		break;
	}

	return false;
}

PLUGIN_API void on_sdk_unload()
{
	switch (myhero->get_champion())
	{
	case champion_id::Xerath:
		xerath::unload();
		return;

	default:
		break;
	}
}