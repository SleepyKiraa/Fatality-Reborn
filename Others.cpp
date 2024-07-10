#include "Others.h"
#include "Helpers.h"
#include "Permashow.hpp"
#include <array>
#include <sstream>

TreeTab* othersMenu = nullptr;

others others::instance;

others& misc()
{
	if (!others::instance.initialized) others::instance.init();

	return others::instance;
}

void others::init()
{
	initialized = true;

	othersMenu = menu->create_tab("others", "[Fatality] Permashow");
	permashow::instance.init(othersMenu);
}

void others::destroy()
{
	if (othersMenu) menu->delete_tab(othersMenu);
	permashow::instance.destroy();
}
