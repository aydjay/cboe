/*
 *  scenario.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "classes.h"
#include "oldstructs.h"

cScenario::~cScenario() {
	destroy_terrain();
}

void cScenario::destroy_terrain() {
	// Nuke towns
	if(!towns.empty()) {
		for(size_t i = 0; i < towns.size(); i++) {
			if(towns[i] != nullptr) delete towns[i];
			towns[i] = nullptr;
		}
	}
	if(!outdoors.empty()){
		for(size_t i = 0; i < outdoors.width(); i++) {
			for(size_t j = 0; j < outdoors.height(); j++) {
				if(outdoors[i][j] != nullptr) delete outdoors[i][j];
				outdoors[i][j] = nullptr;
			}
		}
	}
}

cScenario& cScenario::operator=(cScenario&& other) {
	// If self-assignment, do nothing.
	if(this == &other) return *this;
	// First, free any held pointers.
	destroy_terrain();
	// Resize the outdoors to ensure the assigned outdoors fits
	outdoors.resize(other.outdoors.width(), other.outdoors.height());
	// Then forward to the default assignment operator.
	// Use const_cast to ensure the right overload is selected.
	return *this = const_cast<const cScenario&>(other);
}

cScenario::cScenario(bool init_strings) {
	short i;
	std::string temp_str;
	
	format.ver[0] = 1;
	format.min_run_ver = 1;
	format.prog_make_ver[0] = 1;
	num_towns = 1;
	out_width = 1;
	out_height = 1;
	default_ground = 1;
	for(i = 0; i < 200; i++) {
		town_size[i] = 1;
	}
	where_start.x = 24;
	where_start.y = 24;
	out_start = where_start;
	for(i = 0; i < 10; i++) {
		town_to_add_to[i] = -1;
	}
	for(i = 0; i < 3; i++) {
		store_item_towns[i] = -1;
	}
	for(i = 0; i < 50; i++) {
		special_items[i].special = -1;
	}
	for(i = 0; i < 20; i++) {
		scenario_timer_specs[i] = -1;
	}
	for(i = 0; i < 400; i++) {
		scen_items[i] = cItemRec();
	}
	if(!init_strings) return;
	for(i = 0; i < 270; i++) {
		temp_str = get_str("scen-default",i + 1);
		if(i == 0) scen_name = temp_str;
		else if(i == 1 || i == 2)
			who_wrote[i-1] = temp_str;
		else if(i == 3)
			contact_info = temp_str;
		else if(i >= 4 && i < 10)
			intro_strs[i-4] = temp_str;
		else if(i >= 10 && i < 60)
			journal_strs[i-10] = temp_str;
		else if(i >= 60 && i < 160) {
			if(i % 2 == 0) special_items[(i-60)/2].name = temp_str;
			else special_items[(i-60)/2].descr = temp_str;
		} else if(i >= 260) continue; // These were never ever used, for some reason.
		else spec_strs[i-160] = temp_str;
		scen_str_len[i] = temp_str.length();
	}
}

cScenario::cItemStorage::cItemStorage() : ter_type(-1), property(0) {
	for(int i = 0; i < 10; i++)
		item_num[i] = -1;
	for(int i = 0; i < 10; i++)
		item_odds[i] = 0;
}

//{-1,{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,0,0,0,0,0,0,0,0,0},0}

void cScenario::append(legacy::scenario_data_type& old){
	is_legacy = true;
	int i,j;
//	for(i = 0; i < 3; i++) format.ver[i] = old.ver[i];
//	format.min_run_ver = old.min_run_ver;
//	for(i = 0; i < 3; i++) format.prog_make_ver[i] = old.prog_make_ver[i];
	num_towns = format.num_towns;
	out_width = old.out_width;
	out_height = old.out_height;
	difficulty = old.difficulty;
	intro_pic = old.intro_pic;
	default_ground = old.default_ground;
	// TODO: One or both of these should probably be moved to cTown
	for(i = 0; i < 200; i++) town_size[i] = old.town_size[i];
	for(i = 0; i < 200; i++) town_hidden[i] = old.town_hidden[i];
	flag_a = old.flag_a;
	intro_mess_pic = old.intro_mess_pic;
	intro_mess_len = old.intro_mess_len;
	where_start.x = old.where_start.x;
	where_start.y = old.where_start.y;
	out_sec_start.x = old.out_sec_start.x;
	out_sec_start.y = old.out_sec_start.y;
	out_start.x = old.out_start.x;
	out_start.y = old.out_start.y;
	which_town_start = old.which_town_start;
	flag_b = old.flag_b;
	// TODO: Town data size doesn't even need to be stored at all; its only purpose is for locating the offset of a town within the old scenario format; same with out data size
	for(i = 0; i < 200; i++)
		for(j = 0; j < 5; j++)
			town_data_size[i][j] = old.town_data_size[i][j];
	for(i = 0; i < 10; i++) town_to_add_to[i] = old.town_to_add_to[i];
	for(i = 0; i < 10; i++)
		for(j = 0; j < 2; j++)
			flag_to_add_to_town[i][j] = old.flag_to_add_to_town[i][j];
	flag_c = old.flag_c;
	for(i = 0; i < 100; i++){
		out_data_size[i][0] = old.out_data_size[i][0];
		out_data_size[i][1] = old.out_data_size[i][1];
	}
	// TODO: Combine store_item_rects and store_item_towns into a structure
	for(i = 0; i < 3; i++) {
		store_item_rects[i].top = old.store_item_rects[i].top;
		store_item_rects[i].left = old.store_item_rects[i].left;
		store_item_rects[i].bottom = old.store_item_rects[i].bottom;
		store_item_rects[i].right = old.store_item_rects[i].right;
	}
	for(i = 0; i < 3; i++) store_item_towns[i] = old.store_item_towns[i];
	flag_e = old.flag_e;
	for(i = 0; i < 50; i++) special_items[i].flags = old.special_items[i];
	for(i = 0; i < 50; i++) special_items[i].special = old.special_item_special[i];
	rating = old.rating;
	// TODO: Is this used anywhere?
	uses_custom_graphics = old.uses_custom_graphics;
	flag_f = old.flag_f;
	for(i = 0; i < 256; i++) scen_monsters[i].append(old.scen_monsters[i]);
	for(i = 0; i < 30; i++) boats[i].append(old.scen_boats[i]);
	for(i = 0; i < 30; i++) horses[i].append(old.scen_horses[i]);
	flag_g = old.flag_g;
	for(i = 0; i < 256; i++){
		ter_types[i].i = i;
		ter_types[i].append(old.ter_types[i]);
	}
	for(i = 0; i < 20; i++) scenario_timer_times[i] = old.scenario_timer_times[i];
	for(i = 0; i < 20; i++) scenario_timer_specs[i] = old.scenario_timer_specs[i];
	flag_h = old.flag_h;
	for(i = 0; i < 256; i++) scen_specials[i].append(old.scen_specials[i]);
	for(i = 0; i < 10; i++) storage_shortcuts[i] = old.storage_shortcuts[i];
	flag_d = old.flag_d;
	for(i = 0; i < 300; i++) scen_str_len[i] = old.scen_str_len[i];
	flag_i = old.flag_i;
	last_out_edited.x = old.last_out_edited.x;
	last_out_edited.y = old.last_out_edited.y;
	last_town_edited = old.last_town_edited;
	adjust_diff = true;
}

cScenario::cItemStorage& cScenario::cItemStorage::operator = (legacy::item_storage_shortcut_type& old){
	ter_type = old.ter_type;
	for(int i = 0; i < 10; i++) item_num[i] = old.item_num[i];
	for(int i = 0; i < 10; i++) item_odds[i] = old.item_odds[i];
	property = old.property;
	return *this;
}

void cScenario::append(legacy::scen_item_data_type& old){
	short i;
	for(i = 0; i < 400; i++)
		scen_items[i].append(old.scen_items[i]);
	for(i = 0; i < 256; i++)
		scen_monsters[i].m_name = old.monst_names[i];
	for(i = 0; i < 256; i++)
		ter_types[i].name = old.ter_names[i];
}
