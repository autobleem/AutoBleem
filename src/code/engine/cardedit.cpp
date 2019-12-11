#include <cstring>
#include <autobleem/autobleemui.h>
#include "cardedit.h"
#include <string>
#include <fstream>
#include <iostream>


using namespace std;

#define PSX_DIRECTORY_FREE 0xA0
#define PSX_DIRECTORY_BUSY 0x50
#define PSX_DIRECTORY_RESERVED 0xF0
#define PSX_BLOCK_NOT_USED 0x00
#define PSX_BLOCK_TOP 0x01
#define PSX_BLOCK_LINK 0x02
#define PSX_BLOCK_LINK_END 0x03

CardEdit::CardEdit(SDL_Shared<SDL_Renderer> renderer1) {
    renderer = renderer1;
    // Initialise the card contents
    memset(memoryCard, 0, sizeof(memoryCard));

    convTable = new uint8_t[25088];
    ifstream is(Env::getWorkingPath() + sep + "shiftjis.dat");
    is.read(reinterpret_cast<char *>(convTable), 25088);
    is.close();

    // Create image buffers for icons
    for (int i = 0; i < 15; i++) {

        int pitch;
        void *pixels;
        for (int icon=0;icon<3;icon++) {
            slot_icons[i][icon] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 16, 16);
            SDL_LockTexture(slot_icons[i][icon], NULL, &pixels, &pitch);
            //Copy loaded/formatted surface pixels
            std::memset(pixels, 0, 16 * 16 * 4);
            SDL_UnlockTexture(slot_icons[i][icon]);
        }
        slot_titles[i]="";
        slot_Pcodes[i]="";
        slot_gameID[i]="";

    }

    // New empty card
    load_file(Env::getWorkingPath() + sep + "memcard" + sep + "card1.mcd");
}

CardEdit::~CardEdit() {
    delete convTable;
}

vector<int> CardEdit::getGameSlots(int startslot)
{
    vector<int> slots;
    int currentSlot = startslot;
    slots.push_back(currentSlot);
    int nextSlot;
    int iteration=0;
    while ((nextSlot=next_slot_map[currentSlot])  != 0xFF)
    {
        slots.push_back(nextSlot);
        currentSlot = nextSlot;
        if (iteration==15)
        {
            break;
        }
    }

    return slots;
}
vector<int> CardEdit::findEmptySlot(int requested)
{
    vector<int> slots;
    for (int i=0;i<15;i++)
    {
        if (get_slot_is_free(i))
        {
            slots.push_back(i);
        }
        if (slots.size()==requested)
        {
            break;
        }
    }


    return slots;
}

void CardEdit::getSlotData(int slot, unsigned char* buffer, unsigned char *direntry)
{
    // slot position
    int dir_position = 0x80 + (slot * 0x80);
    memcpy(direntry, memoryCard+dir_position, 0x80);
    int slot_position = 0x2000 + (slot * 0x2000);
    memcpy(buffer,memoryCard+slot_position,0x2000);
}

void CardEdit::setSlotData(int slot, unsigned char* buffer, unsigned char *direntry)
{
    // slot position
    int dir_position = 0x80 + (slot * 0x80);
    memcpy(memoryCard+dir_position, direntry, 0x80);
    int slot_position = 0x2000 + (slot * 0x2000);
    memcpy(memoryCard+slot_position, buffer, 0x2000);
    update();
}

int CardEdit::load_file(std::string filename) {
    ifstream f(filename, ifstream::ate | ifstream::binary);
    // index set to end
    if (f.tellg() < 131072) {
        f.close();
        return 0;
    } // the file is too small...
    if (f.tellg() == 134976) {
        // This must be a DexDrive file.
        f.seekg(3904);  // skip dexdrive header, and go to the memory card header it's self.

    } else {
        f.seekg(0);
    }
    f.read(memoryCard, 131072);
    f.close();
    // Now, lets fill up data
    update();
    return 0;

}



int CardEdit::save_file(string filename) {
    ofstream f(filename);
    f.write(memoryCard, 131072);
    f.close();
    return 0;

}

