/*
 *  QBZ - Lövős
 * 
 *  Kísérleti projekt egy egyszerű konzolos játékra
 *
 *  Irányítás: A, D, SPACE, ESC
 *
*/

#include <iostream>
#include <conio.h>
#include <string>
#include <iomanip>
#include <stdlib.h>

// EGY IDŐEGYSÉG (a játékban, programciklusban kifejezve)
#define DEFAULT_SPEED    350000

// JÁTÉKTÁBLA MÉRETEI (JSZ: szélesség, JM: magasság)
// mindkét irány maximum 255 lehet!
#define JSZ              30
#define JM               15

// RAJZOLÓ KARAKTEREK
#define FUGGOLEGES       char(179)    // - Játéktér függőleges oldala
#define VIZSZINTES       char(196)    // ¦ Játéktér vízszintes oldala
#define BAL_FELSO_SAROK  char(218)    // - Játéktér bal felső sarka
#define JOBB_FELSO_SAROK char(191)    // ¬ Játéktér jobb felső sarka
#define BAL_ALSO_SAROK   char(192)    // L Játéktér bal alsó sarka
#define JOBB_ALSO_SAROK  char(217)    // - Játéktér jobb alsó sarka
#define UTOLSO_SOR_JEL   char(197)    // + Ez jelzi az utolsó sort a tábla szélén
#define AGYUCSO          char(186)    // ¦ Ágyúcső
#define AGYUTALP_BAL     char(201)    // - Ágyútalp bal széle
#define AGYUTALP_KOZEP   char(202)    // ¦ Ágyútalp közepe
#define AGYUTALP_JOBB    char(187)    // ¬ Ágyútalp jobb széle
#define TEGLA1           176          // - Vékony tégla
#define TEGLA2           177          // - Közepes tégla
#define TEGLA3           178          // - Vastag tégla

// a _tégákRarajzolása függvényhez
#define TABLA_SZELE      (i==tabla.utolsoSor?UTOLSO_SOR_JEL:FUGGOLEGES)

typedef unsigned long long  usll;
typedef unsigned char       byte;

struct Agyu
{
    byte agyucso;
    char parancs;

    Agyu()
    {
        agyucso = 14;
        parancs = 0;
    }

    bool mozgatas()
    {
        _jatekosIranyitasa();
        if (parancs==0) return false;
        if (parancs=='a' && agyucso>0) agyucso--;
        if (parancs=='d' && agyucso<JSZ-1) agyucso++;
        return true;
    }

    private:

    void _jatekosIranyitasa()
    {
        parancs=0;
        if (_kbhit()) parancs=_getch();
        if (parancs!='a' && parancs!='d' && parancs!=' ' && int(parancs)!=27) parancs=0;
    }
};

struct Tabla
{
    byte tegla[JM][JSZ];
    byte legfelsoSor;           // aktuálisan a tábla legtetején lévő sor
    byte legalsoSor;            // aktuálisan a tábla legalján lévő sor
    byte utolsoSor;             // aktuálisan a legalsó, téglákat tartalmazó sor
    bool valtozas;

    Tabla()
    {
        for (int i=0;i<JM;i++) for (int j=0;j<JSZ;j++) tegla[i][j]=' ';
        legfelsoSor = 0;
        legalsoSor  = JM-1;
        utolsoSor   = JM-1;
        valtozas    = false;
    }

    void ujSor()
    {
        _leptet(legfelsoSor,JM,'-');
        _leptet(legalsoSor,JM,'-');
        for (int i=0;i<JSZ;i++) tegla[legfelsoSor][i]=rand()%3+176; 
        valtozas=true;
    }

    void loves(const Agyu& agyu, long& pont)
    {
        byte a=agyu.agyucso;
        byte i=utolsoSor;
        while (tegla[i][a]==' ' && i>legfelsoSor) _leptet(i,JM,'-'); // tégla keresése az ágyú felett
        if (tegla[i][a]==TEGLA2 || tegla[i][a]==TEGLA3) {            // tégla kilövése
            tegla[i][a]--;
            pont+=100;
            valtozas=true;
        } else if (tegla[i][a]==TEGLA1) {
            tegla[i][a]=' ';
            pont+=100;
            valtozas=true;
        }
        if (i==utolsoSor) {                                          // utolsó sor léptetése
            bool sorUres=true;
            byte s=0;
            while (sorUres && s<JSZ) if (tegla[i][s++]!=' ') sorUres=false;
            if (sorUres) _leptet(utolsoSor,JM,'-');
        }
    }

    private:

