# ArmagetrongRG

>Armagetron je igrica u kojoj je cilj navesti protivnika da predje preko svog ili vašeg traga (udari u zid). Ova "replika" je namenjena striktno za dva igrača.


### Instalacija

Da bi se program izvršavao na vašem računaru, za početak neophodno je posedovati OpenGL biblioteku instaliranu: 


Debian, *buntu, Mint...:

```sh
sudo apt-get install binutils-gold freeglut3 freeglut3-dev libsoil-dev
```

Klonirati, ili preuzeti sa github-a, kod projekta, zatim raspakovati i
pozicionirati se u datoteku gde je smešten program, ući u direktorijum 
programa, zatim pokrenuti makefile:

```sh
make
```

Sačekati da se komanda završi, dok ne dobijete ispis:

```sh
[100%] Built target Armagetron
```

Preostao je korak pokretanja samog programa: 

```sh
./Armagetron
```

Alternativni način je pokretanje programa kroz neko od razvojnih okruženja (CLion korišćen u izradi).

## Komande

### P1

<kbd> :arrow_left: </kbd> Skretanje levo
<kbd> :arrow_right: </kbd> Skretanje desno

### P2

<kbd> A </kbd> Skretanje levo
<kbd> D </kbd> Skretanje desno

### Oba igrača

<kbd> R </kbd> Restartovanje igrice
<kbd> Space </kbd> Pauza/Start igrice
<kbd> Esc </kbd> Prekid igrice