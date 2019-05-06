#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include "image.h"


#define FILENAME0 "./Images/zid.bmp"
#define FILENAME1 "./Images/pod.bmp"
/*postavljamo interval tajmera na 80*/
#define TIMER_INTERVAL 80
/*tajmer id nam je 0*/
#define TIMER_ID 0
/*duzina niza tacaka za iscrtavanje zida*/
#define MAX_LEN 50000

/*niz imena tekstura*/
static GLuint names[2];

/* Fleg koji odredjuje stanje tajmera. */
static int animation_active;

/* Deklaracije callback funkcija. */
static void on_keyboardSpecial(int key, int x, int y);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);
void initialize();
/*deklaracija funkcija koje ce pozivati neke od kolbek f-ja*/
void display();
void drawWall(int);


/*struktura za x i z koordinate zida*/
typedef struct {
    float x;
    float z;
} Zid;

/*dimenzije prozora*/
int widthP, heightP;

/*stepen rotacije, x koordinata, z koordinata za igraca 1, zatim za igraca 2*/
float rotateP1, xP1, zP1, rotateP2, xP2, zP2;

/*niz za  koordinate zida igraca 1, zatim igraca 2*/
Zid zidP1[MAX_LEN], zidP2[MAX_LEN];
/*poslednja dodata tacka u niz za zid za igraca 1, zatim igraca 2*/
int tackaP1, tackaP2;


int main(int argc, char **argv)
{/* Inicijalizuje se GLUT. */
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
    printf("%d %d\n", x, y);
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
            /*space -> pauza/start*/
            animation_active = (animation_active+1)%2;
            if(animation_active)
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
        default:
            break;
    }
}

static void on_keyboardSpecial(int key, int x, int y) {
    /*ispis kako kompajler ne bi prijavljivao warning-e*/
    printf("%d %d\n", x, y);
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

        /*nove koordinate igraca, u odnosu na to da li je skrenuo ili ne*/

        xP1 += sin(M_PI/2 * rotateP1/90);
        zP1 += cos(M_PI/2 * rotateP1/90);

        xP2 += sin(M_PI/2 * rotateP2/90);
        zP2 -= cos(M_PI/2 * rotateP2/90);

        /* Forsira se ponovno iscrtavanje prozora. */
        glutPostRedisplay();
    }


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
    /* Objekat koji predstavlja teskturu ucitanu iz fajla. */
    Image * image;

    /*postavljaju se inicijalne vrednosti na promenljive, animacija nije aktivna i pocetne pozicije igraca koji nisu rotirani*/

    animation_active = 0;
    rotateP1 = 0;
    xP1 = 0;
    zP1 = -45;
    xP2 = 0;
    zP2 = 45;
    rotateP2 = 0;

    /*pocetne tacke se dodaju u niz za zid, da bi znali odakle pocinje iscrtavanje zida*/

    zidP1[tackaP1].x = xP1;
    zidP1[tackaP1].z = zP1-1;

    zidP2[tackaP2].x = xP2;
    zidP2[tackaP2].z = zP2+1;

    /*pomeramo se za jedno mesto u zidu*/
    tackaP1++;
    tackaP2++;


    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);

    /* Kreira se prva tekstura. */
    image_read(image, FILENAME0);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(2, names);

    /*prva tekstura, za zid*/
    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* druga tekstura, za pod*/
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);

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
    gluLookAt(0, 3, 50, 0, 0, 0, 0, 1, 0);
    display();

    /*postavljamo poged za igraca 1, donji desni ugao ekrana i iscrtavamo objekte*/
    glViewport(widthP/2, 0, widthP/2, heightP/2);
    glLoadIdentity();
    gluLookAt(-2, 3, -50, 0, 0, 0, 0, 1, 0);
    display();
    
    /* Postavlja se nova slika u prozor. */
    glutSwapBuffers();
}

