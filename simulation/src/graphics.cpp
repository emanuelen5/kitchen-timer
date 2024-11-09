#include "graphics.h"
#include "simulation.h"

#include "stdio.h"
#include <GL/glut.h>
#include "pthread.h"

extern "C"
{
#include "sim_avr.h"
}

#define UNUSED (void)
volatile unsigned char key_g;
const char escape_key = 0x1f;
const float pixel_size = 64;

void display_callback(void) /* function called whenever redisplay needed */
{
    // OpenGL rendering goes here...
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up modelview matrix
    glMatrixMode(GL_MODELVIEW); // Select modelview matrix
    glLoadIdentity();           // Start with an identity matrix

    const float pixel_size = 64;
    float grid = pixel_size;
    float size = grid * 0.8;
    glBegin(GL_QUADS);
    glColor3f(1, 0, 0);

    for (int di = 0; di < 3; di++)
    {
        const char on = (port_c_state & (1 << di)) == 0;
        if (on)
        {
            float x = (di)*grid;
            float y = 0;
            glVertex2f(x + size, y + size);
            glVertex2f(x, y + size);
            glVertex2f(x, y);
            glVertex2f(x + size, y);
        }
    }

    glEnd();
    glutSwapBuffers();
}

void key_callback(unsigned char key, int x, int y)
{
    UNUSED x;
    UNUSED y;

    switch (key)
    {
    case 'q':
    case escape_key:
        exit(0);
        break;
    default:
        key_g = key;
        break;
    }
}
void timer_callback(int i)
{
    UNUSED i;
    static uint8_t old_port_state = 0b00000000;

    glutTimerFunc(1000 / 64, timer_callback, 0);

    const bool port_changed = old_port_state != port_c_state;
    old_port_state = port_c_state;
    if (port_changed)
        glutPostRedisplay();
}

static void *
avr_run_thread(void *avr_ptr)
{
    avr_t *avr = (avr_t *)avr_ptr;

    while (true)
    {
        const char *reason = (key_g == escape_key) ? "Escape key pressed" : exit_reason(avr);

        if (reason)
        {
            printf("Exiting: %s\n", exit_reason(avr));
            break;
        }

        if (key_g != 0)
        {
            printf("key pressed: %c\n", key_g);
            key_g = 0;
        }
        avr_run(avr);
    }
    avr_terminate(avr);
    return NULL;
}

void create_and_set_up_glut_window(void)
{
    printf("Demo launching: 'LED' bar is PORTC\n"
           "   Press 'q' to quit\n");

    int argc = 0;
    glutInit(&argc, {});

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(8 * pixel_size, 1 * pixel_size); /* width=400pixels height=500pixels */
    glutCreateWindow("LEDs");

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION); // Select projection matrix
    glLoadIdentity();            // Start with an identity matrix
    glOrtho(0, 8 * pixel_size, 0, 1 * pixel_size, 0, 10);
    glScalef(1, -1, 1);
    glTranslatef(0, -1 * pixel_size, 0);

    glutDisplayFunc(display_callback);
    glutKeyboardFunc(key_callback);
    glutTimerFunc(1000 / 24, timer_callback, 0);
}

void simulate_with_graphics(avr_t *avr)
{
    create_and_set_up_glut_window();

    // the AVR run on it's own thread. it even allows for debugging!
    pthread_t run;
    pthread_create(&run, NULL, avr_run_thread, (void *)avr);

    glutMainLoop();

    pthread_join(run, NULL);
}