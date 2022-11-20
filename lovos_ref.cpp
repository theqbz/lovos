/*
 *  QBZ - Lövős
 * 
 *  Kísérleti projekt egy egyszerű konzolos játékra
 *
 *  Irányítás: A, D, SPACE, ESC
 *
*/

#include <iostream>
#include <string>
#include <conio.h>
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

// TALÁLATÉRT ADHATÓ PONT
#define ALAP_PONT        100

using std::string;
using std::cout;
using std::cin;

struct Agyu
{
    uint8_t agyucso;
    uint8_t parancs;

    Agyu()
    {
        agyucso = 14;
        parancs = 0;
    }

    bool iranyitas()
    {
        _jatekosParancsa();
        if (parancs == 0) return false;
        if (parancs == 'a' && agyucso > 0) agyucso--;
        if (parancs == 'd' && agyucso < JSZ-1) agyucso++;
        return true;
    }

    private:

    void _jatekosParancsa()
    {
        parancs = 0;
        if (_kbhit()) parancs = _getch();
        if (parancs != 'a' && parancs != 'd' && parancs != ' ' && int(parancs) != 27) parancs = 0;
    }
};

struct Tabla
{
    uint8_t tegla[JM][JSZ];
    uint8_t legfelsoSor;           // aktuálisan a tábla legtetején lévő sor
    uint8_t legalsoSor;            // aktuálisan a tábla legalján lévő sor
    uint8_t utolsoSor;             // aktuálisan a legalsó, téglákat tartalmazó sor
    bool    valtozas;

    Tabla()
    {
        legfelsoSor = 0;
        legalsoSor  = JM-1;
        utolsoSor   = JM-1;
        valtozas    = false;
        for (uint8_t s = 0; s < JM; s++) for (uint8_t o = 0; o < JSZ; o++) tegla[s][o] = ' ';
    }

    void ujSor()
    {
        _leptet(legfelsoSor, JM, '-');
        _leptet(legalsoSor, JM, '-');
        for (uint8_t o = 0; o < JSZ; o++) tegla[legfelsoSor][o] = rand() % 3 + TEGLA1; 
        valtozas = true;
    }

    uint64_t loves(const Agyu& agyu)
    {
        uint8_t  sor  = _legalsoTeglaSora(agyu.agyucso);
        uint64_t pont = _teglaKilovese(sor, agyu.agyucso);
        if (sor == utolsoSor && _utolsoSorKiurult()) _leptet(utolsoSor, JM, '-');
        return pont;
    }

    private:

    void _leptet(uint8_t& valtozo, const uint8_t hatar, const uint8_t irany)
    {
        if (irany == '-') {
            if (valtozo == 0) valtozo=hatar - 1;
            else valtozo--;
        }
        else if (irany == '+') {
            if (valtozo == hatar) valtozo = 0;
            else valtozo++;
        }
    }

    const uint8_t _legalsoTeglaSora(const uint8_t agyucso)
    {
        uint8_t s = utolsoSor;
        while (tegla[s][agyucso] == ' ' && s > legfelsoSor) _leptet(s,JM,'-');
        return s;
    }

    uint64_t _teglaKilovese(uint8_t s, uint8_t a)
    {
        if (tegla[s][a] == TEGLA2 || tegla[s][a] == TEGLA3) {
            tegla[s][a]--;
            valtozas = true;
            return ALAP_PONT;
        } else if (tegla[s][a] == TEGLA1) {
            tegla[s][a] = ' ';
            valtozas = true;
            return ALAP_PONT;
        }
        return 0;
    }

    const bool _utolsoSorKiurult()
    {
        uint8_t o = 0;
        while (o < JSZ && tegla[utolsoSor][o] == ' ') o++;
        if (o == JSZ) return true;
        return false;
    }
};

struct Jatek
{
    uint64_t pont;

    Jatek(int argc, char** argv)
    {
        pont      = 0;
        _tick     = 0;
        _vege     = false;
        _eredmeny = 0;
        _speed    = _inditasiParameterek(argc, argv);
    }

    const bool ido()
    {
        if (_tick++ % _speed == 0) return true;
        else return false;
    }

