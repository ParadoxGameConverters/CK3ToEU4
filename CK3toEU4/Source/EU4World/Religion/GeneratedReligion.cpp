#include "GeneratedReligion.h"

void EU4::GeneratedReligion::returnRebels(std::ostream& output) const
{
	output << name << "_rebels = {";
	output << "\n\n\t" << "area = nation";
	output << "\n\t" << "government = theocracy";
	output << "\n\t" << "religion = " << name;
	output << "\n\t" << "defection = religion";
	output << "\n\t" << "defect_delay = 120";
	output << "\n\t" << "independence = none";
	output << "\n\t" << "unit_transfer = yes";
	output << "\n\t" << "gfx_type = culture_province";
	output << "\n\n\t" << "resilient = no";
	output << "\n\t" << "reinforcing = yes";
	output << "\n\t" << "general = yes";
	output << "\n\t" << "smart = yes";
	output << "\n\n\t" << "artillery = 0.0";
	output << "\n\t" << "infantry = 0.7";
	output << "\n\t" << "cavalry = 0.3";
	output << "\n\n\t" << "morale = 1.1";
	output << "\n\n\t" << "handle_action_negotiate = yes";
	output << "\n\t" << "handle_action_stability = yes";
	output << "\n\t" << "handle_action_build_core = yes";
	output << "\n\t" << "handle_action_send_missionary = yes";
	output << "\n\n\t" << "spawn_chance = {";
	output << "\n\t\t" << "factor = 1";
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 0.5";
	output << "\n\t\t\t" << "is_overseas = yes";
	output << "\n\t\t" << "}";
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 0.0";
	output << "\n\t\t\t" << "owner = { religion = ROOT }";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 0.0";
	output << "\n\t\t\t" << "NOT = { religion = " << name << " }";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 0.001";
	output << "\n\t\t\t" << "tolerance_to_this = 5";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 0.5";
	output << "\n\t\t\t" << "tolerance_to_this = 2";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 5.0";
	output << "\n\t\t\t" << "owner = { has_country_flag = religious_turmoil }";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 2000";
	output << "\n\t\t\t" << "has_missionary = yes";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 2";
	output << "\n\t\t\t" << "NOT = { tolerance_to_this = 1 }";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 2";
	output << "\n\t\t\t" << "NOT = { tolerance_to_this = -1 }";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 2";
	output << "\n\t\t\t" << "NOT = { tolerance_to_this = -2 }";
	output << "\n\t\t" << "}";	
	output << "\n\t" << "}";
	output << "\n\n\t" << "movement_evaluation = {";
	output << "\n\t\t" << "factor = 1";
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 0.5";
	output << "\n\t\t\t" << "units_in_province = 1";
	output << "\n\t\t" << "}";
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 0.001";
	output << "\n\t\t\t" << "controlled_by = REB";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 0.0";
	output << "\n\t\t\t" << "religion = " << name;
	output << "\n\t\t\t" << "owner = { religion = " << name << " }";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 2.0";
	output << "\n\t\t\t" << "NOT = { religion = " << name << " }";
	output << "\n\t\t\t" << "any_neighbor_province = {";
	output << "\n\t\t\t\t" << "religion = " << name;
	output << "\n\t\t\t\t" << "NOT = { has_province_modifier = \"forced_conversion\" }";
	output << "\n\t\t\t" << "}";	
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 1.5";
	output << "\n\t\t\t" << "unrest = 2";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 1.5";
	output << "\n\t\t\t" << "unrest = 4";
	output << "\n\t\t" << "}";	
	output << "\n\t\t" << "modifier = {";
	output << "\n\t\t\t" << "factor = 1.5";
	output << "\n\t\t\t" << "unrest = 6";
	output << "\n\t\t" << "}";	
	output << "\n\t" << "}";
	output << "\n\n\t" << "siege_won_trigger = {";
	output << "\n\t\t" << "NOT = {";
	output << "\n\t\t\t" << "AND = {";
	output << "\n\t\t\t\t" << "owned_by = PAP";
	output << "\n\t\t\t\t" << "is_capital = yes";
	output << "\n\t\t\t" << "}";
	output << "\n\t\t" << "}";
	output << "\n\t" << "}";
	output << "\n\t" << "siege_won_effect = {";
	output << "\n\t\t" << "if = {";
	output << "\n\t\t\t" << "limit = { religion = " << name << " }";
	output << "\n\t\t\t" << "add_province_modifier = {";
	output << "\n\t\t\t\t" << "name = \"religious_violence\"";
	output << "\n\t\t\t\t" << "duration = 730";
	output << "\n\t\t\t" << "}";
	output << "\n\t\t" << "}";
	output << "\n\t\t" << "if = {";
	output << "\n\t\t\t" << "limit = { NOT = { religion = " << name << " } }";
	output << "\n\t\t\t" << "change_religion = " << name;
	output << "\n\t\t\t" << "add_province_modifier = {";
	output << "\n\t\t\t\t" << "name = \"religious_violence\"";
	output << "\n\t\t\t\t" << "duration = 730";
	output << "\n\t\t\t" << "}";
	output << "\n\t\t" << "}";
	output << "\n\t" << "}";
	output << "\n\n\t" << "can_negotiate_trigger = {";
	output << "\n\t\t" << "religion_group = " << religionGroup;
	output << "\n\t" << "}";
	output << "\n\n\t" << "can_enforce_trigger = {";
	output << "\n\t\t" << "always = yes";
	output << "\n\t" << "}";
	output << "\n\n\t" << "demands_description = \"generic_rebels_demand\"";
	output << "\n\n\t" << "demands_enforced_effect = {";
	output << "\n\t\t" << "if = {";
	output << "\n\t\t\t" << "limit = { NOT = { religion_group = " << religionGroup << " } }";
	output << "\n\t\t\t" << "add_prestige = -50";
	output << "\n\t\t\t" << "every_owned_province = {";
	output << "\n\t\t\t\t" << "limit = { has_rebel_faction = REB }";
	output << "\n\t\t\t\t" << "add_local_autonomy = 20";
	output << "\n\t\t\t" << "}";
	output << "\n\t\t" << "}";
	output << "\n\t\t" << "if = {";
	output << "\n\t\t\t" << "limit = { tag = PAP }";
	output << "\n\t\t\t" << "add_stability = -1";
	output << "\n\t\t" << "}";
	output << "\n\t\t" << "if = {";
	output << "\n\t\t\t" << "limit = { religion = " << name << " }";
	output << "\n\t\t\t" << "add_country_modifier  = {";
	output << "\n\t\t\t\t" << "name = \"religious_intolerance\"";
	output << "\n\t\t\t\t" << "duration = 3650";
	output << "\n\t\t\t" << "}";
	output << "\n\t\t" << "}";
	output << "\n\t\t" << "if = {";
	output << "\n\t\t\t" << "limit = {";
	output << "\n\t\t\t\t" << "religion_group = " << religionGroup;
	output << "\n\t\t\t\t" << "NOT = { religion = " << name << " }";
	output << "\n\t\t\t" << "}";
	output << "\n\t\t\t" << "change_religion = " << name;
	output << "\n\t\t\t" << "force_converted = yes";
	output << "\n\t\t" << "}";
	output << "\n\t" << "}";
	output << "\n" << "}";
}