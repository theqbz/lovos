/*
 *	QBZ - Lövős
 * 
 * 	Kísérleti projekt egy egyszerű konzolos játékra
 *
 * 	Irányítás: A, D, SPACE, ESC
 *
*/

#include <iostream>
#include <conio.h>
#include <string>
#include <iomanip>
#include <stdlib.h>

// egy idoegyseg a jatekban, programciklusban kifejezve
#define DEFAULT_TICKSPEED 350000

// jatektabla merete
#define JSZ 30					// Jatekter szelessege
#define JM 15					// Jatekter magassaga

// rajzolo karakterek
#define FUGGOLEGES 179				// - Játéktér függőleges oldala
#define VIZSZINTES 196				// ¦ Játéktér vízszintes oldala
#define BAL_FELSO_SAROK 218			// - Játéktér bal felso sarka
#define JOBB_FELSO_SAROK 191			// ¬ Játéktér jobb felso sarka
#define BAL_ALSO_SAROK 192			// L Játéktér bal also sarka
#define JOBB_ALSO_SAROK 217			// - Játéktér jobb also sarka
#define UTOLSO_SOR_JEL 197			// + Ez jelzi az utolsó sort
#define AGYU_CSO 186				// ¦ Ágyúcső
#define AGYU_TALP_BAL 201			// - Ágyútalp bal széle
#define AGYU_TALP_KOZEP 202			// ¦ Ágyútalp közepe
#define AGYU_TALP_JOBB 187			// ¬ Ágyútalp jobb széle
#define TEGLA1 176				// - Vékony tégla
#define TEGLA2 177				// - Közepes tégla
#define TEGLA3 178				// - Vastag tégla

typedef unsigned long long ull;
typedef unsigned char byte;

struct tabla
{
	byte tegla[JM][JSZ]={32};
	int legfelsoSor=0;
	int utolsoSor=0;
};

char jatekosIranyitasa(bool& valtozas)
{
	byte parancs='0';
	if (_kbhit()) parancs=_getch();
	if (parancs!='a' && parancs!='d' && parancs!=' ' && int(parancs)!=27) parancs='0';
	else valtozas=true;
	return parancs;
}

void agyuMozgatasa(int* agyu, char irany)
{
	if (irany=='a' && agyu[1]>1) for (int i=0;i<3;i++) agyu[i]--;
	if (irany=='d' && agyu[1]<JSZ) for (int i=0;i<3;i++) agyu[i]++;
}

#define HELY_BALRA ("  ")

void teglakRajzolasa(tabla& teglak, int i)
{
#define TABLA_SZELE (i==teglak.utolsoSor?char(UTOLSO_SOR_JEL):char(FUGGOLEGES))

	std::cout<<(i<10?" ":"")<<i;
	std::cout<<TABLA_SZELE;
	for (int j=0;j<JSZ;j++) std::cout<<char(teglak.tegla[i][j]);
	std::cout<<TABLA_SZELE;
	std::cout<<std::endl;

#undef TABLA_SZELE
}

void kepernyoRajzolasa(tabla& teglak, int* agyu, bool& valtozas, long pontszam)
{
	system("cls");

	// tabla teteje
	std::cout<<HELY_BALRA<<"EREDMENY: "<<pontszam<<" pont"<<std::endl;
	std::cout<<"  ";
	for (int i=0;i<JSZ+2;i++)
	{
		if (i==0) std::cout<<char(BAL_FELSO_SAROK);
		else if (i==JSZ+1) std::cout<<char(JOBB_FELSO_SAROK);
		else std::cout<<char(VIZSZINTES);
	}
	std::cout<<std::endl;

	// tabla kozepe: elobb a "legfelso sortol" majd a tobbi
	for (int i=teglak.legfelsoSor;i<JM;i++) teglakRajzolasa(teglak, i);
	for (int i=0;i<teglak.legfelsoSor;i++) teglakRajzolasa(teglak, i);

	// tabla alja + agyucso
	std::cout<<HELY_BALRA;
	for (int i=0;i<JSZ+2;i++)
	{
		if (i==0) std::cout<<char(BAL_ALSO_SAROK);
		else if (i==JSZ+1) std::cout<<char(JOBB_ALSO_SAROK);
		else if (i==agyu[1]) std::cout<<char(AGYU_CSO);
		else std::cout<<char(VIZSZINTES);
	}
	std::cout<<std::endl;

	// agyu talp
	std::cout<<HELY_BALRA;
	for (int i=0;i<JSZ+2;i++)
	{
		if (i==agyu[0]) std::cout<<char(AGYU_TALP_BAL);
		else if (i==agyu[1]) std::cout<<char(AGYU_TALP_KOZEP);
		else if (i==agyu[2]) std::cout<<char(AGYU_TALP_JOBB);
		else std::cout<<" ";
	}
	std::cout<<std::endl<<std::endl;
	valtozas=false;
}

#undef HELY_BALRA

void tablaInicializalasa(tabla& teglak)
{
	for (int i=0;i<JM;i++) for (int j=0;j<JSZ;j++) teglak.tegla[i][j]=' ';
	teglak.legfelsoSor=0;
	teglak.utolsoSor=JM-1;
}