void CardEdit::update_data() {
    update();
}

void CardEdit::delete_game(int startslot)
{
    vector<int> slots=getGameSlots(startslot);
    for (int i:slots)
    {
        delete_slot(i);
    }
}
int CardEdit::delete_slot(int slot) {
    int position;
    position = 0x80 + (slot * 0x80);
    // delete the save
    memoryCard[position] = (memoryCard[position] | 0xF0) ^ 0xF0 ^ PSX_DIRECTORY_FREE;

    // xor code
    unsigned char xor_code = 0x00;
    position = 0x80 + (slot * 0x80);  // get to the start of the frame
    for (int j = 0; j < 126; j++) {
        xor_code = xor_code ^ memoryCard[j + position];
    }
    memoryCard[position + 127] = xor_code;
    //memoryCard[position + 127] = 0xFF;

    update();

    return 0;
}

int CardEdit::undelete_slot(int slot) {
    int position;
    position = 0x80 + (slot * 0x80);
    // delete the save
    memoryCard[position] = (memoryCard[position] | 0xF0) ^ 0xF0 ^ PSX_DIRECTORY_BUSY;

    // xor code
    unsigned char xor_code = 0x00;
    position = 0x80 + (slot * 0x80);  // get to the start of the frame
    for (int j = 0; j < 126; j++) {
        xor_code = xor_code ^ memoryCard[j + position];
    }
    memoryCard[position + 127] = xor_code;
    //memoryCard[position + 127] = 0xFF;

    update();

    return 0;
}

int CardEdit::clearData() {
    for (int i = 0; i < 131072; i++) {
        memoryCard[i] = 0;
    }
    load_file(Env::getWorkingPath() + sep + "memcard" + sep + "card1.mcd");
    update();
    return 0;
}

void CardEdit::update() {
    // order is important here.
    update_slot_is_used();    // Verify if it is used or not
    update_slot_is_deleted(); // is it a deleted slot(which can be undeleted)?
    update_slot_has_icon();   // Verify if slot has icon
    update_slot_Pcodes();     // Read the product code
    update_slot_titles();     // Read the title
    update_slot_iconImages(); // Retreive icons
    update_slot_gameIDs();    // Read Game Id's
}

void CardEdit::update_slot_is_used() {
    int current_pos = 128;
    for (int i = 0; i < 15; i++) {
        if ((memoryCard[current_pos] & PSX_DIRECTORY_BUSY) == PSX_DIRECTORY_BUSY) {
            slot_is_used[i] = true;

            // Code to update block_type
            if ((memoryCard[current_pos] & PSX_BLOCK_NOT_USED) == PSX_BLOCK_NOT_USED) {
                block_type[i] = PSX_BLOCK_NOT_USED;
            }
            if ((memoryCard[current_pos] & PSX_BLOCK_TOP) == PSX_BLOCK_TOP) {
                block_type[i] = PSX_BLOCK_TOP;
            }
            if ((memoryCard[current_pos] & PSX_BLOCK_LINK_END) == PSX_BLOCK_LINK) {
                block_type[i] = PSX_BLOCK_LINK;
            }
            if ((memoryCard[current_pos] & PSX_BLOCK_LINK_END) == PSX_BLOCK_LINK_END) {
                block_type[i] = PSX_BLOCK_LINK_END;
            }
        } else {
            //qDebug( "Slot %d is unused", i);
            slot_is_used[i] = false;
            block_type[i] = PSX_BLOCK_NOT_USED;
        }
        unsigned char nextSlot = memoryCard[current_pos+8];
        next_slot_map[i] = nextSlot;

        current_pos += 128;
    }
}

void CardEdit::update_slot_is_deleted() {
    int position = 128;
    for (int i = 0; i < 15; i++) {
        // If the directory is free but the block still starts with SC, then the block may be undeleted
        if ((memoryCard[position] & PSX_DIRECTORY_FREE) == PSX_DIRECTORY_FREE) {
            if ((memoryCard[0x2000 + (i * 0x2000)] == 'S') && (memoryCard[0x2000 + (i * 0x2000) + 1] == 'C')) {
                slot_is_deleted[i] = true;

            } else {
                slot_is_deleted[i] = false;

            }
        } else {
            slot_is_deleted[i] = false;

        }
        position += 128;
    }
}

