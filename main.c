#include <stdlib.h>
#include <GL/glut.h>

/* Dimenzije prozora */
static int window_width, window_height;

/* Deklaracije callback funkcija. */
static void on_reshape(int width, int height);
static void on_display(void);

int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(1600, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se callback funkcije. */
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1);

    /* Program ulazi u glavnu petlju. */
    glutMainLoop();

    return 0;
}


static void on_reshape(int width, int height)
{
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
}

static void on_display(void)
{
    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se viewport. */
    glViewport(0, 0, window_width, window_height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-10, 10, -10, 10, 1, 15);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 15, 5, 0, 0, 0, 0, 1, 0);

   
        /*X*/
        
    glColor3f(1,0,0);
        glBegin(GL_LINES);
            glVertex3f(0, 0, 0);
            glVertex3f(100, 0, 0);
        glEnd();
        /*Y*/
        
    glColor3f(0,1,0);
        glBegin(GL_LINES);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 100, 0);
        glEnd();
        
        /*Z*/
        
    glColor3f(0,0,1);
        glBegin(GL_LINES);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 100);
        glEnd();
   
   
   
    int i = 0;
    glColor3f(1,1,1);
    
    for(; i < 200; i++){
        glBegin(GL_LINES);
            glVertex3f(100, 0, 100-i);
            glVertex3f(-100, 0, 100-i);
        glEnd();
    }
    i=0;
    for(; i < 200; i++){
        glBegin(GL_LINES);
            glVertex3f(100-i, 0, 100);
            glVertex3f(100-i, 0, -100);
        glEnd();
    }
    
    
    glutSwapBuffers();
}
