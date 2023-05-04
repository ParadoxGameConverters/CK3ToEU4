#include "GeneratedReligion.h"

void EU4::GeneratedReligion::outputGUI(std::ostream& output) const
{
	output << "guiTypes = {";
	output << "\n\t################################################################################################";
	output << "\n\t"
			 << "################# Church aspect item (for selection) For " << name;
	output << "\n\t"
			 << "windowType = {";
	output << "\n\t\t"
			 << "name = \"countryreligionview_aspectitem_" << name << "\"";
	output << "\n\t\t"
			 << "backGround =\"\"";
	output << "\n\t\t"
			 << "position = { x=10 y=-20 }";
	output << "\n\t\t"
			 << "size = { x=50 y=46 }";
	output << "\n\t\t"
			 << "moveable = 0";
	output << "\n\t\t"
			 << "dontRender =\"\"";
	output << "\n\t\t"
			 << "horizontalBorder =\"\"";
	output << "\n\t\t"
			 << "Orientation = \"UPPER_LEFT\"";
	output << "\n";
	output << "\n\t\t"
			 << "iconType = {";
	output << "\n\t\t\t"
			 << "name = \"aspect_icon\"";
	output << "\n\t\t\t"
			 << "spriteType = \"GFX_hindu_deities_strip\"";
	output << "\n\t\t\t"
			 << "position = { x= 45 y = -14 }";
	output << "\n\t\t"
			 << "}";
	output << "\n";
	output << "\n\t\t"
			 << "instantTextBoxType = {";
	output << "\n\t\t\t"
			 << "name = \"aspect_name\"";
	output << "\n\t\t\t"
			 << "position = { x= -42 y = 6 }";
	output << "\n\t\t\t"
			 << "font = \"vic_18\"";
	output << "\n\t\t\t"
			 << "borderSize = {x = 0 y = 0}";
	output << "\n\t\t\t"
			 << "text = \"AspectName\"";
	output << "\n\t\t\t"
			 << "maxWidth = 102";
	output << "\n\t\t\t"
			 << "maxHeight = 24";
	output << "\n\t\t\t"
			 << "Orientation = \"UPPER_LEFT\"";
	output << "\n\t\t\t"
			 << "format = left";
	output << "\n\t\t"
			 << "}";
	output << "\n";
	output << "\n\t\t"
			 << "instantTextBoxType = {";
	output << "\n\t\t\t"
			 << "name = \"aspect_effect\"";
	output << "\n\t\t\t"
			 << "position = { x= 98 y = -2 }";
	output << "\n\t\t\t"
			 << "font = \"vic_18\"";
	output << "\n\t\t\t"
			 << "borderSize = {x = 0 y = 0}";
	output << "\n\t\t\t"
			 << "text = \"AspectEffect\"";
	output << "\n\t\t\t"
			 << "maxWidth = 206";
	output << "\n\t\t\t"
			 << "maxHeight = 24";
	output << "\n\t\t\t"
			 << "Orientation = \"UPPER_LEFT\"";
	output << "\n\t\t\t"
			 << "format = left";
	output << "\n\t\t"
			 << "}";
	output << "\n";
	output << "\n\t\t"
			 << "guiButtonType = {";
	output << "\n\t\t\t"
			 << "name = \"pick_aspect\"";
	output << "\n\t\t\t"
			 << "position = { x= 301 y = 1 }";
	output << "\n\t\t\t"
			 << "quadTextureSprite = \"GFX_standard_button_71\"";
	output << "\n\t\t\t"
			 << "buttonText = \"SELECT\"";
	output << "\n\t\t\t"
			 << "font = \"vic_18\"";
	output << "\n\t\t\t"
			 << "Orientation = \"LEFT\"";
	output << "\n\t\t"
			 << "}";
	output << "\n\t"
			 << "}";
	output << "\n"
			 << "}";
}