void CardEdit::update_slot_has_icon() {
    for (int i = 0; i < 15; i++) {
        if ((block_type[i] == PSX_BLOCK_TOP) || slot_is_deleted[i]) {
            slot_has_icon[i] = true;
        } else {
            slot_has_icon[i] = false;
        }
    }
}


void CardEdit::update_slot_Pcodes() {
    int current_pos = 128;
    int pcode_pos = 0;
    int char_count = 0;
    for (int i = 0; i < 15; i++) {
        if (slot_is_used[i] ) {
            pcode_pos = current_pos + 12; // The product code is the 12th byte
            char_count = 0;
            slot_Pcodes[i] = "";
            while ((memoryCard[pcode_pos] != 0) & (char_count < 10)) {
                slot_Pcodes[i] += memoryCard[pcode_pos];
                pcode_pos++;
                char_count++;
            }
            //qDebug("Slot %d pcode is: %s",i, slot_Pcodes[i].latin1());
        } else {
            slot_Pcodes[i] = "";
        }
        current_pos += 128;
    }
}

void CardEdit::update_slot_gameIDs() {
    int current_pos = 128;
    int pcode_pos = 0;
    int char_count = 0;
    for (int i = 0; i < 15; i++) {
        if (slot_is_used[i] ) {
            pcode_pos = current_pos + 22; // The game ID is the 22th byte
            char_count = 0;
            slot_gameID[i] = "";
            while ((memoryCard[pcode_pos] != 0)) {
                slot_gameID[i] += memoryCard[pcode_pos];
                pcode_pos++;
                char_count++;
            }
            //qDebug("Slot %d pcode is: %s",i, slot_gameID[i].latin1());
        } else {
            slot_gameID[i] = "";
        }
        current_pos += 128;
    }
}

bool CardEdit::is_slot_top(int slot)
{
    return block_type[slot] == PSX_BLOCK_TOP;
}

void CardEdit::update_slot_titles() {
    shared_ptr<Lang> lang(Lang::getInstance());
    int current_pos = 0x2000;  // The second block starts here

    for (int i = 0; i < 15; i++) {
        if (slot_is_used[i] ) {
            if ((block_type[i] == PSX_BLOCK_TOP) || slot_is_deleted[i]) {
                string tmpbuf;
                char *jis_title;
                size_t tlen;

                slot_titles[i] = "";

                //  04h-43h  Title in Shift-JIS format (64 bytes = max 32 characters)
                jis_title = (char *) &memoryCard[current_pos + 4];
                tlen = strnlen(jis_title, 64);


                slot_titles[i] += sj2utf8(jis_title);

                if (slot_is_deleted[i])
                    slot_titles[i] = "(" + slot_titles[i] + ")";

            }   // endif block_type
            else {
                if (block_type[i] == PSX_BLOCK_LINK) { slot_titles[i] = _("Link Block"); }
                if (block_type[i] == PSX_BLOCK_LINK_END) { slot_titles[i] = _("Link end Block"); }
            }
        } else {
            slot_titles[i] = _("Free");
        }

        current_pos += 0x2000;
    }
}