    void _leptet(byte& valtozo, const int hatar, const char irany)
    {
        if (irany=='-') {
            if (valtozo==0) valtozo=hatar-1;
            else valtozo--;
        }
        else if (irany=='+') {
            if (valtozo==hatar) valtozo=0;
            else valtozo++;
        }
    }
};

struct Jatek
{
    long pont;

    Jatek(int argc, char** argv)
    {
        pont   = 0;
        _tick  = 0;
        _vege  = false;
        _speed = _inditasiParameterek(argc, argv);
    }

    bool ido()
    {
        if (_tick++%_speed==0) return true;
        else return false;
    }

    void kepRajzolasa(Tabla& tabla, const Agyu& agyu)
    {
        byte agyucso = agyu.agyucso+1;
        std::string kep = "\0";
        kep+="  EREDMENY: "+std::to_string(pont)+" pont\n  ";   // eredményjelző
        kep+=BAL_FELSO_SAROK;                                   // felső szegély
        for (int i=1;i<JSZ+1;i++) kep+=VIZSZINTES;
        kep+=JOBB_FELSO_SAROK;
        kep+="\n";                                              // játéktér
        for (int i=tabla.legfelsoSor;i<JM;i++) _teglakRajzolasa(tabla, i, kep);
        for (int i=0;i<tabla.legfelsoSor;i++) _teglakRajzolasa(tabla, i, kep);
        kep+="  ";
        kep+=BAL_ALSO_SAROK;                                    // alsó szegély
        for (int i=1;i<agyucso;i++) kep+=VIZSZINTES;
        kep+=AGYUCSO;
        for (int i=agyucso+1;i<JSZ+1;i++) kep+=VIZSZINTES;
        kep+=JOBB_ALSO_SAROK;
        kep+="\n  ";
        for (int i=0;i<agyucso-1;i++) kep+=" ";                 // ágyú talp
        kep+=AGYUTALP_BAL;
        kep+=AGYUTALP_KOZEP;
        kep+=AGYUTALP_JOBB;
        for (int i=agyucso+2;i<JSZ;i++) kep+=" ";
        kep+="\n\n";
        system("cls");                                          // kiírás
        std::cout<<kep;
        tabla.valtozas=false;
    }

    void kilepes()
    {
        if (_kerdes("Kilepsz? ")) _vege=true;
    }

    bool vege(const Tabla& tabla)
    {
        if (_tick==0) return _vege;
        else return _vege || _vegeEllenorzes(tabla);
    }

    private:

    usll _tick;
    long _speed;
    bool _vege;

    void _teglakRajzolasa(const Tabla& tabla, const int i, std::string& kep)
    {
        kep+=(i<10?" ":"")+std::to_string(i)+TABLA_SZELE;
        for (int j=0;j<JSZ;j++) kep+=char(tabla.tegla[i][j]);
        kep+=TABLA_SZELE;
        kep+="\n";
    }

    bool _vegeEllenorzes(const Tabla& tabla)
    {
        if (tabla.utolsoSor+1==tabla.legfelsoSor) return true;
        else if (tabla.utolsoSor==tabla.legalsoSor) return true;
        return false;
    }

    bool _kerdes(const std::string szoveg)
    {
        byte reakcio='0';
        std::cout<<szoveg;
        std::cin>>reakcio;
        if (reakcio!='y' && reakcio!='i' && reakcio!='Y' && reakcio!='I') return false;
        else return true;
    }

    long _inditasiParameterek(int c, char *v[])
    {
        char *p;
        if (c==1) return DEFAULT_SPEED;
        if (c==2) return strtol(v[1],&p,0);
        if (c>2) {
            _vege=true;
            std::cout<<"\nHiba: tul sok inditasi parameter! "
                     <<"A program parameter nelkul vagy csak egy parameterrel indithato.\n\n"
                     <<"\t"<<v[0]<<" <tickSpeed>\n\n"
                     <<"tickSpeed: egy idoegyseg hossza programciklusban kifejezve (default: 350000)\n";
        }
        return DEFAULT_SPEED;
    }
};

int main(int argc, char** argv)
{
    srand(time(NULL));
    Jatek jatek(argc, argv);
    Tabla tabla;
    Agyu  agyu;
    while (!jatek.vege(tabla)) {
        if (jatek.ido()) tabla.ujSor();
        if (tabla.valtozas) jatek.kepRajzolasa(tabla, agyu);
        if (agyu.mozgatas()) tabla.valtozas=true;
        if (agyu.parancs==32) tabla.loves(agyu, jatek.pont);
        if (agyu.parancs==27) jatek.kilepes();
    }
    return 0;
}
