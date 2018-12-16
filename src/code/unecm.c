/***************************************************************************/
/*
** UNECM - Decoder for ECM (Error Code Modeler) format.
** Version 1.0
** Copyright (C) 2002 Neill Corlett
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/***************************************************************************/
/*
** Portability notes:
**
** - Assumes a 32-bit or higher integer size
** - No assumptions about byte order
** - No assumptions about struct packing
** - No unaligned memory access
*/
/***************************************************************************/

#include <stdio.h>
#include <string.h>


/***************************************************************************/

void banner(void) {
    fprintf(stderr,
            "UNECM - Decoder for Error Code Modeler format v1.0\n"
            "Copyright (C) 2002 Neill Corlett\n\n"
    );
}

/***************************************************************************/

/* Data types */
#define ecc_uint8 unsigned char
#define ecc_uint16 unsigned short
#define ecc_uint32 unsigned

/* LUTs used for computing ECC/EDC */
static ecc_uint8 ecc_f_lut[256];
static ecc_uint8 ecc_b_lut[256];
static ecc_uint32 edc_lut[256];

/* Init routine */
void eccedc_init(void) {
    ecc_uint32 i, j, edc;
    for (i = 0; i < 256; i++) {
        j = (i << 1) ^ (i & 0x80 ? 0x11D : 0);
        ecc_f_lut[i] = j;
        ecc_b_lut[i ^ j] = i;
        edc = i;
        for (j = 0; j < 8; j++) edc = (edc >> 1) ^ (edc & 1 ? 0xD8018001 : 0);
        edc_lut[i] = edc;
    }
}

/***************************************************************************/
/*
** Compute EDC for a block
*/
ecc_uint32 edc_partial_computeblock(
        ecc_uint32 edc,
        const ecc_uint8 *src,
        ecc_uint16 size
) {
    while (size--) edc = (edc >> 8) ^ edc_lut[(edc ^ (*src++)) & 0xFF];
    return edc;
}

void edc_computeblock(
        const ecc_uint8 *src,
        ecc_uint16 size,
        ecc_uint8 *dest
) {
    ecc_uint32 edc = edc_partial_computeblock(0, src, size);
    dest[0] = (edc >> 0) & 0xFF;
    dest[1] = (edc >> 8) & 0xFF;
    dest[2] = (edc >> 16) & 0xFF;
    dest[3] = (edc >> 24) & 0xFF;
}

/***************************************************************************/
/*
** Compute ECC for a block (can do either P or Q)
*/
static void ecc_computeblock(
        ecc_uint8 *src,
        ecc_uint32 major_count,
        ecc_uint32 minor_count,
        ecc_uint32 major_mult,
        ecc_uint32 minor_inc,
        ecc_uint8 *dest
) {
    ecc_uint32 size = major_count * minor_count;
    ecc_uint32 major, minor;
    for (major = 0; major < major_count; major++) {
        ecc_uint32 index = (major >> 1) * major_mult + (major & 1);
        ecc_uint8 ecc_a = 0;
        ecc_uint8 ecc_b = 0;
        for (minor = 0; minor < minor_count; minor++) {
            ecc_uint8 temp = src[index];
            index += minor_inc;
            if (index >= size) index -= size;
            ecc_a ^= temp;
            ecc_b ^= temp;
            ecc_a = ecc_f_lut[ecc_a];
        }
        ecc_a = ecc_b_lut[ecc_f_lut[ecc_a] ^ ecc_b];
        dest[major] = ecc_a;
        dest[major + major_count] = ecc_a ^ ecc_b;
    }
}

/*
** Generate ECC P and Q codes for a block
*/
static void ecc_generate(
        ecc_uint8 *sector,
        int zeroaddress
) {
    ecc_uint8 address[4], i;
    /* Save the address and zero it out */
    if (zeroaddress)
        for (i = 0; i < 4; i++) {
            address[i] = sector[12 + i];
            sector[12 + i] = 0;
        }
    /* Compute ECC P code */
    ecc_computeblock(sector + 0xC, 86, 24, 2, 86, sector + 0x81C);
    /* Compute ECC Q code */
    ecc_computeblock(sector + 0xC, 52, 43, 86, 88, sector + 0x8C8);
    /* Restore the address */
    if (zeroaddress) for (i = 0; i < 4; i++) sector[12 + i] = address[i];
}

/***************************************************************************/
/*
** Generate ECC/EDC information for a sector (must be 2352 = 0x930 bytes)
** Returns 0 on success
*/
void eccedc_generate(ecc_uint8 *sector, int type) {
    ecc_uint32 i;
    switch (type) {
        case 1: /* Mode 1 */
            /* Compute EDC */
            edc_computeblock(sector + 0x00, 0x810, sector + 0x810);
            /* Write out zero bytes */
            for (i = 0; i < 8; i++) sector[0x814 + i] = 0;
            /* Generate ECC P/Q codes */
            ecc_generate(sector, 0);
            break;
        case 2: /* Mode 2 form 1 */
            /* Compute EDC */
            edc_computeblock(sector + 0x10, 0x808, sector + 0x818);
            /* Generate ECC P/Q codes */
            ecc_generate(sector, 1);
            break;
        case 3: /* Mode 2 form 2 */
            /* Compute EDC */
            edc_computeblock(sector + 0x10, 0x91C, sector + 0x92C);
            break;
    }
}

/***************************************************************************/

unsigned mycounter;
unsigned mycounter_total;

void resetcounter(unsigned total) {
    mycounter = 0;
    mycounter_total = total;
}

void setcounter(unsigned n) {
    if ((n >> 20) != (mycounter >> 20)) {
        unsigned a = (n + 64) / 128;
        unsigned d = (mycounter_total + 64) / 128;
        if (!d) d = 1;
        fprintf(stderr, "Decoding (%02d%%)\r", (100 * a) / d);
    }
    mycounter = n;
}