std::string CardEdit::sj2utf8(const std::string &input)
{
    std::string output(3 * input.length(), ' '); //ShiftJis won't give 4byte UTF8, so max. 3 byte per input char are needed
    size_t indexInput = 0, indexOutput = 0;

    while(indexInput < input.length())
    {
        char arraySection = ((uint8_t)input[indexInput]) >> 4;

        size_t arrayOffset;
        if(arraySection == 0x8) arrayOffset = 0x100; //these are two-byte shiftjis
        else if(arraySection == 0x9) arrayOffset = 0x1100;
        else if(arraySection == 0xE) arrayOffset = 0x2100;
        else arrayOffset = 0; //this is one byte shiftjis

        //determining real array offset
        if(arrayOffset)
        {
            arrayOffset += (((uint8_t)input[indexInput]) & 0xf) << 8;
            indexInput++;
            if(indexInput >= input.length()) break;
        }
        arrayOffset += (uint8_t)input[indexInput++];
        arrayOffset <<= 1;

        //unicode number is...
        uint16_t unicodeValue = (convTable[arrayOffset] << 8) | convTable[arrayOffset + 1];

        //converting to UTF8
        if(unicodeValue < 0x80)
        {
            output[indexOutput++] = unicodeValue;
        }
        else if(unicodeValue < 0x800)
        {
            output[indexOutput++] = 0xC0 | (unicodeValue >> 6);
            output[indexOutput++] = 0x80 | (unicodeValue & 0x3f);
        }
        else
        {
            output[indexOutput++] = 0xE0 | (unicodeValue >> 12);
            output[indexOutput++] = 0x80 | ((unicodeValue & 0xfff) >> 6);
            output[indexOutput++] = 0x80 | (unicodeValue & 0x3f);
        }
    }

    output.resize(indexOutput); //remove the unnecessary bytes
    return output;
}

int CardEdit::getExportSize(int startslot)
{
    return 8320 + ((getGameSlots(startslot).size() - 1) * 8192);
}

void CardEdit::importGame(unsigned char* buffer, int length)
{
    int slotCount = (length - 128) / 8192;
    int numberOfBytes = slotCount * 8192;
    vector<int> destSlots = findEmptySlot(slotCount);

    if (destSlots.size()!=slotCount)
    {
        return;
    }
    // Place header data

    int dir_position = 0x80 + (destSlots[0] * 0x80);
    for (int i = 0; i < 128; i++)
        memoryCard[dir_position+i] = buffer[i];

    // update size in header
    memoryCard[dir_position+4] = (unsigned char)(numberOfBytes & 0xFF);
    memoryCard[dir_position+5] = (unsigned char)((numberOfBytes & 0xFF00) >> 8);
    memoryCard[dir_position+6] = (unsigned char)((numberOfBytes & 0xFF0000) >> 16);

    // store all slots

    int iteration=0;
    for (int slotNumber:destSlots)
    {
        int  slot_position = 0x2000 + (slotNumber * 0x2000);
        //Set all bytes
        for (int byteCount = 0; byteCount < 8192; byteCount++)
        {
            memoryCard[slot_position+byteCount] = buffer[128 + (iteration * 8192) + byteCount];
        }
        iteration++;
    }
    // Recreate headers
    // Set pointer to all slots except the last
    for (int i = 0; i < slotCount; i++)
    {
        int slot = destSlots[i];
        dir_position = 0x80 + (slot * 0x80);
        memoryCard[dir_position+0] = 0x52;
        memoryCard[dir_position+8] = (unsigned char) destSlots[i + 1];
        memoryCard[dir_position+9] = 0x00;
    }

    dir_position = 0x80 + (destSlots[destSlots.size()-1] * 0x80);
    //Add final slot pointer to the last slot in the link
    memoryCard[dir_position+0] = 0x53;
    memoryCard[dir_position+8] = 0xFF;
    memoryCard[dir_position+9] = 0xFF;

    dir_position = 0x80 + (destSlots[0] * 0x80);
    memoryCard[dir_position+0] = 0x51;

    for (int slotNumber:destSlots) {
        unsigned char xor_code = 0x00;
       int  position = 0x80 + (slotNumber * 0x80);  // get to the start of the frame
        for (int j = 0; j < 126; j++) {
            xor_code = xor_code ^ memoryCard[j + position];
        }
        memoryCard[position + 127] = xor_code;
    }
    update();
}

void CardEdit::exportGame(int slot, unsigned char *buffer)
{
    vector<int> slots = getGameSlots(slot);
    // copy save header
    for (int i=0;i<128;i++)
    {
        int dir_position = 0x80 + (slot * 0x80);
        buffer[i] = memoryCard[dir_position+i];
    }
    // copy data
    //Copy save data
    int slotUsed = 0;
    for (int slotNumber:slots)
    {
        int  slot_position = 0x2000 + ((slotNumber) * 0x2000);
        for (int i = 0; i < 0x2000; i++) {
            int address = 128 + (slotUsed * 0x2000) + i;
            buffer[address] = memoryCard[slot_position + i];

        }
        slotUsed++;
    }
}

