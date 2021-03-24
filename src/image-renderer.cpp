#include "hdr-image.h"
#include "color.h"
#undef NDEBUG
#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

int main()
{
    // buffer
    stringstream sstr;
    sstr <<"PF\n"; //try "PF" or "Ps"
    string streamString{sstr.str()}; // to string

    // exa version
    unsigned char ref[] = {
        0x50, 0x46, 0x0a
    };
    string refString(reinterpret_cast<char*>(ref), 3); // to string

    // let's compare them
    if(streamString.compare(refString) == 0) {
        cout << "si\n";
    }
    else {
        cout << "Sono diverse!\n"
        <<"stream: " <<streamString <<"len " <<streamString.length() <<"\n"
        <<"reference: " <<refString <<"len " <<refString.length() <<"\n"
        <<"compare: " <<streamString.compare(refString) <<"\n";
    }


    HdrImage img2{3, 2};

    img2.setPixel(0, 0, Color{1.0e1, 2.0e1, 3.0e1});
    img2.setPixel(1, 0, Color{4.0e1, 5.0e1, 6.0e1});
    img2.setPixel(2, 0, Color{7.0e1, 8.0e1, 9.0e1});
    img2.setPixel(0, 1, Color{1.0e2, 2.0e2, 3.0e2});
    img2.setPixel(1, 1, Color{4.0e2, 5.0e2, 6.0e2});
    img2.setPixel(2, 1, Color{7.0e2, 8.0e2, 9.0e2});

    unsigned char referencePfm[] = {
        0x50, 0x46, 0x0a, 0x33, 0x20, 0x32, 0x0a, 0x2d, 0x31, 0x2e, 0x30, 0x0a,
        0x00, 0x00, 0xc8, 0x42, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x96, 0x43,
        0x00, 0x00, 0xc8, 0x43, 0x00, 0x00, 0xfa, 0x43, 0x00, 0x00, 0x16, 0x44,
        0x00, 0x00, 0x2f, 0x44, 0x00, 0x00, 0x48, 0x44, 0x00, 0x00, 0x61, 0x44,
        0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0xa0, 0x41, 0x00, 0x00, 0xf0, 0x41,
        0x00, 0x00, 0x20, 0x42, 0x00, 0x00, 0x48, 0x42, 0x00, 0x00, 0x70, 0x42,
        0x00, 0x00, 0x8c, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xb4, 0x42
    };
    unsigned int referenceLenght = 84;
    string stringPfm{reinterpret_cast<char*>(referencePfm), referenceLenght};

    stringstream buf;
    img2.savePfm(buf);
    string bufString{buf.str()};
    if (bufString.compare(stringPfm) == 0) {
        cout <<"Sono uguali" <<endl;
    }
    else {
        cout <<"Sono diverse!\n"
            <<"buffer: " <<bufString <<"\n"
           <<"reference: " <<stringPfm <<endl;
    }

    const float a = 1.0f;
    cout <<fixed <<setprecision(1) <<a <<endl;

	return 0;
}