void EU4::GeneratedReligion::outputPersonalDeities(std::ostream& output) const
{
	if (parent == "germanic_pagan")
	{
		// Tiwaz
		output << "tiwaz_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "discipline = 0.05";
		output << "\n\t"
				 << "infantry_power = 0.10";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 49";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 3";
		output << "\n\t\t\t"
				 << "personality = ai_militarist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_diplomat";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Wodan
		output << "wodan_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "land_morale = 0.1";
		output << "\n\t"
				 << "core_creation = -0.10";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 50";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 3";
		output << "\n\t\t\t"
				 << "personality = ai_militarist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_diplomat";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Dunar
		output << "dunar_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "defensiveness = 0.2";
		output << "\n\t"
				 << "leader_land_shock = 1";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 51";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 3";
		output << "\n\t\t\t"
				 << "personality = ai_militarist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_diplomat";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Frijjo
		output << "frijjo_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "diplomatic_upkeep = 1";
		output << "\n\t"
				 << "improve_relation_modifier = 0.10";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 52";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 3";
		output << "\n\t\t\t"
				 << "personality = ai_diplomat";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Austro
		output << "austro_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "production_efficiency = 0.1";
		output << "\n\t"
				 << "manpower_recovery_speed = 0.20";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 53";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 3";
		output << "\n\t\t\t"
				 << "personality = ai_capitalist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_militarist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Fraujaz
		output << "fraujaz_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "global_trade_goods_size_modifier = 0.1";
		output << "\n\t"
				 << "land_attrition = -0.10";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 54";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_capitalist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
	}
	else if (parent == "norse_pagan")
	{
		// Odin
		output << "odin_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "legitimacy = 0.1";
		output << "\n\t"
				 << "horde_unity = 0.1";
		output << "\n\t"
				 << "core_creation = -0.10";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 7";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_militarist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_diplomat";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Freya
		output << "freya_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "global_manpower_modifier = 0.1";
		output << "\n\t"
				 << "global_tax_modifier = 0.1";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 8";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_balanced";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_colonialist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Tor
		output << "tor_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "leader_land_shock = 1";
		output << "\n\t"
				 << "defensiveness = 0.1";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 9";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "NOT = { num_of_cities = 5 }";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "NOT = { num_of_cities = 10 }";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "NOT = { num_of_cities = 30 }";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_militarist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_capitalist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Tyr
		output << "tyr_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "global_sailors_modifier = 0.10";
		output << "\n\t"
				 << "discipline = 0.05";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 10";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_balanced";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_colonialist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Njord
		output << "njord_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "trade_efficiency = 0.1";
		output << "\n\t"
				 << "global_trade_goods_size_modifier = 0.1";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 11";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_capitalist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_colonialist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_militarist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
		output << "\n\n";
		// Snotra
		output << "snotra_" << personalDeityNumber << "{";
		output << "\n\n\t"
				 << "technology_cost = -0.05";
		output << "\n\t"
				 << "build_cost = -0.1";
		output << "\n\n\t"
				 << "allow = { religion = " << name << " }";
		output << "\n\t"
				 << "sprite = 12";
		output << "\n\n\t"
				 << "ai_will_do = {";
		output << "\n\t\t"
				 << "factor = 1";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 2";
		output << "\n\t\t\t"
				 << "personality = ai_diplomat";
		output << "\n\t\t"
				 << "}";
		output << "\n\t\t"
				 << "modifier = {";
		output << "\n\t\t\t"
				 << "factor = 0.5";
		output << "\n\t\t\t"
				 << "personality = ai_militarist";
		output << "\n\t\t"
				 << "}";
		output << "\n\t"
				 << "}";
		output << "\n"
				 << "}";
	}
}

