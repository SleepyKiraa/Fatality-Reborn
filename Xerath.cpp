#include "plugin_sdk/plugin_sdk.hpp"
#include <unordered_set>
#include <unordered_map>
#include <stdarg.h>
#include <iostream>
#include "Permashow.hpp"
#include "Others.h"
#include "Xerath.h"
#include "FatalityUtils.h"
#include <chrono>
#include <thread>

namespace xerath
{
	script_spell* Q;
	script_spell* W;
	script_spell* E;
	script_spell* R;
	auto last_cast_time = std::chrono::steady_clock::now();

	TreeTab* mainMenu;
	namespace settings
	{
		namespace qsettings
		{
			TreeEntry* qCombo;
			TreeEntry* waitforw;
			TreeEntry* overcharge;
			TreeEntry* overchargevalue;
		}

		namespace wsettings
		{
			TreeEntry* wCombo;
			TreeEntry* autoW;
		}

		namespace esettings
		{
			TreeEntry* eCombo;
			TreeEntry* autoE;
		}

		namespace rsettings
		{
			TreeEntry* rMode;
			TreeEntry* cursorrange;
			TreeEntry* rdelay;
		}

		namespace Killsteal
		{
			TreeEntry* ksQ;
			TreeEntry* ksW;
			TreeEntry* ksE;
		}

		namespace Clear
		{
			TreeEntry* FarmToggle;
			TreeEntry* LcW;
			TreeEntry* LcWMinions;
			TreeEntry* JcQ;
			TreeEntry* JcW;
		}

		namespace Misc
		{
			TreeEntry* ag;
			TreeEntry* AgHealth;
			TreeEntry* interrupter;
		}

		namespace hitchance
		{
			TreeEntry* qHitChance;
			TreeEntry* wHitChance;
			TreeEntry* eHitChance;
			TreeEntry* rHitChance;
		}

		namespace Draws
		{
			TreeEntry* drawQ;
			TreeEntry* drawQColor;
			TreeEntry* drawQ2Color;
			TreeEntry* drawQthickness;
			TreeEntry* drawW;
			TreeEntry* drawWColor;
			TreeEntry* drawW2Color;
			TreeEntry* drawWthickness;
			TreeEntry* drawE;
			TreeEntry* drawEColor;
			TreeEntry* drawE2Color;
			TreeEntry* drawEthickness;
			TreeEntry* drawR;
			TreeEntry* drawAA;
			TreeEntry* drawKill;
			TreeEntry* rkillableTextSize;
			TreeEntry* Damage;
		}

		namespace Keys
		{
			TreeEntry* farmtogglekey;
			TreeEntry* rKey;
			TreeEntry* semiE;
		}
	}