    void kepRajzolasa(Tabla& tabla, const Agyu& agyu)
    {
        uint8_t agyucso = agyu.agyucso+1;
        string  kep     = "\0";
        kep += "  EREDMENY: "+std::to_string(pont)+" pont\n  ";   // eredményjelző
        kep += BAL_FELSO_SAROK;                                   // felső szegély
        for (uint8_t o=1; o < JSZ+1; o++) kep += VIZSZINTES;
        kep += JOBB_FELSO_SAROK;
        kep += "\n";                                              // játéktér
        for (uint8_t s=tabla.legfelsoSor; s < JM; s++) _teglakRajzolasa(tabla, s, kep);
        for (uint8_t s=0; s < tabla.legfelsoSor; s++) _teglakRajzolasa(tabla, s, kep);
        kep += "  ";
        kep += BAL_ALSO_SAROK;                                    // alsó szegély
        for (uint8_t o=1; o < agyucso; o++) kep += VIZSZINTES;
        kep += AGYUCSO;
        for (uint8_t o=agyucso+1; o < JSZ+1; o++) kep += VIZSZINTES;
        kep += JOBB_ALSO_SAROK;
        kep += "\n  ";
        for (uint8_t o=0; o < agyucso-1; o++) kep += " ";         // ágyú talp
        kep += AGYUTALP_BAL;
        kep += AGYUTALP_KOZEP;
        kep += AGYUTALP_JOBB;
        for (uint8_t o=agyucso+2; o < JSZ; o++) kep += " ";
        kep += "\n\n";
        system("cls");                                            // kiírás
        cout << kep;
        tabla.valtozas = false;
    }

    void kilepes()
    {
        if (_kerdes("Kilepsz? ")) _vege = true;
    }

    const bool vege(const Tabla& tabla)
    {
        if (_tick == 0) return _vege;
        else return _vege || _vegeEllenorzes(tabla);
    }

    const void eredmeny()
    {
        if (_eredmeny == 1) cout << "\nGratulalok, NYERTEL! :-)\n";
        if (_eredmeny == 2) cout << "\nSajnos vesztettel :-(\n";
    }

    private:

    uint8_t  _eredmeny;
    uint64_t _tick;
    uint64_t _speed;
    bool     _vege;

    void _teglakRajzolasa(const Tabla& tabla, const uint8_t s, string& kep)
    {
        kep += (s < 10 ? " " : "") + std::to_string(s);
        kep += _tablaSzele(tabla, s);
        for (uint8_t o=0; o < JSZ; o++) kep += char(tabla.tegla[s][o]);
        kep += _tablaSzele(tabla, s);
        kep += "\n";
    }

    const uint8_t _tablaSzele(const Tabla& tabla, const uint8_t s)
    {
        return s == tabla.utolsoSor ? UTOLSO_SOR_JEL : FUGGOLEGES;
    }

    const bool _vegeEllenorzes(const Tabla& tabla)
    {
        if (tabla.utolsoSor + 1 == tabla.legfelsoSor) {
            _eredmeny = 1;
            return true;
        }
        else if (tabla.utolsoSor == tabla.legalsoSor) {
            _eredmeny = 2;
            return true;
        }
        _eredmeny = 0;
        return false;
    }

    const bool _kerdes(const string szoveg)
    {
        uint8_t reakcio='0';
        cout << szoveg;
        cin >> reakcio;
        if (reakcio != 'y' && reakcio != 'i' && reakcio != 'Y' && reakcio != 'I') return false;
        else return true;
    }

    uint64_t _inditasiParameterek(int c, char* v[])
    {
        char* p;
        if (c == 1) return DEFAULT_SPEED;
        if (c == 2) return strtoll(v[1], &p, 0);
        if (c > 2) {
            _vege = true;
            cout << "\nHiba: tul sok inditasi parameter! "
                 << "A program parameter nelkul vagy csak egy parameterrel indithato.\n\n"
                 << "\t" << v[0] << " <tickSpeed>\n\n"
                 << "tickSpeed: egy idoegyseg hossza programciklusban kifejezve (default: 350000)\n";
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
        if (agyu.iranyitas()) tabla.valtozas = true;
        if (agyu.parancs == 32) jatek.pont += tabla.loves(agyu);
        if (agyu.parancs == 27) jatek.kilepes();
    }
    jatek.eredmeny();
    return 0;
}
