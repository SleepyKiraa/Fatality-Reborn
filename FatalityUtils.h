#include "plugin_sdk/plugin_sdk.hpp"
#include <string>
#include <sstream>

namespace fatalityutils
{
	bool is_valid_minion(const game_object_script& obj);
	int count_minions(const vector& position, const float range, const std::string& team);
	void draw_dmg_rl(const game_object_script& target, const float damage, unsigned long color);
	void AATracker();
}