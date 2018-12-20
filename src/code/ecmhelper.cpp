//
// Created by screemer on 2018-12-16.
//

#include "ecmhelper.h"


extern "C"
{
void eccedc_init(void);
int unecmify(FILE *in, FILE *out);
}


bool Ecmhelper::unecm(string input, string output) {
    cout << "Unpacking: " << input << " to " << output << endl;
    if (!Util::matchExtension(output, EXT_BIN)) {
        output = output + ".bin";
    }
    eccedc_init();
    FILE *fin = fopen(input.c_str(), "rb");
    if (!fin) {
        return false;
    }
    FILE *fout = fopen(output.c_str(), "wb");
    if (!fout) {
        fclose(fin);
        return false;
    }
    int result = unecmify(fin, fout);
    fclose(fout);
    fclose(fin);
    return result == 0;
}