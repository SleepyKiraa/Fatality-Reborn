#include "plugin_sdk/plugin_sdk.hpp"

namespace xerath
{
	inline bool canCastAgain();
	inline void onprocess(game_object_script sender, spell_instance_script spell);
	inline void antigap(game_object_script sender, antigapcloser::antigapcloser_args* args);
	inline void Interrupter();
	inline void laneclear();
	inline void jungleclear();
	inline float QDamage(const game_object_script& target);
	inline float WDamage(const game_object_script& target);
	inline float EDamage(const game_object_script& target);
	inline float RDamage(const game_object_script& target);
	inline float GetDamage(const game_object_script& target);
	inline void Preupdate();
	inline void RLogic();
	inline void QLogic();
	inline void WLogic();
	inline void AutoW();
	inline void AutoE();
	inline void ELogic();
	inline void SemiELogic();
	inline void killstealQ(const game_object_script& enemy);
	inline void killstealW(const game_object_script& enemy);
	inline void killstealE(const game_object_script& enemy);
	inline void killstealloop();
	inline void update();	
	inline void onEnvDraw();
	inline void OnRealDraw();
	inline void createmenu();
	inline hit_chance getPredIntFromSettings(int hitchance);
	void load();
	void unload();
}