	void createmenu()
	{
		mainMenu = menu->create_tab("fatality.xerath", "[Fatality] Xerath");
		mainMenu->set_texture(myhero->get_square_icon_portrait());

		const auto combotab = mainMenu->add_tab("fatality.xerath.combo", "Combo");
		{
			const auto qsettings = combotab->add_tab("fatality.xerath.combo.qsettings", "Arcanopulse - [Q]");
			qsettings->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
			{
				settings::qsettings::qCombo = qsettings->add_checkbox("fatality.xerath.useq", "Enable Q in Combo", true);
				settings::qsettings::waitforw = qsettings->add_checkbox("fatality.xerath.waitw", "Wait For W", true);
				settings::qsettings::overcharge = qsettings->add_checkbox("fatality.xerath.overcharge", "Enable Overcharge Mode", true);
				settings::qsettings::overchargevalue = qsettings->add_slider("fatality.xerath.overchargevalue", "Overcharge Range", 200, 100, 350, true);
			}

			const auto wsettings = combotab->add_tab("fatality.xerath.combo.wsettings", "Eye of Destruction - [W]");
			wsettings->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
			{
				settings::wsettings::wCombo = wsettings->add_checkbox("fatality.xerath.usew", "Enable W in Combo", true);
				settings::wsettings::autoW = wsettings->add_checkbox("fatality.xerath.autow", "Auto W on Dash/CC", true);
			}

			const auto esettings = combotab->add_tab("fatality.xerath.combo.esettings", "Shocking Orb - [E]");
			esettings->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
			{
				settings::esettings::eCombo = esettings->add_checkbox("fatality.xerath.usee", "Enable E in Combo", true);
				settings::esettings::autoE = esettings->add_checkbox("fatality.xerath.autoe", "Auto E on Dash/CC", true);
			}

			const auto rsettings = combotab->add_tab("fatality.xerath.combo.rsettings", "Rite of the Arcane - [R]");
			rsettings->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
			{
				settings::rsettings::rMode = rsettings->add_combobox("fatality.xerath.rmode", "R Mode", { {"Auto", nullptr}, {"Tap Key", nullptr}}, 1, true);
				settings::rsettings::cursorrange = rsettings->add_slider("fatality.xerath.cursorrange", "Cursor Range", 500, 300, 1000, true);
				settings::rsettings::rdelay = rsettings->add_slider("fatality.xerath.delay", "R Delay", 500, 0, 1500, true);
			}
		}

		const auto killstealtab = mainMenu->add_tab("fatality.xerath.ks", "Killsteal");
		{
			settings::Killsteal::ksQ = killstealtab->add_checkbox("fatality.xerath.ksq", "Enable Q Killsteal", true);
			settings::Killsteal::ksW = killstealtab->add_checkbox("fatality.xerath.ksw", "Enable W Killsteal", true);
			settings::Killsteal::ksE = killstealtab->add_checkbox("fatality.xerath.kse", "Enable E Killsteal", true);
		}

		const auto cleartab = mainMenu->add_tab("fatality.xerath.clear", "Clear");
		{
			settings::Clear::FarmToggle = cleartab->add_hotkey("fatality.xerath.toggle", "Farm Toggle", TreeHotkeyMode::Toggle, 0x41, true, true);
			cleartab->add_separator("seperator1", "Lane Clear");
			settings::Clear::LcW = cleartab->add_checkbox("fatality.xerath.wclear", "Enable W Lane Clear", true);
			settings::Clear::LcWMinions = cleartab->add_slider("fatality.xerath.wmins", "Minions to W Clear", 3, 1, 5, true);
			cleartab->add_separator("seperator2", "Jungle Clear");
			settings::Clear::JcQ = cleartab->add_checkbox("fatality.xerath.jcq", "Enable Q Jungle Clear", true);
			settings::Clear::JcW = cleartab->add_checkbox("fatality.xerath.jcw", "Enable W Jungle Clear", true);
		}

		const auto misctab = mainMenu->add_tab("fatality.xerath.misc", "Misc");
		{
			settings::Misc::ag = misctab->add_checkbox("fatality.xerath.anti", "Enable Anti Gapcloser", true);
			settings::Misc::AgHealth = misctab->add_slider("fatality.xerath.hp", "Min HP % to Anti Gapclose", 50, 1, 100, true);
			settings::Misc::interrupter = misctab->add_checkbox("fatality.xerath.inter", "Enable Interrupter", true);
		}

		const auto hitchancetab = mainMenu->add_tab("fatality.xerath.hitchance", "Hitchance");
		std::vector<std::pair<std::string, void*>> combo_elemts = {};
		for (int i = 3; i < 9; i++)
		{
			const auto hitchance = static_cast<hit_chance>(i);

			std::string hitchance_str;
			switch (hitchance)
			{
			case hit_chance::immobile:
				hitchance_str = "Immobile";
				break;

			case hit_chance::dashing:
				hitchance_str = "Dashing";
				break;

			case hit_chance::very_high:
				hitchance_str = "Very High";
				break;
			case hit_chance::high:
				hitchance_str = "High";
				break;
			case hit_chance::medium:
				hitchance_str = "Medium";
				break;
			case hit_chance::low:
				hitchance_str = "Low";
				break;
			case hit_chance::impossible:
				hitchance_str = "Impossible";
				break;
			case hit_chance::out_of_range:
				hitchance_str = "Out Of Range";
				break;
			case hit_chance::collision:
				hitchance_str = "Collision";
				break;
			}

			combo_elemts.emplace_back(hitchance_str, nullptr);
		}

		settings::hitchance::qHitChance = hitchancetab->add_combobox("fatality.xerath.qhitchance", "Q Hitchance", combo_elemts, 2);
		settings::hitchance::wHitChance = hitchancetab->add_combobox("fatality.xerath.whitchance", "W Hitchance", combo_elemts, 2);
		settings::hitchance::eHitChance = hitchancetab->add_combobox("fatality.xerath.ehitchance", "E Hitchance", combo_elemts, 2);
		settings::hitchance::rHitChance = hitchancetab->add_combobox("fatality.xerath.rhitchance", "R Hitchance", combo_elemts, 2);

		const auto drawTab = mainMenu->add_tab("fatality.xerath.draw", "Draws");
		{
			float defaultcolorQ[] = { 1.f, 1.f, 1.f, 1.f };
			float defaultcolorQ2[] = { 1.f, 0.f, 1.f, 1.f };
			float defaultcolorW[] = { 0.f, 0.f, 1.f, 1.f };
			float defaultcolorW2[] = { 1.f, 0.f, 1.f, 1.f };
			float defaultcolorE[] = { 0.f, 1.f, 0.f, 1.f };
			float defaultcolorE2[] = { 1.f, 0.f, 1.f, 1.f };

			const auto qdrawtab = drawTab->add_tab("fatality.xerath.drawq", "Q Draw Settings");
			{
				settings::Draws::drawQ = qdrawtab->add_checkbox("fatality.xerath.qrange", "Draw Q Range", true);
				settings::Draws::drawQColor = qdrawtab->add_colorpick("fatality.xerath.qcolor", "Q Draw Color", defaultcolorQ);
				settings::Draws::drawQ2Color = qdrawtab->add_colorpick("fatality.xerath.q2color", "Q2 Draw Color", defaultcolorQ2);
				settings::Draws::drawQthickness = qdrawtab->add_slider("fatality.xerath.thicknessq", "Q circle Thickness", 2, 1, 6, true);
			}

			const auto wdrawtab = drawTab->add_tab("fatality.xerath.draww", "W Draw Settings");
			{
				settings::Draws::drawW = wdrawtab->add_checkbox("fatality.xerath.wrange", "Draw W Range", true);
				settings::Draws::drawWColor = wdrawtab->add_colorpick("fatality.xerath.wcolor", "W Draw Color", defaultcolorW);
				settings::Draws::drawW2Color = wdrawtab->add_colorpick("fatality.xerath.w2color", "W2 Draw Color", defaultcolorW2);
				settings::Draws::drawWthickness = wdrawtab->add_slider("fatality.xerath.thicknessw", "W circle Thickness", 2, 1, 6, true);
			}

			const auto edrawtab = drawTab->add_tab("fatality.xerath.drawe", "E Draw Settings");
			{
				settings::Draws::drawE = edrawtab->add_checkbox("fatality.xerath.erange", "Draw E Range", true);
				settings::Draws::drawEColor = edrawtab->add_colorpick("fatality.xerath.ecolor", "E Draw Color", defaultcolorE);
				settings::Draws::drawE2Color = edrawtab->add_colorpick("fatality.xerath.e2color", "E2 Draw Color", defaultcolorE2);
				settings::Draws::drawEthickness = edrawtab->add_slider("fatality.xerath.thicknesse", "E circle Thickness", 2, 1, 6, true);
			}

			const auto rdrawtab = drawTab->add_tab("fatality.xerath.drawr", "R Draw Settings");
			{
				settings::Draws::drawR = rdrawtab->add_checkbox("fatality.xerath.rrange", "Draw R Range", true);
			}

			const auto miscdrawTab = drawTab->add_tab("fatality.xerath.draw.misc", "Misc Draw Settings");
			{
				settings::Draws::Damage = miscdrawTab->add_checkbox("fatality.xerath.damage", "Draw Damage Indicator", true);
				settings::Draws::drawAA = miscdrawTab->add_checkbox("fatality.xerath.aaindicator", "Draw AA Counter", true);
				miscdrawTab->add_separator("sep44", "Killable Text Settings");
				settings::Draws::drawKill = miscdrawTab->add_checkbox("fatality.xerath.killtext", "Draw Killable R Text", true);
				settings::Draws::rkillableTextSize = miscdrawTab->add_slider("fatality.xerath.textsize", "Text Size", 22, 5, 35);
			}
		}

		const auto keysTab = mainMenu->add_tab("fatality.xerath.keys", "Keys");
		{
			settings::Keys::semiE = keysTab->add_hotkey("semie", "Semi E", TreeHotkeyMode::Hold, 0x47, false, true);
			settings::Keys::rKey = keysTab->add_hotkey("semir", "Semi R", TreeHotkeyMode::Hold, 0x54, false, true);
		}

		misc().init();
		get_permashow().add_element("Farm", settings::Clear::FarmToggle);
		get_permashow().add_element("Semi E", settings::Keys::semiE);
		get_permashow().add_element("semi R", settings::Keys::rKey);
	}