void EU4::GeneratedReligion::outputRebels(std::ostream& output) const
{
	output << name << "_rebels = {";
	output << "\n\n\t"
			 << "area = nation";
	output << "\n\t"
			 << "government = theocracy";
	output << "\n\t"
			 << "religion = " << name;
	output << "\n\t"
			 << "defection = religion";
	output << "\n\t"
			 << "defect_delay = 120";
	output << "\n\t"
			 << "independence = none";
	output << "\n\t"
			 << "unit_transfer = yes";
	output << "\n\t"
			 << "gfx_type = culture_province";
	output << "\n\n\t"
			 << "resilient = no";
	output << "\n\t"
			 << "reinforcing = yes";
	output << "\n\t"
			 << "general = yes";
	output << "\n\t"
			 << "smart = yes";
	output << "\n\n\t"
			 << "artillery = 0.0";
	output << "\n\t"
			 << "infantry = 0.7";
	output << "\n\t"
			 << "cavalry = 0.3";
	output << "\n\n\t"
			 << "morale = 1.1";
	output << "\n\n\t"
			 << "handle_action_negotiate = yes";
	output << "\n\t"
			 << "handle_action_stability = yes";
	output << "\n\t"
			 << "handle_action_build_core = yes";
	output << "\n\t"
			 << "handle_action_send_missionary = yes";
	output << "\n\n\t"
			 << "spawn_chance = {";
	output << "\n\t\t"
			 << "factor = 1";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 0.5";
	output << "\n\t\t\t"
			 << "is_overseas = yes";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 0.0";
	output << "\n\t\t\t"
			 << "owner = { religion = ROOT }";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 0.0";
	output << "\n\t\t\t"
			 << "NOT = { religion = " << name << " }";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 0.001";
	output << "\n\t\t\t"
			 << "tolerance_to_this = 5";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 0.5";
	output << "\n\t\t\t"
			 << "tolerance_to_this = 2";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 5.0";
	output << "\n\t\t\t"
			 << "owner = { has_country_flag = religious_turmoil }";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 2000";
	output << "\n\t\t\t"
			 << "has_missionary = yes";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 2";
	output << "\n\t\t\t"
			 << "NOT = { tolerance_to_this = 1 }";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 2";
	output << "\n\t\t\t"
			 << "NOT = { tolerance_to_this = -1 }";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 2";
	output << "\n\t\t\t"
			 << "NOT = { tolerance_to_this = -2 }";
	output << "\n\t\t"
			 << "}";
	output << "\n\t"
			 << "}";
	output << "\n\n\t"
			 << "movement_evaluation = {";
	output << "\n\t\t"
			 << "factor = 1";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 0.5";
	output << "\n\t\t\t"
			 << "units_in_province = 1";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 0.001";
	output << "\n\t\t\t"
			 << "controlled_by = REB";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 0.0";
	output << "\n\t\t\t"
			 << "religion = " << name;
	output << "\n\t\t\t"
			 << "owner = { religion = " << name << " }";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 2.0";
	output << "\n\t\t\t"
			 << "NOT = { religion = " << name << " }";
	output << "\n\t\t\t"
			 << "any_neighbor_province = {";
	output << "\n\t\t\t\t"
			 << "religion = " << name;
	output << "\n\t\t\t\t"
			 << "NOT = { has_province_modifier = \"forced_conversion\" }";
	output << "\n\t\t\t"
			 << "}";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 1.5";
	output << "\n\t\t\t"
			 << "unrest = 2";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 1.5";
	output << "\n\t\t\t"
			 << "unrest = 4";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "modifier = {";
	output << "\n\t\t\t"
			 << "factor = 1.5";
	output << "\n\t\t\t"
			 << "unrest = 6";
	output << "\n\t\t"
			 << "}";
	output << "\n\t"
			 << "}";
	output << "\n\n\t"
			 << "siege_won_trigger = {";
	output << "\n\t\t"
			 << "NOT = {";
	output << "\n\t\t\t"
			 << "AND = {";
	output << "\n\t\t\t\t"
			 << "owned_by = PAP";
	output << "\n\t\t\t\t"
			 << "is_capital = yes";
	output << "\n\t\t\t"
			 << "}";
	output << "\n\t\t"
			 << "}";
	output << "\n\t"
			 << "}";
	output << "\n\t"
			 << "siege_won_effect = {";
	output << "\n\t\t"
			 << "if = {";
	output << "\n\t\t\t"
			 << "limit = { religion = " << name << " }";
	output << "\n\t\t\t"
			 << "add_province_modifier = {";
	output << "\n\t\t\t\t"
			 << "name = \"religious_violence\"";
	output << "\n\t\t\t\t"
			 << "duration = 730";
	output << "\n\t\t\t"
			 << "}";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "if = {";
	output << "\n\t\t\t"
			 << "limit = { NOT = { religion = " << name << " } }";
	output << "\n\t\t\t"
			 << "change_religion = " << name;
	output << "\n\t\t\t"
			 << "add_province_modifier = {";
	output << "\n\t\t\t\t"
			 << "name = \"religious_violence\"";
	output << "\n\t\t\t\t"
			 << "duration = 730";
	output << "\n\t\t\t"
			 << "}";
	output << "\n\t\t"
			 << "}";
	output << "\n\t"
			 << "}";
	output << "\n\n\t"
			 << "can_negotiate_trigger = {";
	output << "\n\t\t"
			 << "religion_group = " << religionGroup;
	output << "\n\t"
			 << "}";
	output << "\n\n\t"
			 << "can_enforce_trigger = {";
	output << "\n\t\t"
			 << "always = yes";
	output << "\n\t"
			 << "}";
	output << "\n\n\t"
			 << "demands_description = \"generic_rebels_demand\"";
	output << "\n\n\t"
			 << "demands_enforced_effect = {";
	output << "\n\t\t"
			 << "if = {";
	output << "\n\t\t\t"
			 << "limit = { NOT = { religion_group = " << religionGroup << " } }";
	output << "\n\t\t\t"
			 << "add_prestige = -50";
	output << "\n\t\t\t"
			 << "every_owned_province = {";
	output << "\n\t\t\t\t"
			 << "limit = { has_rebel_faction = REB }";
	output << "\n\t\t\t\t"
			 << "add_local_autonomy = 20";
	output << "\n\t\t\t"
			 << "}";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "if = {";
	output << "\n\t\t\t"
			 << "limit = { tag = PAP }";
	output << "\n\t\t\t"
			 << "add_stability = -1";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "if = {";
	output << "\n\t\t\t"
			 << "limit = { religion = " << name << " }";
	output << "\n\t\t\t"
			 << "add_country_modifier  = {";
	output << "\n\t\t\t\t"
			 << "name = \"religious_intolerance\"";
	output << "\n\t\t\t\t"
			 << "duration = 3650";
	output << "\n\t\t\t"
			 << "}";
	output << "\n\t\t"
			 << "}";
	output << "\n\t\t"
			 << "if = {";
	output << "\n\t\t\t"
			 << "limit = {";
	output << "\n\t\t\t\t"
			 << "religion_group = " << religionGroup;
	output << "\n\t\t\t\t"
			 << "NOT = { religion = " << name << " }";
	output << "\n\t\t\t"
			 << "}";
	output << "\n\t\t\t"
			 << "change_religion = " << name;
	output << "\n\t\t\t"
			 << "force_converted = yes";
	output << "\n\t\t"
			 << "}";
	output << "\n\t"
			 << "}";
	output << "\n"
			 << "}";
}

