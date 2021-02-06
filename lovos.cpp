#include <iostream>
#include <conio.h>
#include <string>
#include <iomanip>

#define KSZ 60					// Kepernyo szelessege
#define KM 20					// Kepernyo magassaga
#define JSZ 30					// Jatekter szelessege
#define JM 15					// Jatekter magassaga
#define FUGGOLEGES 179			// - Jatekter függőleges oldala
#define VIZSZINTES 196			// ¦ Jatekter vízszintes oldala
#define BAL_FELSO_SAROK 218		// - Jatekter bal felso sarka
#define JOBB_FELSO_SAROK 191		// ¬ Jatekter jobb felso sarka
#define BAL_ALSO_SAROK 192		// L Jatekter bal also sarka
#define JOBB_ALSO_SAROK 217		// - Jatekter jobb also sarka
#define UTOLSO_SOR_JEL 197

#define AGYUCSO 186			// agyucso
#define AGYU_TALP_BAL 201
#define AGYU_TALP_KOZEP 202
#define AGYU_TALP_JOBB 187

#define ONETICK 350000

typedef unsigned long long ull;

struct tabla
{
	unsigned char tegla[JM][JSZ]={32};
	int legfelsoSor=0;
	int utolsoSor=0;
};

char jatekosIranyitasa(bool& valtozas)
{
	unsigned char parancs='0';
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

void teglakRajzolasa(tabla& teglak, int i)
{
	i<10 ? std::cout<<" "<<i : std::cout<<i;	//debug
	std::cout<<(i==teglak.utolsoSor?char(UTOLSO_SOR_JEL):char(FUGGOLEGES));
	for (int j=0;j<JSZ;j++) std::cout<<char(teglak.tegla[i][j]);
	std::cout<<(i==teglak.utolsoSor?char(UTOLSO_SOR_JEL):char(FUGGOLEGES));
	std::cout<<std::endl;
}

void kepernyoRajzolasa(tabla& teglak, int* agyu, bool& valtozas, ull gameTick)
{
	system("cls");

	// tabla teteje
	std::cout<<"   ";	//debug
	for (int i=0;i<JSZ;i++) std::cout<<i%10;	//debug
	std::cout<<" "<<gameTick<<"|"<<gameTick%ONETICK<<std::endl<<"  ";	//debug
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
	std::cout<<"  ";	//debug
	for (int i=0;i<JSZ+2;i++)
	{
		if (i==0) std::cout<<char(BAL_ALSO_SAROK);
		else if (i==JSZ+1) std::cout<<char(JOBB_ALSO_SAROK);
		else if (i==agyu[1]) std::cout<<char(AGYUCSO);
		else std::cout<<char(VIZSZINTES);
	}
	std::cout<<std::endl;

	// agyu talp
	std::cout<<"  ";	//debug
	for (int i=0;i<JSZ+2;i++)
	{
		if (i==agyu[0]) std::cout<<char(AGYU_TALP_BAL);
		else if (i==agyu[1]) std::cout<<char(AGYU_TALP_KOZEP);
		else if (i==agyu[2]) std::cout<<char(AGYU_TALP_JOBB);
		else std::cout<<" ";
	}
	std::cout<<std::endl<<"agyu[3]:("<<agyu[0]<<","<<agyu[1]<<","<<agyu[2]<<")"<<std::endl;	//debug
	valtozas=false;
}

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
	for (int i=0;i<JSZ;i++)
	{
		unsigned char T=rand()%3+176;
		// ide meg kell majd valami, amitol gyakoribb lesz a "vekony" tegla (176)...
		teglak.tegla[teglak.legfelsoSor][i]=T;
	}
	valtozas=true;
}

void loves(tabla& teglak, int* agyu, bool& valtozas, long& pontszam)
{
	// itt esetleg gond lehet az ellenorzessel,
	// mert ha pl. a legfelso sor 1 az utolso sor 0 akkor nem mukodik.
	int i=teglak.utolsoSor;
// d	std::cout<<"LFS:"<<teglak.legfelsoSor<<"|"<<teglak.tegla[i][agyu[1]-1]<<std::endl;
	while (teglak.tegla[i][agyu[1]-1]==' ' && i>teglak.legfelsoSor){std::cout<<"i="<<i<<std::endl;
		leptet(i,JM,'-');
	}

	std::cout<<"i talalat:"<<i<<std::endl;
	if (teglak.tegla[i][agyu[1]-1]==177 || teglak.tegla[i][agyu[1]-1]==178) teglak.tegla[i][agyu[1]-1]--;
	else if (teglak.tegla[i][agyu[1]-1]==176) teglak.tegla[i][agyu[1]-1]=' ';
	if (i==teglak.utolsoSor)
	{
		bool nincsTegla=true;
		int s=0;
		while (nincsTegla && s<JSZ)
		{
			if (teglak.tegla[i][s]!=' ') nincsTegla=false;
			s++;
		}
		// ha az utolso sor eleve a legfelso sor volt,
		// akkor nem kell leptetni, mert akkor vege a jateknak
		// kiveve, ha a jatekos nyerni akar, ha kitakaritotta a tablat
		if (nincsTegla) leptet(teglak.utolsoSor,JM,'-');
	}
	valtozas=true;
// d	std::cin.get();
}

bool kerdes(std::string szoveg)
{
	bool valasz=false;
	unsigned char reakcio='0';
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

int main()
{
	srand(time(NULL));
	tabla teglak;
	long pontszam;
	int agyu[3]={ 14,15,16 };
	ull gameTick=0;
	char parancs=0;
	bool valtozas=true;
	bool jatekVege=false;

	tablaInicializalasa(teglak);

	while (!jatekVege)
	{
		// kell valami rendes idomeres, hogy ne a processzor sebessegetol fuggjon
		if (gameTick%ONETICK==0) teglaGeneralas(teglak, valtozas);
		if (valtozas) kepernyoRajzolasa(teglak, agyu, valtozas, gameTick);
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
	return 0;
}
