#include "plugin_sdk/plugin_sdk.hpp"
#include "Others.h"
#include "Helpers.h"
#include <string>

namespace fatalityutils
{
	void AATracker()
	{
		for (const auto& target : entitylist->get_enemy_heroes())
		{
			if (target != nullptr && target->is_visible_on_screen() && !target->is_dead())
			{
				int num = (int)(target->get_health() / myhero->get_auto_attack_damage(target));
				std::string numstr = std::to_string(num);
				int firstdigit = std::stoi(numstr.substr(0, 2));
				draw_manager->add_text(target->get_position(), MAKE_COLOR(255, 255, 255, 255), 22, "AA Left: %i", firstdigit);
			}
		}
	}

	bool is_valid_minion(const game_object_script& obj)
	{
		if (obj == nullptr)
		{
			return false;
		}

		if (obj->is_ward())
		{
			return false;
		}
		const auto hash = buff_hash_real(obj->get_base_skin_name().c_str());
		if (hash == buff_hash("TeemoMushroom") || hash == buff_hash("JhinTrap") || hash == buff_hash("NidaleeSpear"))
		{
			return false;
		}
		return true;
	}

	void draw_dmg_rl(const game_object_script& target, const float damage, unsigned long color)
	{
		// Draw damage on HP bar from right to left
		if (target != nullptr && target->is_valid() && target->is_hpbar_recently_rendered())
		{
			auto bar_pos = target->get_hpbar_pos();

			if (bar_pos.is_valid() && !target->is_dead() && target->is_visible())
			{
				const auto health = target->get_real_health();

				bar_pos = vector(bar_pos.x + (105 * (health / target->get_max_health())), bar_pos.y -= 10);

				auto damage_size = (105 * (damage / target->get_max_health()));

				if (damage >= health)
				{
					damage_size = (105 * (health / target->get_max_health()));
				}

				if (damage_size > 105)
				{
					damage_size = 105;
				}

				const auto size = vector(bar_pos.x + (damage_size * -1), bar_pos.y + 11);

				draw_manager->add_filled_rect(bar_pos, size, color);
			}
		}
	}

	int count_minions(const vector& position, const float range, const std::string& team)
	{
		int count = 0;
		if (team == "ally")
		{
			for (auto&& t : entitylist->get_ally_minions())
			{
				if (t != nullptr && t->is_valid() && is_valid_minion(t)) count++;
			}
		}
		if (team == "enemy")
		{
			for (auto&& t : entitylist->get_enemy_minions())
			{
				if (t != nullptr && t->is_valid_target(range, position) && is_valid_minion(t)) count++;
			}
		}
		if (team == "neutral")
		{
			for (auto&& t : entitylist->get_jugnle_mobs_minions())
			{
				if (t != nullptr && t->is_valid_target(range, position) && is_valid_minion(t)) count++;
			}
		}
		if (team == "both")
		{
			for (auto&& t : entitylist->get_enemy_minions())
			{
				if (t != nullptr && t->is_valid_target(range, position) && is_valid_minion(t)) count++;
			}
			for (auto&& t : entitylist->get_jugnle_mobs_minions())
			{
				if (t != nullptr && t->is_valid_target(range, position) && is_valid_minion(t)) count++;
			}
		}
		return count;
	}
}