void CardEdit::update_slot_iconImages() {

    int icn_pos = 0;
    RGB palette[16];
    for (int i = 0; i < 15; i++) {
        for (int icon=0;icon<3;icon++) {
            int offset = icon*128;
            int paladdr = 0x2060 + (i * 0x2000) ;
            int dataaddr = 0x2080 + (i * 0x2000) + offset;

            if (slot_has_icon[i]) {

                for (int p = 0; p < 16; p++) {
                    unsigned char pp;
                    unsigned char red, green, blue;


                    // Calculate blue component
                    pp = memoryCard[paladdr + (p * 2) + 1];
                    //blue = (pp|0xE0) ^ 0xE0;
                    blue = pp >> 2;

                    pp = memoryCard[paladdr + (p * 2)];
                    green = ((pp >> 5) | 0xF8) ^ 0xF8;
                    pp = memoryCard[paladdr + (p * 2) + 1];
                    green = green + (((pp | 0xFC) ^ 0xFC) << 3);

                    // Calculate blue component
                    pp = memoryCard[paladdr + (p * 2)];
                    //red = ((pp|0x83) ^ 0x83);
                    red = (pp | 0xE0) ^ 0xE0;

                    //if (i==0) { qDebug("Color %d: R:%d  G:%d  B:%d",p,red,green,blue); }
                    //if (i==0) { qDebug("Color %d: R:%d  G:%d  B:%d",p,red*8,green*8,blue*8); }
                    if (slot_is_deleted[i]) {
                        palette[p] = RGB((red * 4 + 127), (green * 4 + 127), (blue * 4 + 127));
                    } else {
                        palette[p] = RGB(red * 8, green * 8, blue * 8);
                    }

                }

                icn_pos = 0;
                void *pixels;
                int pitch;
                SDL_LockTexture(slot_icons[i][icon], NULL, &pixels, &pitch);
                Uint32 *pixelData = (Uint32 *) pixels;
                SDL_PixelFormat *fmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
                for (int y = 0; y < 16; y++) {
                    for (int x = 0; x < 16; x += 2) {
                        unsigned char index;

                        //Copy loaded/formatted surface pixels
                        index = (memoryCard[dataaddr + icn_pos] | 0xF0) ^ 0xF0;
                        Uint32 color1 = SDL_MapRGBA(fmt, palette[index].r, palette[index].g, palette[index].b, 255);
                        pixelData[x + y * 16] = color1;

                        //slot_icons[i]->setPixel(x, y, palette[index]);

                        index = (((memoryCard[dataaddr + icn_pos]) >> 4) | 0xF0) ^ 0xF0;
                        Uint32 color2 = SDL_MapRGBA(fmt, palette[index].r, palette[index].g, palette[index].b, 255);
                        //slot_icons[i]->setPixel(x+1, y, palette[index]);
                        pixelData[x + 1 + y * 16] = color2;
                        icn_pos += 1;
                    }
                }
                SDL_UnlockTexture(slot_icons[i][icon]);
                SDL_FreeFormat(fmt);
            } else {
                void *pixels;
                int pitch;
                SDL_LockTexture(slot_icons[i][icon], NULL, &pixels, &pitch);
                Uint32 *pixelData = (Uint32 *) pixels;
                SDL_PixelFormat *fmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
                Uint32 transparent = SDL_MapRGBA(fmt, 0, 0, 0, 127);
                for (int y = 0; y < 16; y++) {
                    for (int x = 0; x < 16; x++) {
                        pixelData[x + y * 16] = transparent;
                    }
                }
                SDL_UnlockTexture(slot_icons[i][icon]);
                SDL_FreeFormat(fmt);
            }
        }
    }

    // now update link blocks to have dimmed image
    for (int i=0;i<15;i++)
    {
        GfxImage  currentTex;
        if (is_slot_top(i))
        {
            currentTex = slot_icons[i][0];
            vector<int> allSlots = getGameSlots(i);
            for (int slot:allSlots)
            {
                if (slot==i)
                {
                    continue; // do not touch top slot
                } else
                {
                    SDL_PixelFormat *fmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
                    for (int icon=0;icon<3;icon++) {
                        void *pixels;
                        int pitch;
                        void *destpixels;
                        int destpitch;
                        SDL_LockTexture(slot_icons[slot][icon], NULL, &destpixels, &destpitch);
                        SDL_LockTexture(currentTex, NULL, &pixels, &pitch);
                        Uint32 *pixelData = (Uint32 *) pixels;
                        Uint32 *pixelDataDest = (Uint32 *) destpixels;

                        Uint8 r,g,b,a;
                        Uint32 transparent = SDL_MapRGBA(fmt, 0, 0, 0, 127);
                        for (int y = 0; y < 16; y++) {
                            for (int x = 0; x < 16; x++) {
                                SDL_GetRGBA(pixelData[x+y*16],fmt,&r,&g,&b,&a);
                                Uint32 processedColour = SDL_MapRGBA(fmt, r/3,g/3,b/3,255);
                                pixelDataDest[x + y * 16] = processedColour;
                            }
                        }
                        SDL_UnlockTexture(currentTex);
                        SDL_UnlockTexture(slot_icons[slot][icon]);

                    }
                    slot_has_icon[slot] = true;
                    SDL_FreeFormat(fmt);
                }
            }
        }
    }

}