void leptet(int& valtozo, int hatarertek, char irany)
{
	if (irany='-')
	{
		if (valtozo==0) valtozo=hatarertek-1;
		else valtozo--;
	}
	else if (irany='+')
	{
		if (valtozo==hatarertek) valtozo=0;
		else valtozo++;
	}
}

void teglaGeneralas(tabla& teglak, bool& valtozas)
{
	leptet(teglak.legfelsoSor,JM,'-');
	for (int i=0;i<JSZ;i+=1)
	{
		byte T=rand()%3+176;
		// ide meg kell majd valami, amitol gyakoribb lesz a "vekony" tegla (176)...
		teglak.tegla[teglak.legfelsoSor][i]=T;
	}
	valtozas=true;
}

void loves(tabla& teglak, int* agyu, bool& valtozas, long& pontszam)
{
#define AGYUCSO (agyu[1]-1)

	// Alulrol az elso tegla keresese
	// abban az oszlopban, ahol az agyucso van
	int i=teglak.utolsoSor;
	while (teglak.tegla[i][AGYUCSO]==' ' && i>teglak.legfelsoSor) leptet(i,JM,'-');

	// A megtalalt tegla kilovese
	if (teglak.tegla[i][AGYUCSO]==TEGLA2 || teglak.tegla[i][AGYUCSO]==TEGLA3)
	{
		teglak.tegla[i][AGYUCSO]--;
		pontszam+=100;
	}
	else if (teglak.tegla[i][AGYUCSO]==TEGLA1)
	{
		teglak.tegla[i][AGYUCSO]=' ';
		pontszam+=100;
	}

	// Ha a kiloves az utolso sorban tortent, akkor
	// annak ellenorzese, hogy van-e meg tegla az utolso sorban
	if (i==teglak.utolsoSor)
	{
		bool nincsTegla=true;
		int s=0;
		while (nincsTegla && s<JSZ)
		{
			if (teglak.tegla[i][s]!=' ') nincsTegla=false;
			s++;
		}
		// Ha kiutult az utolso sor, akkor egyel feljebb lepteti.
		// - Ha az utolso sor egyben a legfelso sor is,
		//   akkor gyozelemmel er veget a jatek.
		// - Ha az a cel, hogy "vegtelen" sokaig lehessen jatszani
		//   akkor nem kell leptetni az utolso sort,
		//   ha az egyben a legfelso sor is.
		if (nincsTegla) leptet(teglak.utolsoSor,JM,'-');
	}
	valtozas=true;

#undef AGYUCSO
}

bool kerdes(std::string szoveg)
{
	bool valasz=false;
	byte reakcio='0';
	std::cout<<szoveg;
	std::cin>>reakcio;
	if (reakcio!='y' && reakcio!='i' && reakcio!='Y' && reakcio!='I') valasz=false;
	else valasz=true;
	return valasz; 
}

bool vegeEllenorzes(tabla& teglak)
{
	bool gameOver=false;
	if (teglak.utolsoSor+1==JM && teglak.legfelsoSor==0) gameOver=true;
	else if (teglak.utolsoSor+1==teglak.legfelsoSor) gameOver=true;
	return gameOver;	
}

int inditasiParameterek(int c, char *v[])
{
	int vissza;
	char *p;
	if (c>2)
	{
		std::cout<<"Hiba! Tul sok inditasi parameter!\n"
			<<"A program parameter nelkul vagy csak egy parameterrel indithato:\n\n"
			<<v[0]<<" <tickSpeed>\n\n"
			<<"tickSpeed: egy idoegyseg hossza programciklusban kifejezve (default: 350000)"<<std::endl;
		vissza=0;
	}
	if (c==2) vissza=strtol(v[1],&p,0);
	if (c==1) vissza=DEFAULT_TICKSPEED;
	return vissza;
}

int main(int argc, char** argv)
{
	int tickSpeed = inditasiParameterek(argc, argv);
	if (tickSpeed!=0)
	{
		srand(time(NULL));
		tabla teglak;
		long pontszam=0;
		int agyu[3]={ 14,15,16 };
		ull gameTick=0;
		char parancs=0;
		bool valtozas=true;
		bool jatekVege=false;

		tablaInicializalasa(teglak);

		while (!jatekVege)
		{
			// kell valami rendes idomeres, hogy ne a processzor sebessegetol fuggjon
			if (gameTick%tickSpeed==0) teglaGeneralas(teglak, valtozas);
			if (valtozas) kepernyoRajzolasa(teglak, agyu, valtozas, pontszam);
			parancs=jatekosIranyitasa(valtozas);
			jatekVege=vegeEllenorzes(teglak);
			if (parancs!=0 && !jatekVege)
			{
				if (parancs==27) jatekVege=kerdes("Kilepsz? ");
				else if (parancs==32) loves(teglak, agyu, valtozas, pontszam);
				else agyuMozgatasa(agyu, parancs);
			}
			gameTick++;
		}
		//	std::cout<<"\nVEGE A JATEKNAK"<<std::endl;
	}
	return 0;
}
