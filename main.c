#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include "SOIL.h"

/*postavljamo interval tajmera na 80*/
#define TIMER_INTERVAL 60
/*tajmer id nam je 0*/
#define TIMER_ID 0
/*duzina niza tacaka za iscrtavanje zida*/
#define MAX_LEN 5000

/* Fleg koji odredjuje stanje tajmera. */
static int animation_active;

/* Deklaracije callback funkcija. */
static void on_keyboardSpecial(int key, int x, int y);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);
void initialize();

/*deklaracija funkcija koje ce biti pozivane od strane neke od callback f-ja*/
void display();
void drawWall(int);
void displayPlayer(int);
void loadTextures();
void checkCollision();
void initMatrix();

/*struktura za x i z koordinate zida (traga)*/
typedef struct {
    float x;
    float z;
} Zid;
/*imena tekstura*/
GLuint zid, pod, player1, player2;

/*koeficijent kretanja 0,1,2,3 u zavisnosti da li se x ili z koordinata smanjuje ili povecava*/
unsigned int koefKretanjaP1, koefKretanjaP2;

/*promenljive za oznaku pobednika i oznaku za krkaj*/
int winner, end;

/*matrica kkolizije 1-ima zid (nas, protivnokov, zid od arene), 0-nema zid, opseg koordinata je [-50,50], stoga 101 polje, gde su krajna polja 1*/
unsigned int collisionMatrix[101][101];

/*dimenzije prozora*/
int widthP, heightP;

/*parametar za priblizavanje kamere koja gleda u teksturu pobednika*/
int cameraParam;

/*stepen rotacije, x koordinata, z koordinata za igraca 1, zatim za igraca 2*/
float rotateP1, xP1, zP1, rotateP2, xP2, zP2;

/*niz za  koordinate zida igraca 1, zatim igraca 2*/
Zid zidP1[MAX_LEN], zidP2[MAX_LEN];

/*poslednja dodata tacka u niz za zid za igraca 1, zatim igraca 2*/
int tackaP1, tackaP2;