string CardEdit::get_slot_Pcode(int slot) {
    return slot_Pcodes[slot];
}

string CardEdit::get_slot_title(int slot) {
    return slot_titles[slot];
}

string CardEdit::get_slot_gameID(int slot) {
    return slot_gameID[slot];
}

bool CardEdit::get_slot_is_used(int slot) {
    return slot_is_used[slot];
}

bool CardEdit::get_slot_is_free(int slot) {
    if (block_type[slot] == PSX_BLOCK_NOT_USED) {
        return true;
    }
    return false;
}

GfxImage  CardEdit::get_slot_icon(int slot, int frame) {
    return slot_icons[slot][frame];
}

void CardEdit::set_slot_gameID(int slot, string newID) {
    int position;
    int i = 0;
    int max_title_length = 102; // not counting 0
    const char *id_string = newID.c_str();


    position = 0x80 + (slot * 0x80);  // get to the start of the frame
    position += 0x0C;  // go to the product code + Game ID ASCIIZ string
    position += 10; // jump to the Game ID start


    // write the new game id
    do {
        memoryCard[position + i] = id_string[i];
        i++;
    } while ((id_string[i] != 0) && (i < max_title_length));

    // compute the new XOR code
    unsigned char xor_code = 0x00;
    position = 0x80 + (slot * 0x80);  // get to the start of the frame
    for (int j = 0; j < 126; j++) {
        xor_code = xor_code ^ memoryCard[j + position];
    }
    memoryCard[position + 127] = xor_code;
    //memoryCard[position + 127] = 0xFF;

    update();
}


void CardEdit::set_slot_Pcode(int slot, string newPcode) {
    int position;
    int i = 0;
    int max_title_length = 10; // not counting 0
    const char *id_string = newPcode.c_str();


    position = 0x80 + (slot * 0x80);  // get to the start of the frame
    position += 0x0C;  // go to the product code + Game ID ASCIIZ string



    // write the new game id

    while ((id_string[i] != 0) && (i < max_title_length)) {
        memoryCard[position + i] = id_string[i];
        i++;
    }

    // compute the new XOR code
    unsigned char xor_code = 0x00;
    position = 0x80 + (slot * 0x80);  // get to the start of the frame
    for (int j = 0; j < 126; j++) {
        xor_code = xor_code ^ memoryCard[j + position];
    }
    memoryCard[position + 127] = xor_code;
    //memoryCard[position + 127] = 0xFF;

    update();
}
