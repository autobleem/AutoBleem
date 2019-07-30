/*
 *
 *   Code based on https://github.com/raphnet/psxmemcardmgr
 *   Raphael Assenat   Licenced as: GPLv3
 *
 *
 */

#include <string>
#include <iconv.h>
#include <SDL2/SDL_render.h>
#include "../gui/gui_sdl_wrapper.h"

#pragma once

using namespace std;

class RGB
{
public:
    RGB():RGB(0,0,0,0){};
    RGB(unsigned char r, unsigned char g, unsigned char b) : RGB(r,g,b,0xFF){};
    RGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    unsigned char r,g,b,a;


};
class CardEdit
{
public:
    CardEdit(SDL_Shared<SDL_Renderer> renderer1);
	~CardEdit();

	// action commands
	int load_file(string filename);
    int save_file(string filename);
	int clearData();
	void update_data();

	int delete_slot(int slot);
	int undelete_slot(int slot);

	// informaton commands
    string get_slot_Pcode(int slot);
    string get_slot_title(int slot);
    string get_slot_gameID(int slot);
    SDL_Shared<SDL_Texture> get_slot_icon(int slot);

	// information change command
	void set_slot_gameID(int slot, string newID);
	void set_slot_title(int slot, string newTitle);
	void set_slot_Pcode(int slot, string newPcode);

	bool get_slot_is_used(int slot);
	bool get_slot_is_free(int slot);

	void getSlotData(int slot, unsigned char* output);
	void setSlotData(int slot, unsigned char* input);


private:
    SDL_Shared<SDL_Renderer> renderer;
	char memoryCard[131072];   //a memory card can hold 128K
	bool slot_is_used[15];
	bool slot_is_deleted[15];  // deleted, but SC still there
	bool slot_has_icon[15];
	unsigned char block_type[15]; // 0 not used, 1 top block, 2 link, 3 link end block
    string slot_Pcodes[15];
    string slot_gameID[15];
    string slot_titles[15];
    SDL_Shared<SDL_Texture>  slot_icons[15];
	void update();
	void update_slot_is_used();  // also updates block_type
	void update_slot_is_deleted();
	void update_slot_has_icon();
	void update_slot_Pcodes();
	void update_slot_gameIDs();
	void update_slot_titles();
	void update_slot_iconImages();
	iconv_t sjisToUtf8;
};