void display(){

    /*iscrtavamo zid po zid, koristeci teksuru za zid*/
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, names[0]);

    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(50, -10, -50);

        glTexCoord2f(1, 0);
        glVertex3f(50, -10, 50);

        glTexCoord2f(1, 1);
        glVertex3f(50, 100, 50);

        glTexCoord2f(0, 1);
        glVertex3f(50, 100, -50);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(50, -10, 50);

        glTexCoord2f(1, 0);
        glVertex3f(-50, -10, 50);

        glTexCoord2f(1, 1);
        glVertex3f(-50, 100, 50);

        glTexCoord2f(0, 1);
        glVertex3f(50, 100, 50);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(-50, -10, 50);

        glTexCoord2f(1, 0);
        glVertex3f(-50, -10, -50);

        glTexCoord2f(1, 1);
        glVertex3f(-50, 100, -50);

        glTexCoord2f(0, 1);
        glVertex3f(-50, 100, 50);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);

        glTexCoord2f(0, 0);
        glVertex3f(-50, -10, -50);

        glTexCoord2f(1, 0);
        glVertex3f(50, -10, -50);

        glTexCoord2f(1, 1);
        glVertex3f(50, 100, -50);

        glTexCoord2f(0, 1);
        glVertex3f(-50, 100, -50);
    glEnd();


    /*iscrtavamo pod, koristeci teksturu za pod*/
    glBindTexture(GL_TEXTURE_2D, names[1]);
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

    glPushMatrix();
        glColor3f(0.65,0.96,0.20);
        glTranslatef(xP2, 0.5, zP2);
        glRotatef(rotateP2, 0, 1, 0);
        glScalef(0.5,0.7,2);
        glutSolidCube(1);
    glPopMatrix();
    drawWall(2);

    glPushMatrix();
        glColor3f(0.93, 0.16, 0.22);
        glTranslatef(xP1, 0.5, zP1);
        glRotatef(rotateP1, 0, 1, 0);
        glScalef(0.5,0.7,2);
        glutSolidCube(1);
    glPopMatrix();
    drawWall(1);
}

void drawWall(int p){
    /*iscrtavamo zidove (tragove) igraca, za to ukljucujemo BLENG i ALPHA */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*igrac 1*/
    if(p==1){
        glColor4f(0.93, 0.16, 0.22, 0.7);
        int i = 0;
        glBegin(GL_TRIANGLES);
                while(i < tackaP1){
                    glVertex3f(zidP1[i].x, 0, zidP1[i].z);
                    glVertex3f(zidP1[i].x, 0.7, zidP1[i].z);
                    /*ako smo dosli do poslednje dodate tacke u niz, nju spajamo sa trenutnom pozicijom igraca*/
                    i == tackaP1 -1 ? glVertex3f(xP1, 0, zP1): glVertex3f(zidP1[i+1].x, 0, zidP1[i+1].z);


                    i == tackaP1 -1 ? glVertex3f(xP1, 0, zP1): glVertex3f(zidP1[i+1].x, 0, zidP1[i+1].z);
                    i == tackaP1 -1 ? glVertex3f(xP1, 0.7, zP1): glVertex3f(zidP1[i+1].x, 0.7, zidP1[i+1].z);
                    glVertex3f(zidP1[i].x, 0.7, zidP1[i].z);

                    i++;
                    }
        glEnd();
    }

    /*igrac 2*/
    if(p==2){
        glColor4f(0.65,0.96,0.20, 0.7);

        int i = 0;
        glBegin(GL_TRIANGLES);
        while(i < tackaP2){
            glVertex3f(zidP2[i].x, 0, zidP2[i].z);
            glVertex3f(zidP2[i].x, 0.7, zidP2[i].z);
            /*ako smo dosli do poslednje tacke dodate u niz, nju spajamo sa trenutnom pozicijom igraca*/
            i == tackaP2 -1 ? glVertex3f(xP2, 0, zP2): glVertex3f(zidP2[i+1].x, 0, zidP2[i+1].z);

            i == tackaP2 -1 ? glVertex3f(xP2, 0, zP2): glVertex3f(zidP2[i+1].x, 0, zidP2[i+1].z);
            i == tackaP2 -1 ? glVertex3f(xP2, 0.7, zP2): glVertex3f(zidP2[i+1].x, 0.7, zidP2[i+1].z);
            glVertex3f(zidP2[i].x, 0.7, zidP2[i].z);

            i++;
        }
        glEnd();
    }
}