void EU4::GeneratedReligion::outputSounds(std::ostream& output) const
{
	output << "sound = {";
	output << "\n\t"
			 << "name = religion_" << name;
	output << "\n\t"
			 << "file = ";
	if (religionGroup == "christian" || religionGroup == "gnostic")
	{
		output << "\"tab_religion_christian.wav\"";
		output << "\n"
				 << "}";
		output << "\n"
				 << "sound = {";
		output << "\n\t"
				 << "name = start_convert_" << name;
		output << "\n\t"
				 << "file = ";
		if (unique == "fervor")
			output << "\"start_conversion_christian_reformed.wav\"";
		else if (unique == "has_patriarchs")
			output << "\"start_conversion_christian_orthodox.wav\"";
		else if (parent != "catholic")
			output << "\"start_conversion_christian_protestant.wav\"";
		else
			output << "\"start_conversion_christian_catholic.wav\"";
	}
	else if (religionGroup == "muslim" || religionGroup == "yazidism")
	{
		output << "\"tab_religion_muslim.wav\"";
		output << "\n"
				 << "}";
		output << "\n"
				 << "sound = {";
		output << "\n\t"
				 << "name = start_convert_" << name;
		output << "\n\t"
				 << "file = ";
		if (religionGroup == "yazidism")
			output << "\"start_conversion_zoroastrian.wav\"";
		else if (parent == "ashari" || parent == "muwalladi" || parent == "maturidi" || parent == "masmudi" || parent == "mutazila" || parent == "quranist")
			output << "\"start_conversion_muslim_sunni.wav\"";
		else
			output << "\"start_conversion_muslim_shiite.wav\"";
	}
	else if (religionGroup == "zoroastrian_group")
	{
		output << "\"start_conversion_zoroastrian.wav\"";
		output << "\n"
				 << "}";
		output << "\n"
				 << "sound = {";
		output << "\n\t"
				 << "name = start_convert_" << name;
		output << "\n\t"
				 << "file = \"start_conversion_zoroastrian.wav\"";
	}
	else if (religionGroup == "jewish_group")
	{
		output << "\"start_conversion_jewish.wav\"";
		output << "\n"
				 << "}";
		output << "\n"
				 << "sound = {";
		output << "\n\t"
				 << "name = start_convert_" << name;
		output << "\n\t"
				 << "file = \"start_conversion_jewish.wav\"";
	}
	else if (religionGroup == "dharmic")
	{
		output << "\"tab_religion_india.wav\"";
		output << "\n"
				 << "}";
		output << "\n"
				 << "sound = {";
		output << "\n\t"
				 << "name = start_convert_" << name;
		output << "\n\t"
				 << "file = \"start_conversion_eastern_hinduism.wav\"";
	}
	else if (religionGroup == "eastern")
	{
		output << "\"tab_religion_eastern.wav\"";
		output << "\n"
				 << "}";
		output << "\n"
				 << "sound = {";
		output << "\n\t"
				 << "name = start_convert_" << name;
		output << "\n\t"
				 << "file = ";
		if (parent == "theravada" || parent == "mahayana" || parent == "vajrayana" || parent == "ari" || parent == "lamaism")
			output << "\"start_conversion_eastern_buddhism.wav\"";
		else
			output << "\"start_conversion_eastern_confucianism.wav\"";
	}
	else // Should only be Pagans, but just in case I'm leaving this open
	{
		if (parent == "norse_pagan")
			output << "\"start_conversion_norse.wav\"";
		else
			output << "\"tab_religion_pagan.wav\"";
		output << "\n"
				 << "}";
		output << "\n"
				 << "sound = {";
		output << "\n\t"
				 << "name = start_convert_" << name;
		output << "\n\t"
				 << "file = ";
		if (parent == "norse_pagan" || parent == "baltic_pagan")
			output << "\"start_conversion_norse.wav\"";
		else if (parent == "waaqism_pagan" || parent == "west_african_bori_pagan" || parent == "magyar_pagan" || parent == "west_african_bidu_pagan" ||
					parent == "siberian_pagan" || parent == "tengri_pagan" || parent == "slavic_pagan" || parent == "west_african_roog_pagan" ||
					parent == "finnish_pagan" || parent == "west_african_pagan" || parent == "west_african_orisha_pagan" || parent == "akom_pagan")
			output << "\"start_conversion_pagan_shamanism.wav\"";
		else
			output << "\"start_conversion_pagan_animism.wav\"";
	}
	output << "\n"
			 << "}";
}