	hit_chance getPredIntFromSettings(int hitchance)
	{
		// Get hitchance from settings value
		return static_cast<hit_chance>(hitchance + 3);
	}

	void QLogic()
	{
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			return;
		}

		auto qtarget = target_selector->get_target(Q->charged_max_range, damage_type::magical);

		if (settings::qsettings::waitforw->get_bool())
		{
			if (W->is_ready())
			{
				if (qtarget != nullptr)
				{
					if (qtarget->get_distance(myhero) <= W->range())
					{
						return;
					}
				}
			}
		}

		if (settings::qsettings::qCombo->get_bool())
		{
			if (Q->is_ready())
			{
				if (qtarget != nullptr)
				{
					if (qtarget->is_valid())
					{
						if (!myhero->has_buff(buff_hash("XerathArcanopulseChargeUp")))
						{
							if (!Q->is_charging())
							{
								Q->start_charging();
								console->print("[Fatality] Q Started to Charge");
								return;
							}
						}

						if (myhero->has_buff(buff_hash("XerathArcanopulseChargeUp")))
						{
							if (!settings::qsettings::overcharge->get_bool())
							{
								if (Q->range() == Q->charged_max_range)
								{
									auto qpred = Q->get_prediction(qtarget);
									if (qpred.hitchance >= getPredIntFromSettings(settings::hitchance::qHitChance->get_int()))
									{
										myhero->update_charged_spell(Q->get_slot(), qpred.get_cast_position(), true, false);
										console->print("debug");
									}
								}
							}

							if (settings::qsettings::overcharge->get_bool())
							{
								auto bufftime = myhero->get_buff(buff_hash("XerathArcanopulseChargeUp"));
								if (bufftime->get_remaining_time() <= 1.5)
								{
									auto qpred = Q->get_prediction(qtarget);
									if (qpred.hitchance >= getPredIntFromSettings(settings::hitchance::qHitChance->get_int()))
									{
										myhero->update_charged_spell(Q->get_slot(), qpred.get_cast_position(), true, false);
									}
								}

								if (Q->range() > qtarget->get_distance(myhero) + settings::qsettings::overchargevalue->get_int())
								{
									auto qpred = Q->get_prediction(qtarget);
									if (qpred.hitchance >= getPredIntFromSettings(settings::hitchance::qHitChance->get_int()))
									{
										myhero->update_charged_spell(Q->get_slot(), qpred.get_cast_position(), true, false);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void WLogic()
	{
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			return;
		}

		if (settings::wsettings::wCombo->get_bool())
		{
			if (W->is_ready())
			{
				auto wtarget = target_selector->get_target(W->range(), damage_type::magical);
				if (wtarget != nullptr)
				{
					if (wtarget->is_valid())
					{
						auto wpred = W->get_prediction(wtarget);
						if (wpred.hitchance >= getPredIntFromSettings(settings::hitchance::wHitChance->get_int()))
						{
							W->cast(wpred.get_cast_position());
						}
					}
				}
			}
		}
	}

	void AutoW()
	{
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			return;
		}

		if (settings::wsettings::autoW->get_bool())
		{
			if (W->is_ready())
			{
				auto wtarget = target_selector->get_target(W->range(), damage_type::magical);
				if (wtarget != nullptr)
				{
					if (wtarget->is_valid())
					{
						if (wtarget->is_dashing())
						{
							if (wtarget->get_path_controller()->get_end_vec().distance(myhero) <= W->range())
							{
								W->cast(wtarget->get_path_controller()->get_end_vec());
								console->print("[Fatality] Casting W on Dash Target: %s", wtarget->get_base_skin_name().c_str());
							}
						}

						if (wtarget->has_buff_type(buff_type::Stun) || wtarget->has_buff_type(buff_type::Snare))
						{
							W->cast(wtarget->get_position());
							console->print("[Fatality] Casting W on CC Target: %s", wtarget->get_base_skin_name().c_str());
						}
					}
				}
			}
		}
	}

	void ELogic()
	{
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			return;
		}

		if (settings::esettings::eCombo->get_bool())
		{
			if (E->is_ready())
			{
				auto etarget = target_selector->get_target(E->range(), damage_type::magical);
				if (etarget != nullptr)
				{
					if (etarget->is_valid())
					{
						auto epred = E->get_prediction(etarget);
						if (epred.hitchance >= getPredIntFromSettings(settings::hitchance::eHitChance->get_int()))
						{
							E->cast(epred.get_cast_position());
						}
					}
				}
			}
		}
	}

	void SemiELogic()
	{
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			return;
		}

		if (E->is_ready())
		{
			auto etarget = target_selector->get_target(E->range(), damage_type::magical);
			if (etarget != nullptr)
			{
				if (etarget->is_valid())
				{
					auto epred = E->get_prediction(etarget);
					if (epred.hitchance >= hit_chance::medium)
					{
						E->cast(epred.get_cast_position());
					}
				}
			}
		}
	}

	void AutoE()
	{
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			return;
		}

		if (settings::esettings::autoE->get_bool())
		{
			if (E->is_ready())
			{
				auto etarget = target_selector->get_target(E->range(), damage_type::magical);
				if (etarget != nullptr)
				{
					if (etarget->is_valid())
					{
						if (etarget->is_dashing())
						{
							if (etarget->get_path_controller()->get_end_vec().distance(myhero) <= E->range())
							{
								E->cast(etarget->get_path_controller()->get_end_vec());
								console->print("[Fatality] Casting E on Dash Target: %s", etarget->get_base_skin_name().c_str());
							}
						}

						if (etarget->has_buff_type(buff_type::Stun) || etarget->has_buff_type(buff_type::Snare))
						{
							E->cast(etarget->get_position());
							console->print("[Fatality] Casting E on CC Target: %s", etarget->get_base_skin_name().c_str());
						}
					}
				}
			}
		}
	}

	void RLogic()
	{
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			for (const auto& target : entitylist->get_enemy_heroes())
			{
				if (target != nullptr)
				{
					if (target->is_valid())
					{
						if (target->get_distance(myhero) <= R->range())
						{
							if (hud->get_hud_input_logic()->get_game_cursor_position().distance(target->get_position()) <= settings::rsettings::cursorrange->get_int())
							{
								if (!target->is_dashing())
								{
									if (canCastAgain())
									{
										auto rpred = R->get_prediction(target);
										if (rpred.hitchance >= getPredIntFromSettings(settings::hitchance::rHitChance->get_int()))
										{
											R->cast(rpred.get_cast_position());
											last_cast_time = std::chrono::steady_clock::now();
										}
									}
								}

								if (target->is_dashing())
								{
									R->cast(target->get_path_controller()->get_end_vec());
								}
							}
						}
					}
				}
			}
		}
	}

	void killstealQ(const game_object_script& enemy)
	{
		if (!Q->is_ready()) return;
		if (!settings::Killsteal::ksQ->get_bool()) return;
		if (!enemy->is_valid_target(Q->charged_max_range)) return;
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2"))) return;

		if (!myhero->has_buff(buff_hash("XerathArcanopulseChargeUp")))
		{
			if (enemy->get_real_health() <= QDamage(enemy))
			{
				if (!Q->is_charging())
				{
					Q->start_charging();
					return;
				}
			}
		}

		if (myhero->has_buff(buff_hash("XerathArcanopulseChargeUp")))
		{
			if (Q->range() > enemy->get_distance(myhero) + settings::qsettings::overchargevalue->get_int())
			{
				auto qpred = Q->get_prediction(enemy);
				if (qpred.hitchance >= hit_chance::high)
				{
					myhero->update_charged_spell(Q->get_slot(), qpred.get_cast_position(), true, false);
					console->print("Casting Q To Killsteal Target: %s", enemy->get_base_skin_name().c_str());
				}
			}
		}
	}

	void killstealW(const game_object_script& enemy)
	{
		if (!W->is_ready()) return;
		if (!settings::Killsteal::ksW->get_bool()) return;
		if (!enemy->is_valid_target(W->range())) return;
		if (Q->is_charging()) return;

		if (enemy->get_real_health() <= WDamage(enemy))
		{
			auto wpred = W->get_prediction(enemy);
			if (wpred.hitchance >= hit_chance::high)
			{
				W->cast(wpred.get_cast_position());
				console->print("Casting W To Killsteal Target: %s", enemy->get_base_skin_name().c_str());
			}
		}
	}

	void killstealE(const game_object_script& enemy)
	{
		if (!E->is_ready()) return;
		if (!settings::Killsteal::ksE->get_bool()) return;
		if (!enemy->is_valid_target(E->range())) return;
		if (Q->is_charging()) return;

		if (enemy->get_real_health() <= EDamage(enemy))
		{
			auto epred = E->get_prediction(enemy);
			if (epred.hitchance >= hit_chance::high)
			{
				E->cast(epred.get_cast_position());
				console->print("Casting E To Killsteal Target: %s", enemy->get_base_skin_name().c_str());
			}
		}
	}

	void killstealloop()
	{
		for (auto&& enemy : entitylist->get_enemy_heroes())
		{
			if (enemy == nullptr || !enemy->is_valid_target()) continue;

			killstealQ(enemy);
			killstealW(enemy);
			killstealE(enemy);
		}
	}

	void laneclear()
	{
		if (!settings::Clear::FarmToggle->get_bool())
		{
			return;
		}

		if (entitylist->get_enemy_minions().empty())
			return;

		std::vector<game_object_script> minions_list;
		minions_list.reserve(entitylist->get_enemy_minions().size());
		minions_list.insert(minions_list.end(), entitylist->get_enemy_minions().begin(), entitylist->get_enemy_minions().end());	

		for (auto& minionW : minions_list)
		{
			if (minionW->get_distance(myhero) > W->range())
				continue;


			if (settings::Clear::LcW->get_bool() && W->is_ready())
			{
				W->cast_on_best_farm_position(settings::Clear::LcWMinions->get_int(), false);
			}
		}
	}

	void jungleclear()
	{
		if (!settings::Clear::FarmToggle->get_bool())
		{
			return;
		}

		if (entitylist->get_jugnle_mobs_minions().empty())
			return;

		std::vector<game_object_script> minions_list;
		minions_list.reserve(entitylist->get_jugnle_mobs_minions().size());
		minions_list.insert(minions_list.end(), entitylist->get_jugnle_mobs_minions().begin(), entitylist->get_jugnle_mobs_minions().end());

		for (auto& Jungleminionq : minions_list)
		{
			if (Jungleminionq->get_distance(myhero) > Q->range())
				continue;


			if (settings::Clear::JcQ->get_bool() && Q->is_ready())
			{
				if (!myhero->has_buff(buff_hash("XerathArcanopulseChargeUp")))
				{
					if (!Q->is_charging())
					{
						Q->start_charging();
						return;
					}
				}

				if (myhero->has_buff(buff_hash("XerathArcanopulseChargeUp")))
				{
					if (Q->is_charging())
					{
						myhero->update_charged_spell(Q->get_slot(), Jungleminionq->get_position(), true, false);
					}
				}
			}
		}

		for (auto& Jungleminionw : minions_list)
		{
			if (Jungleminionw->get_distance(myhero) > W->range())
				continue;


			if (settings::Clear::JcW->get_bool() && W->is_ready())
			{
				W->cast(Jungleminionw->get_position());
			}
		}	
	}

	void onEnvDraw()
	{
		auto glow = glow_data(0.2, 0.6, 0.2, 0.6);

		if (settings::Draws::drawQ->get_bool())
		{
			if (Q->level() > 0)
			{
				draw_manager->add_circle_with_glow_gradient(myhero->get_position(), settings::Draws::drawQColor->get_color(), settings::Draws::drawQ2Color->get_color(), Q->charged_max_range, settings::Draws::drawQthickness->get_int(), glow);
			}
		}

		if (settings::Draws::drawW->get_bool())
		{
			if (W->level() > 0)
			{
				draw_manager->add_circle_with_glow_gradient(myhero->get_position(), settings::Draws::drawWColor->get_color(), settings::Draws::drawW2Color->get_color(), W->range(), settings::Draws::drawWthickness->get_int(), glow);
			}
		}

		if (settings::Draws::drawE->get_bool())
		{
			if (E->level() > 0)
			{
				draw_manager->add_circle_with_glow_gradient(myhero->get_position(), settings::Draws::drawEColor->get_color(), settings::Draws::drawE2Color->get_color(), E->range(), settings::Draws::drawEthickness->get_int(), glow);
			}
		}

		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			draw_manager->add_circle_with_glow_gradient(hud->get_hud_input_logic()->get_game_cursor_position(), MAKE_COLOR(255, 0, 255, 255), MAKE_COLOR(255, 0, 0, 255), settings::rsettings::cursorrange->get_int(), 2, glow);
		}
	}

	void OnRealDraw()
	{
		if (settings::Draws::drawR->get_bool())
		{
			if (R->level() > 0)
			{
				draw_manager->draw_circle_on_minimap(myhero->get_position(), R->range(), MAKE_COLOR(255, 0, 0, 255), 2, 100);
			}
		}

		for (const auto& target : entitylist->get_enemy_heroes())
		{
			if (!target->is_valid()) continue;

			if (target->is_visible_on_screen() && target->is_hpbar_recently_rendered() && !target->is_dead())
			{
				if (settings::Draws::Damage->get_bool())
				{
					fatalityutils::draw_dmg_rl(target, GetDamage(target), MAKE_COLOR(255, 170, 0, 150));
				}

				if (settings::Draws::drawAA->get_bool())
				{
					fatalityutils::AATracker();
				}
			}
		}

		if (settings::Draws::drawKill->get_bool())
		{
			if (R->level() > 0 && R->is_ready())
			{
				int index = 0;
				for (const auto& target : entitylist->get_enemy_heroes())
				{
					if (target != nullptr)
					{
						if (!target->is_dead() && target->is_hpbar_recently_rendered())
						{
							if (R->level() == 1)
							{
								if (RDamage(target) * 4 >= target->get_real_health())
								{
									int num = (int)target->get_real_health();
									std::string numstr = std::to_string(num);
									int firstdigit = std::stoi(numstr.substr(0, 4));
									const auto key = index++;
									const auto position = vector(208.f, 551.f + (key * 30));
									draw_manager->add_text_on_screen(position, MAKE_COLOR(255, 0, 0, 255), settings::Draws::rkillableTextSize->get_int(), "%s is Killable HP Left: %i", target->get_base_skin_name().c_str(), firstdigit);
								}
							}

							if (R->level() == 2)
							{
								if (RDamage(target) * 5 >= target->get_real_health())
								{
									int num = (int)target->get_real_health();
									std::string numstr = std::to_string(num);
									int firstdigit = std::stoi(numstr.substr(0, 4));
									const auto key = index++;
									const auto position = vector(208.f, 551.f + (key * 30));
									draw_manager->add_text_on_screen(position, MAKE_COLOR(255, 0, 0, 255), settings::Draws::rkillableTextSize->get_int(), "%s is Killable HP Left: %i", target->get_base_skin_name().c_str(), firstdigit);
								}
							}

							if (R->level() == 3)
							{
								if (RDamage(target) * 6 >= target->get_real_health())
								{
									int num = (int)target->get_real_health();
									std::string numstr = std::to_string(num);
									int firstdigit = std::stoi(numstr.substr(0, 4));
									const auto key = index++;
									const auto position = vector(208.f, 551.f + (key * 30));
									draw_manager->add_text_on_screen(position, MAKE_COLOR(255, 0, 0, 255), settings::Draws::rkillableTextSize->get_int(), "%s is Killable HP Left: %i", target->get_base_skin_name().c_str(), firstdigit);
								}
							}
						}
					}
				}
			}
		}
	}

	void update()
	{
		if (myhero->has_buff(buff_hash("XerathLocusOfPower2")))
		{
			orbwalker->set_attack(false);
			orbwalker->set_movement(false);
			evade->disable_evade();
		}
		else
		{
			orbwalker->set_attack(true);
			orbwalker->set_movement(true);
			evade->enable_evade();
		}

		if (orbwalker->lane_clear_mode())
		{
			laneclear();
			jungleclear();
		}

		if (orbwalker->combo_mode())
		{
			WLogic();
			QLogic();
			ELogic();
		}

		if (settings::Keys::semiE->get_bool())
		{
			myhero->issue_order(hud->get_hud_input_logic()->get_game_cursor_position());
			SemiELogic();
		}

		if (settings::rsettings::rMode->get_int() == 0)
		{
			RLogic();
		}

		if (settings::rsettings::rMode->get_int() == 1)
		{
			if (settings::Keys::rKey->get_bool())
			{
				RLogic();
			}
		}
	}

	void Preupdate()
	{
		killstealloop();
		AutoW();
		AutoE();
	}

	void load()
	{
		Q = plugin_sdk->register_spell(spellslot::q, 700.f);
		Q->set_skillshot(0.55f, 70.f, FLT_MAX, {}, skillshot_type::skillshot_line);
		Q->set_charged(700.f, 1450.f, 1.75f);

		W = plugin_sdk->register_spell(spellslot::w, 1000.f);
		W->set_skillshot(0.75f, 125.f, FLT_MAX, {}, skillshot_type::skillshot_circle);

		E = plugin_sdk->register_spell(spellslot::e, 1125);
		E->set_skillshot(0.25f, 60.f, 1400.f, { collisionable_objects::minions, collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);

		R = plugin_sdk->register_spell(spellslot::r, 5000);
		R->set_skillshot(0.62f, 170.f, FLT_MAX, {}, skillshot_type::skillshot_circle);

		createmenu();

		antigapcloser::add_event_handler(antigap);
		event_handler<events::on_preupdate>::add_callback(Preupdate, event_prority::high);
		event_handler<events::on_update>::add_callback(update, event_prority::high);
		event_handler<events::on_env_draw>::add_callback(onEnvDraw, event_prority::high);
		event_handler<events::on_draw>::add_callback(OnRealDraw, event_prority::high);
		event_handler<events::on_process_spell_cast>::add_callback(onprocess, event_prority::high);
	}

	void unload()
	{
		plugin_sdk->remove_spell(Q);
		plugin_sdk->remove_spell(W);
		plugin_sdk->remove_spell(E);
		plugin_sdk->remove_spell(R);

		antigapcloser::remove_event_handler(antigap);
		event_handler<events::on_update>::remove_handler(update);
		event_handler<events::on_preupdate>::remove_handler(Preupdate);
		event_handler<events::on_env_draw>::remove_handler(onEnvDraw);
		event_handler<events::on_draw>::remove_handler(OnRealDraw);
		event_handler<events::on_process_spell_cast>::remove_handler(onprocess);

		misc().destroy();
	}

	float qdamage[] = { 0.f, 70.f, 110.f, 150.f, 190.f, 230.f };

	float QDamage(const game_object_script& target)
	{
		if (Q->level() == 0) return 0.f;
		damage_input input;
		input.raw_magical_damage = qdamage[Q->level()] + 0.85 * myhero->get_total_ability_power();
		const float damage = damagelib->calculate_damage_on_unit(myhero, target, &input);
		return damage;
	}

	float wdamage[] = { 0.f, 100.02f, 158.365f, 216.71f, 275.055f, 333.4f };

	float WDamage(const game_object_script& target)
	{
		if (W->level() == 0) return 0.f;
		damage_input input;
		input.raw_magical_damage = wdamage[W->level()] + 1.02f * myhero->get_total_ability_power();
		const float damage = damagelib->calculate_damage_on_unit(myhero, target, &input);
		return damage;
	}

	float edamage[] = { 0.f, 80.f, 110.f, 140.f, 170.f, 200.f };

	float EDamage(const game_object_script& target)
	{
		if (E->level() == 0) return 0.f;
		damage_input input;
		input.raw_magical_damage = edamage[E->level()] + 0.45f * myhero->get_total_ability_power();
		const float damage = damagelib->calculate_damage_on_unit(myhero, target, &input);
		return damage;
	}

	float rdamage[] = { 0.f, 180.f, 230.f, 280.f };
	float rrampup[] = { 0.f, 20.f, 25.f, 30.f };

	float RDamage(const game_object_script& target)
	{
		if (R->level() == 0) return 0.f;
		damage_input input;
		input.raw_magical_damage = rdamage[R->level()] + 0.40f * myhero->get_total_ability_power() + rrampup[R->level()] * myhero->get_buff_count(buff_hash("xerathrrampup"));
		const float damage = damagelib->calculate_damage_on_unit(myhero, target, &input);
		return damage;
	}

	float GetDamage(const game_object_script& target)
	{
		auto Damage = 0;

		if (Q->is_ready())
		{
			Damage += QDamage(target);
		}

		if (W->is_ready())
		{
			Damage += WDamage(target);
		}

		if (E->is_ready())
		{
			Damage += EDamage(target);
		}

		if (R->is_ready())
		{
			if (R->level() == 1)
			{
				Damage += RDamage(target) * 4;
			}
			else if (R->level() == 2)
			{
				Damage += RDamage(target) * 5;
			}
			else if (R->level() == 3)
			{
				Damage += RDamage(target) * 6;
			}
		}

		return Damage;
	}

	void antigap(game_object_script sender, antigapcloser::antigapcloser_args* args)
	{
		if (myhero->get_health_percent() > settings::Misc::AgHealth->get_int())
		{
			return;
		}

		if (sender != nullptr)
		{
			if (sender->has_buff_type(buff_type::SpellImmunity) || sender->has_buff_type(buff_type::SpellShield))
			{
				return;
			}
		}

		if (settings::Misc::ag->get_bool())
		{
			if (E->is_ready())
			{
				if (sender != nullptr)
				{
					if (sender->is_enemy() && sender->is_valid())
					{
						if (args->end_position.distance(myhero) <= 350)
						{
							E->cast(args->end_position);
							console->print("[Fatality] Casting E on Gap Closer End Position Sender: %s", sender->get_base_skin_name().c_str());
						}
					}
				}
			}
		}
	}

	void Interrupter()
	{
		if (settings::Misc::interrupter->get_bool())
		{
			for (auto& enemy : entitylist->get_enemy_heroes())
			{
				if (E->is_ready())
				{
					if (enemy->is_enemy() && enemy->is_casting_interruptible_spell() == 2)
					{
						if (enemy->get_distance(myhero) <= E->range())
						{
							auto epred = E->get_prediction(enemy);
							if (epred.hitchance >= hit_chance::high)
							{
								E->cast(epred.get_cast_position());
								console->print("[Spaceglide] Interrupting %s", enemy->get_base_skin_name().c_str());
							}
						}
					}
				}
			}
		}
	}

	bool canCastAgain()
	{
		auto now = std::chrono::steady_clock::now();
		auto duration_since_last_cast = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_cast_time).count();
		return duration_since_last_cast >= settings::rsettings::rdelay->get_int();
	}

	void onprocess(game_object_script sender, spell_instance_script spell)
	{
		if (settings::Misc::ag->get_bool())
		{
			if (E->is_ready())
			{
				if (sender != nullptr)
				{
					if (sender->get_base_skin_name() == "Alistar")
					{
						if (spell->get_spellslot() == spellslot::w)
						{
							if (spell->get_last_target_id() == myhero->get_id())
							{
								E->cast(sender);
								console->print("[Spaceglide] Casting E on Alistar W");
							}
						}
					}

					if (sender->get_base_skin_name() == "Jax")
					{
						if (spell->get_spellslot() == spellslot::q)
						{
							if (spell->get_last_target_id() == myhero->get_id())
							{
								E->cast(sender);
								console->print("[Spaceglide] Casting E on Jax Q");
							}
						}
					}

					if (sender->get_base_skin_name() == "Pantheon")
					{
						if (spell->get_spellslot() == spellslot::w)
						{
							if (spell->get_last_target_id() == myhero->get_id())
							{
								E->cast(sender);
								console->print("[Spaceglide] Casting E on Pantheon W");
							}
						}
					}

					if (sender->get_base_skin_name() == "Briar")
					{
						if (spell->get_spellslot() == spellslot::q)
						{
							if (spell->get_last_target_id() == myhero->get_id())
							{
								E->cast(sender);
								console->print("[Spaceglide] Casting E on Briar Q");
							}
						}
					}
				}
			}
		}
	}
}