int main(int argc, char **argv) {
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(1500, 1500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se funkcije za obradu dogadjaja. */
    glutSpecialFunc(on_keyboardSpecial);
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
    glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    /*inicijalizuje se prostor*/
    initialize();

    /* Ulazi se u glavnu petlju. */
    glutMainLoop();

    return 0;
}


static void on_keyboard(unsigned char key, int x, int y) {
    /*ispis kako ne bi kompajler prijavljivao warning-e kako nisu koriscene promenljive*/
    fprintf(stderr, "%d %d\n", x, y);
    /*klikom na esc dugme, izlazi se iz programa*/
    switch (key) {
        case 27:
            /* Zavrsava se program. */
            exit(0);
        case 'a':
        case 'A':
            /*p2 skreni levo, ubaci koordinate tacke skretanja u niz za zid*/
            if(animation_active){
                rotateP2 -= 90;
                rotateP2 = (int)rotateP2 % 360;
                zidP2[tackaP2].x = xP2;
                zidP2[tackaP2].z = zP2;
                tackaP2++;
                koefKretanjaP2 = (koefKretanjaP2 - 1)%4;
                glutPostRedisplay();
            }
            break;
        case 'd':
        case 'D':
            /*p2 skreni desno, ubaci koordinate tacke skretanja u niz za zid*/
            if(animation_active){
                rotateP2 += 90;
                rotateP2 = (int)rotateP2 % 360;
                zidP2[tackaP2].x = xP2;
                zidP2[tackaP2].z = zP2;
                tackaP2++;
                koefKretanjaP2 = (koefKretanjaP2 + 1)%4;
                glutPostRedisplay();
            }
            break;
            /*restuj stanje igrice na pocetno*/
        case 'r':
        case 'R':
            initialize();
            glutPostRedisplay();
            break;
        case 32:
            /*space -> pauza/start, ide naizmenicno, pa je resenje mod 2*/
            animation_active = (animation_active+1)%2;
            if(animation_active || end)
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
        default:
            break;
    }
}

static void on_keyboardSpecial(int key, int x, int y) {
    /*ispis kako kompajler ne bi prijavljivao warning-e*/
    fprintf(stderr, "%d %d\n", x, y);
    switch (key) {
        case 27:
            /* Zavrsava se program. */
            exit(0);

        case GLUT_KEY_LEFT:
            /*p1 skreni levo, dodaj tacku skretanja u niz za zid*/
            if (animation_active){
                rotateP1 += 90;
                rotateP1 = (int)rotateP1 % 360;
                zidP1[tackaP1].x = xP1;
                zidP1[tackaP1].z = zP1;
                tackaP1++;
                koefKretanjaP1 = (koefKretanjaP1 -1)%4;
                glutPostRedisplay();
            }
            break;

        case GLUT_KEY_RIGHT:
            /*p1 skreni desno, dodaj tacku skretanja u niz za zid*/
            if (animation_active){
                rotateP1 -= 90;
                rotateP1 = (int)rotateP1 % 360;
                zidP1[tackaP1].x = xP1;
                zidP1[tackaP1].z = zP1;
                tackaP1++;
                koefKretanjaP1 = (koefKretanjaP1 +1)%4;
                glutPostRedisplay();
            }
            break;
        default:
            break;
    }
}

static void on_timer(int value) {
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
    if (value != 0)
        return;

    if(animation_active){

        if(!end) {
            /*nove koordinate igraca, u odnosu na to da li je skrenuo ili ne*/

            xP1 += sin(M_PI / 2 * rotateP1 / 90);
            zP1 += cos(M_PI / 2 * rotateP1 / 90);

            xP2 += sin(M_PI / 2 * rotateP2 / 90);
            zP2 -= cos(M_PI / 2 * rotateP2 / 90);

            checkCollision();
        }

        /*ako je kraj, onda prikazujemo sliku pobednika tako sto priblizavamo kameru*/
        if(end == 1 && cameraParam < 1400){
                /*uvecavamo parametar po kom se kamera priblizava slici pobednika*/
                cameraParam+=100;
        }
        /* Forsira se ponovno iscrtavanje prozora. */
        glutPostRedisplay();
    }

    /*podesavanje tajmera*/
    if(animation_active)
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}

static void on_reshape(int width, int height) {
    /*cuvaju se dimenzije prozora*/
    widthP = width;
    heightP = height;

    /* Postavljaju se parametri projekcije. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 1500);
}

void initialize() {
    /*postavljaju se inicijalne vrednosti na promenljive, animacija nije aktivna i pocetne pozicije igraca koji nisu rotirani*/
    animation_active = 0;

    rotateP1 = 0;
    xP1 = 0;
    zP1 = -45;

    xP2 = 0;
    zP2 = 45;
    rotateP2 = 0;

    /*koeficijenti kretanja, odnosno igraci na pocetku nisu skretali*/
    koefKretanjaP1 = 0;
    koefKretanjaP2 = 0;

    /*pocetne tacke se dodaju u niz za zid, da bi znali odakle pocinje iscrtavanje zida*/
    tackaP1 = 0;
    tackaP2 = 0;

    zidP1[tackaP1].x = xP1;
    zidP1[tackaP1].z = zP1-2;

    zidP2[tackaP2].x = xP2;
    zidP2[tackaP2].z = zP2+2;

    /*pomeramo se za jedno mesto u zidu*/
    tackaP1++;
    tackaP2++;


    /*podesavamo inicijalne vrednosti za kameru pobednika, da nije kraj i da nema pobednika,
     * matrica kolizije se postavlja da ima jedinice samo na mestima gde je zid arene*/
    cameraParam = 0;
    end = 0;
    winner = 0;
    initMatrix();

    /*ucitavamo teksture koje su nam potrebne*/
    loadTextures();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void on_display(void) {
    /*prozor postavljamo da bude preko celog ekrana*/
    glutFullScreen();

    /*cistimo bafere koje koristimo*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    /*postavljamo pogled za igraca 2, gornji levi ugao ekrana i iscrtavamo objekte*/
    glViewport(0, heightP/2, widthP/2, heightP/2);
    glLoadIdentity();
    /*u odnosu na to kako se igrac krece, postavljamo kameru iza njega, da gleda u smeru kretanja*/
    if(koefKretanjaP2 == 0)
        gluLookAt(xP2-1, 3, zP2+3, xP2, 0, zP2-10, 0, 1, 0);
    else if(koefKretanjaP2 == 1)
        gluLookAt(xP2-3, 3, zP2-1, xP2+10, 0, zP2, 0, 1, 0);
    else if(koefKretanjaP2 == 2)
        gluLookAt(xP2-1, 3, zP2-3, xP2, 0, zP2+10, 0, 1, 0);
    else if(koefKretanjaP2 == 3)
        gluLookAt(xP2+3, 3, zP2+1, xP2-10, 0, zP2, 0, 1, 0);
    display();

    /*postavljamo poged za igraca 1, donji desni ugao ekrana i iscrtavamo objekte*/
    glViewport(widthP/2, 0, widthP/2, heightP/2);
    glLoadIdentity();
    /*u odnosu na to kako se igrac krece, postavljamo kameru iza njega, da gleda u smeru kretanja*/
    if(koefKretanjaP1 == 0)
        gluLookAt(xP1-1, 3, zP1-3, xP1, 0, zP1+10, 0, 1, 0);
    else if(koefKretanjaP1 == 1)
        gluLookAt(xP1+3, 3, zP1-1, xP1-10, 0, zP1, 0, 1, 0);
    else if(koefKretanjaP1 == 2)
        gluLookAt(xP1-1, 3, zP1+3, xP1, 0, zP1-10, 0, 1, 0);
    else if(koefKretanjaP1 == 3)
        gluLookAt(xP1-3, 3, zP1+1, xP1+10, 0, zP1, 0, 1, 0);
    display();

    /*viewport u kom se nalazi obavestanje da je prvi igrac pobedio*/
    glViewport(0,0, widthP/2, heightP/2);
    glLoadIdentity();
    gluLookAt(0,1500-cameraParam, 0, 0,0,0, 1,0,0);
    if(winner == 2)
        displayPlayer(2);

    /*viewport u kom se nalazi obavestenje da je drugi igrac pobedio */
    glViewport(widthP/2,heightP/2, widthP/2, heightP/2);
    glLoadIdentity();
    gluLookAt(0,1500-cameraParam,0, 0,0,0, 1,0,0);
    if(winner == 1)
        displayPlayer(1);

    /* Postavlja se nova slika u prozor. */
    glutSwapBuffers();
}

void display() {
    /*iscrtavamo zid po zid, koristeci teksuru za zid*/
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, zid);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(50, 0, -50);

        glTexCoord2f(1, 0);
        glVertex3f(50, 0, 50);

        glTexCoord2f(1, 1);
        glVertex3f(50, 100, 50);

        glTexCoord2f(0, 1);
        glVertex3f(50, 100, -50);
        glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(50, 0, 50);

        glTexCoord2f(1, 0);
        glVertex3f(-50, 0, 50);

        glTexCoord2f(1, 1);
        glVertex3f(-50, 100, 50);

        glTexCoord2f(0, 1);
        glVertex3f(50, 100, 50);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(-50, 0, 50);

        glTexCoord2f(1, 0);
        glVertex3f(-50, 0, -50);

        glTexCoord2f(1, 1);
        glVertex3f(-50, 100, -50);

        glTexCoord2f(0, 1);
        glVertex3f(-50, 100, 50);
    glEnd();

    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(-50, 0, -50);

        glTexCoord2f(1, 0);
        glVertex3f(50, 0, -50);

        glTexCoord2f(1, 1);
        glVertex3f(50, 100, -50);

        glTexCoord2f(0, 1);
        glVertex3f(-50, 100, -50);
    glEnd();


    /*iscrtavamo pod, koristeci teksturu za pod*/
    glBindTexture(GL_TEXTURE_2D, pod);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);

        glTexCoord2f(0, 0);
        glVertex3f(-50, 0, 50);

        glTexCoord2f(50, 0);
        glVertex3f(50, 0, 50);

        glTexCoord2f(50, 50);
        glVertex3f(50, 0, -50);

        glTexCoord2f(0, 50);
        glVertex3f(-50, 0, -50);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    GLfloat light_position[] = { 0.6, 1.4, 0.9, 0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    /*Ukljucuje se osvjetljenje i podesavaju parametri svetla */

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    /*iscrtavanje igraca 2*/
    glPushMatrix();
        glColor3f(0.16,0.53,0.78);
        glTranslatef(xP2, 0.15, zP2);
        glRotatef(rotateP2, 0, 1, 0);
        glScalef(0.2,0.7,0.85);
        glutSolidCube(1);
    glPopMatrix();
    drawWall(2); /*iscrtavanje traga igraca 2*/

    /*iscrtavanje igraca 1*/
    glPushMatrix();
        glColor3f(0.93, 0.16, 0.22);
        glTranslatef(xP1, 0.15, zP1);
        glRotatef(rotateP1, 0, 1, 0);
        glScalef(0.2,0.7,0.85);
        glutSolidCube(1);
    glPopMatrix();
    drawWall(1); /*iscrtavanje traga igraca 1*/
}

void drawWall(int p){
    /*iscrtavamo zidove (tragove) igraca, za to ukljucujemo BLEND, sa datim parametrima, da bi zid bio proziran i na koji nacin bi se to desavalo*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(5);
    /*igrac 1*/
    if(p==1){
        glColor4f(0.93, 0.16, 0.22, 0.7);
        int i = 0;
        while(i < tackaP1){
            glBegin(GL_TRIANGLES);

                glVertex3f(zidP1[i].x, 0, zidP1[i].z);
                glVertex3f(zidP1[i].x, 0.5, zidP1[i].z);
                /*ako smo dosli do poslednje dodate tacke u niz, nju spajamo sa trenutnom pozicijom igraca*/
                i == tackaP1 -1 ? glVertex3f(xP1, 0, zP1): glVertex3f(zidP1[i+1].x, 0, zidP1[i+1].z);

                i == tackaP1 -1 ? glVertex3f(xP1, 0, zP1): glVertex3f(zidP1[i+1].x, 0, zidP1[i+1].z);
                i == tackaP1 -1 ? glVertex3f(xP1, 0.5, zP1): glVertex3f(zidP1[i+1].x, 0.5, zidP1[i+1].z);
                glVertex3f(zidP1[i].x, 0.5, zidP1[i].z);

            glEnd();

            /*linije iscrtavamo jer se iz nekih uglova zid ne vidi (kad se gleda pravo odozgo na njega) */
            glBegin(GL_LINES);

                glVertex3f(zidP1[i].x, 0.5, zidP1[i].z);
                /*ako smo dosli do poslednje dodate tacke u niz, nju spajamo sa trenutnom pozicijom igraca*/
                i == tackaP1 -1 ? glVertex3f(xP1, 0.5, zP1): glVertex3f(zidP1[i+1].x, 0.5, zidP1[i+1].z);

            glEnd();
            i++;
        }
    }

    /*igrac 2*/
    if(p==2){
        glColor4f(0.16,0.53,0.78, 0.7);
        int i = 0;
        while(i < tackaP2){
            glBegin(GL_TRIANGLES);
                glVertex3f(zidP2[i].x, 0, zidP2[i].z);
                glVertex3f(zidP2[i].x, 0.5, zidP2[i].z);
                /*ako smo dosli do poslednje tacke dodate u niz, nju spajamo sa trenutnom pozicijom igraca*/
                i == tackaP2 -1 ? glVertex3f(xP2, 0, zP2): glVertex3f(zidP2[i+1].x, 0, zidP2[i+1].z);

                i == tackaP2 -1 ? glVertex3f(xP2, 0, zP2): glVertex3f(zidP2[i+1].x, 0, zidP2[i+1].z);
                i == tackaP2 -1 ? glVertex3f(xP2, 0.5, zP2): glVertex3f(zidP2[i+1].x, 0.5, zidP2[i+1].z);
                glVertex3f(zidP2[i].x, 0.5, zidP2[i].z);

            glEnd();

            /*linije iscrtavamo jer se iz nekih uglova zid ne vidi (kad se gleda pravo odozgo na njega) */
            glBegin(GL_LINES);

                glVertex3f(zidP2[i].x, 0.5, zidP2[i].z);
                /*ako smo dosli do poslednje dodate tacke u niz, nju spajamo sa trenutnom pozicijom igraca*/
                i == tackaP2 -1 ? glVertex3f(xP2, 0.5, zP2): glVertex3f(zidP2[i+1].x, 0.5, zidP2[i+1].z);

            glEnd();

            i++;
        }
    }
}

void displayPlayer(int player){
    /*iscrtavanje poruke da je neki (pokazuje i koji) od igraca pobedio*/
    if(player == 1){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, player1);

        glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);

            glTexCoord2f(0, 0);
            glVertex3f(-50, 0, -90);

            glTexCoord2f(1, 0);
            glVertex3f(-50, 0, 90);

            glTexCoord2f(1, 1);
            glVertex3f(50, 0, 90);

            glTexCoord2f(0, 1);
            glVertex3f(50, 0, -90);
        glEnd();
    }

    if(player == 2){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, player2);

        glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);

            glTexCoord2f(0, 0);
            glVertex3f(-50, 0, -90);

            glTexCoord2f(1, 0);
            glVertex3f(-50, 0, 90);

            glTexCoord2f(1, 1);
            glVertex3f(50, 0, 90);

            glTexCoord2f(0, 1);
            glVertex3f(50, 0, -90);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void loadTextures(){
    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);
    /*podesava se okruzenje za teksture*/
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    /*ucitavamo teksturu za zid*/
    zid = SOIL_load_OGL_texture("./Images/rim_wall.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    /*provera da li je dobro ucitana teksura*/
    if(zid == 0){
        fprintf(stderr, "Zid nije ucitan\n %s \n", SOIL_last_result());
        exit(EXIT_FAILURE);
    }

    /*prva tekstura, za zid, podesavaju se parametri*/
    glBindTexture(GL_TEXTURE_2D, zid);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /*ucitavamo teksturu za pod*/
    pod = SOIL_load_OGL_texture("./Images/floor.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    /*provera da li je dobro ucitana tekstura*/
    if(pod == 0){
        printf("Pod nije ucitan\n %s\n", SOIL_last_result());
        exit(EXIT_FAILURE);
    }

    /*druga tekstura, za pod, podesavaju se paramteri*/
    glBindTexture(GL_TEXTURE_2D, pod);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /*tekstura za obavestenje da je pobednik igrac 1*/
    player1 = SOIL_load_OGL_texture("./Images/p1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    /*provera da li je dobro ucitana tekstura*/
    if(player1 == 0){
        printf("P1 nije ucitan\n %s\n", SOIL_last_result());
        exit(EXIT_FAILURE);
    }

    /*podesavanje parametara*/
    glBindTexture(GL_TEXTURE_2D, player1);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /*ucitavanje teksture za obavestenje da je drugi igrac pobedio*/
    player2 = SOIL_load_OGL_texture("./Images/p2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    /*provera da li je dobro ucitana tekstura*/
    if(player2 == 0){
        printf("P2 nije ucitan\n %s\n", SOIL_last_result());
        exit(EXIT_FAILURE);
    }

    /*podesavanje parametara*/
    glBindTexture(GL_TEXTURE_2D, player2);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);
}

void initMatrix(){
    /*inicijaluzujemo matricu kolizije, da ima jedinice samo na mestima gde je zid arene*/
    for(int i = 0; i < 101; i++){
        for(int j = 0; j < 101; j++){
            if(i == 0 || j == 0 || i == 100 || j == 100)
                collisionMatrix[i][j] = 1;
            else
                collisionMatrix[i][j] = 0;
        }
    }
    /*pocetna polja igraca, takodje oznacavamo jedinicama*/
    collisionMatrix[(int)xP1][(int)zP1] = 1;
    collisionMatrix[(int)xP2][(int)zP2] = 1;
}

void checkCollision(){
    int x1 = (int)xP1, z1 = (int)zP1, x2 = (int)xP2, z2 = (int)zP2;
    /*za svaki pokret igraca, proveravamo da li je polje na koje ide
     * zauzeto, ako jeste proglasavamo kraj i protivnika za pobednika
     * ako nije, onda mu dopustamo da prodje tim poljem*/
    if(collisionMatrix[x1+50][z1+50] == 1){
        end = 1;
        winner = 2;
    } else
        collisionMatrix[x1+50][z1+50] = 1;

    if(collisionMatrix[x2+50][z2+50] == 1){
        end = 1;
        winner = 1;
    } else
        collisionMatrix[x2+50][z2+50] = 1;
}
