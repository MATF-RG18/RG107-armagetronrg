#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "image.h"


#define FILENAME0 "./Images/zid.bmp"
#define FILENAME1 "./Images/pod.bmp"

static GLuint names[2];

/* Vreme proteklo od pocetka simulacije. */
static float animation_parameter;

/* Fleg koji odredjuje stanje tajmera. */
static int animation_active;

/* Deklaracije callback funkcija. */
static void on_keyboardSpecial(int key, int x, int y);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void on_display(void);
void initialize();
void display();


int widthP, heightP;

int main(int argc, char **argv)
{/* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(1500, 1500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se funkcije za obradu dogadjaja. */
    glutSpecialFunc(on_keyboardSpecial);
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    /* Inicijalizuju se globalne promenljive. */
    animation_parameter = 0;
    animation_active = 0;

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    
    glLineWidth(2);

    initialize();

    /* Ulazi se u glavnu petlju. */
    glutMainLoop();

    return 0;
}


static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        /* Zavrsava se program. */
        exit(0);
        break;

    }
}

static void on_keyboardSpecial(int key, int x, int y)
{
    switch (key) {
    case 27:
        /* Zavrsava se program. */
        exit(0);
        break;

    case GLUT_KEY_UP:
        /* Pokrece se simulacija. */
        if (!animation_active) {
            glutTimerFunc(10, on_timer, 0);
            animation_active = 1;
        }
        break;

    case GLUT_KEY_DOWN:
        /* Zaustavlja se simulacija. */
        animation_active = 0;
        break;
    }
}

static void on_timer(int value)
{
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
    if (value != 0)
        return;

    /* Azurira se vreme simulacije. */
    animation_parameter++;

    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

    /* Po potrebi se ponovo postavlja tajmer. */
    if (animation_active)
        glutTimerFunc(10, on_timer, 0);
}

static void on_reshape(int width, int height)
{
    
    widthP = width;
    heightP = height;
    /* Postavlja se viewport. */
    
    /* Postavljaju se parametri projekcije. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 1500);
}

void initialize(){
    /* Objekat koji predstavlja teskturu ucitanu iz fajla. */
    Image * image;

    /* Postavlja se boja pozadine. */
    glClearColor(0, 0, 0, 0);


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

    /* Kreira se druga tekstura. */
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

    /* Inicijalizujemo matricu rotacije. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}




static void on_display(void) {
    glutFullScreen();
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    glMatrixMode(GL_MODELVIEW);

    glViewport(0, heightP/2, widthP/2, heightP/2);
    glLoadIdentity();
    gluLookAt(0, 3, 50, 0, 0, 0, 0, 1,0);
    display();

    glViewport(widthP/2, 0, widthP/2, heightP/2);
    glLoadIdentity();
    gluLookAt(0, 3, -50, 0, 0, 0, 0, 1, 0);
    display();
    
    /* Postavlja se nova slika u prozor. */
    glutSwapBuffers();
}

void display(){
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

}