int unecmify(
        FILE *in,
        FILE *out
) {
    unsigned checkedc = 0;
    unsigned char sector[2352];
    unsigned type;
    unsigned num;
    fseek(in, 0, SEEK_END);
    resetcounter(ftell(in));
    fseek(in, 0, SEEK_SET);
    if (
            (fgetc(in) != 'E') ||
            (fgetc(in) != 'C') ||
            (fgetc(in) != 'M') ||
            (fgetc(in) != 0x00)
            ) {
        fprintf(stderr, "Header not found!\n");
        goto corrupt;
    }
    for (;;) {
        int c = fgetc(in);
        int bits = 5;
        if (c == EOF) goto uneof;
        type = c & 3;
        num = (c >> 2) & 0x1F;
        while (c & 0x80) {
            c = fgetc(in);
            if (c == EOF) goto uneof;
            num |= ((unsigned) (c & 0x7F)) << bits;
            bits += 7;
        }
        if (num == 0xFFFFFFFF) break;
        num++;
        if (num >= 0x80000000) goto corrupt;
        if (!type) {
            while (num) {
                int b = num;
                if (b > 2352) b = 2352;
                if (fread(sector, 1, b, in) != b) goto uneof;
                checkedc = edc_partial_computeblock(checkedc, sector, b);
                fwrite(sector, 1, b, out);
                num -= b;
                setcounter(ftell(in));
            }
        } else {
            while (num--) {
                memset(sector, 0, sizeof(sector));
                memset(sector + 1, 0xFF, 10);
                switch (type) {
                    case 1:
                        sector[0x0F] = 0x01;
                        if (fread(sector + 0x00C, 1, 0x003, in) != 0x003) goto uneof;
                        if (fread(sector + 0x010, 1, 0x800, in) != 0x800) goto uneof;
                        eccedc_generate(sector, 1);
                        checkedc = edc_partial_computeblock(checkedc, sector, 2352);
                        fwrite(sector, 2352, 1, out);
                        setcounter(ftell(in));
                        break;
                    case 2:
                        sector[0x0F] = 0x02;
                        if (fread(sector + 0x014, 1, 0x804, in) != 0x804) goto uneof;
                        sector[0x10] = sector[0x14];
                        sector[0x11] = sector[0x15];
                        sector[0x12] = sector[0x16];
                        sector[0x13] = sector[0x17];
                        eccedc_generate(sector, 2);
                        checkedc = edc_partial_computeblock(checkedc, sector + 0x10, 2336);
                        fwrite(sector + 0x10, 2336, 1, out);
                        setcounter(ftell(in));
                        break;
                    case 3:
                        sector[0x0F] = 0x02;
                        if (fread(sector + 0x014, 1, 0x918, in) != 0x918) goto uneof;
                        sector[0x10] = sector[0x14];
                        sector[0x11] = sector[0x15];
                        sector[0x12] = sector[0x16];
                        sector[0x13] = sector[0x17];
                        eccedc_generate(sector, 3);
                        checkedc = edc_partial_computeblock(checkedc, sector + 0x10, 2336);
                        fwrite(sector + 0x10, 2336, 1, out);
                        setcounter(ftell(in));
                        break;
                }
            }
        }
    }
    if (fread(sector, 1, 4, in) != 4) goto uneof;
    fprintf(stderr, "Decoded %ld bytes -> %ld bytes\n", ftell(in), ftell(out));
    if (
            (sector[0] != ((checkedc >> 0) & 0xFF)) ||
            (sector[1] != ((checkedc >> 8) & 0xFF)) ||
            (sector[2] != ((checkedc >> 16) & 0xFF)) ||
            (sector[3] != ((checkedc >> 24) & 0xFF))
            ) {
        fprintf(stderr, "EDC error (%08X, should be %02X%02X%02X%02X)\n",
                checkedc,
                sector[3],
                sector[2],
                sector[1],
                sector[0]
        );
        goto corrupt;
    }
    fprintf(stderr, "Done; file is OK\n");
    return 0;
    uneof:
    fprintf(stderr, "Unexpected EOF!\n");
    corrupt:
    fprintf(stderr, "Corrupt ECM file!\n");
    return 1;
}

/***************************************************************************/
/*
int main(int argc, char **argv) {
    FILE *fin, *fout;
    char *infilename;
    char *outfilename;
    banner();

    eccedc_init();

    if((argc != 2) && (argc != 3)) {
        fprintf(stderr, "usage: %s ecmfile [outputfile]\n", argv[0]);
        return 1;
    }

    infilename = argv[1];
    if(strlen(infilename) < 5) {
        fprintf(stderr, "filename '%s' is too short\n", infilename);
        return 1;
    }
    if(strcasecmp(infilename + strlen(infilename) - 4, ".ecm")) {
        fprintf(stderr, "filename must end in .ecm\n");
        return 1;
    }

    if(argc == 3) {
        outfilename = argv[2];
    } else {
        outfilename = malloc(strlen(infilename) - 3);
        if(!outfilename) abort();
        memcpy(outfilename, infilename, strlen(infilename) - 4);
        outfilename[strlen(infilename) - 4] = 0;
    }
    fprintf(stderr, "Decoding %s to %s.\n", infilename, outfilename);

    fin = fopen(infilename, "rb");
    if(!fin) {
        perror(infilename);
        return 1;
    }
    fout = fopen(outfilename, "wb");
    if(!fout) {
        perror(outfilename);
        fclose(fin);
        return 1;
    }

    unecmify(fin, fout);

    fclose(fout);
    fclose(fin);
    return 0;
}
*/