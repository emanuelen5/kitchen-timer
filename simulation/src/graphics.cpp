#include "graphics.h"
#include "simulation.h"

#include "stdio.h"
#include <GL/glut.h>
#include "pthread.h"
#include <math.h>
#include <time.h>

extern "C"
{
#include "sim_avr.h"
#include "rotenc.h"
}

#define UNUSED (void)

/* ─── Command queue (GLUT thread → AVR thread) ──────────────────── *
 * Keyboard callbacks on the GLUT thread push commands into a simple
 * ring-buffer.  The AVR run thread polls it each iteration and drives
 * the rotary encoder / button from its own context, which is required
 * because simavr's cycle-timers and IRQs are not thread-safe.
 * ─────────────────────────────────────────────────────────────────── */

enum cmd_type {
    CMD_ROTENC_CW   = 1,
    CMD_ROTENC_CCW  = 2,
    CMD_BTN_PRESS   = 3,
    CMD_BTN_RELEASE = 4,
};

struct input_cmd {
    enum cmd_type type;
    int           count;   /* number of clicks for rotary */
};

#define CMD_RING_SIZE 64
static struct input_cmd cmd_ring[CMD_RING_SIZE];
static volatile int     cmd_head = 0;   /* written by GLUT thread */
static volatile int     cmd_tail = 0;   /* read by AVR thread */

static void cmd_push(enum cmd_type t, int count)
{
    int next = (cmd_head + 1) % CMD_RING_SIZE;
    if (next == cmd_tail) return;        /* full – drop */
    cmd_ring[cmd_head].type  = t;
    cmd_ring[cmd_head].count = count;
    cmd_head = next;                     /* publish */
}

static bool cmd_pop(struct input_cmd *out)
{
    if (cmd_tail == cmd_head) return false;
    *out = cmd_ring[cmd_tail];
    cmd_tail = (cmd_tail + 1) % CMD_RING_SIZE;
    return true;
}

/* ─── Real-time throttle helper ──────────────────────────────────── */
static uint64_t get_wall_usec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
}

static uint64_t get_sim_usec(avr_t *avr)
{
    /* avr->cycle is the total number of clock cycles executed */
    return (uint64_t)avr->cycle * 1000000ULL / avr->frequency;
}

/* ─── Layout constants ───────────────────────────────────────────── */
static const int MATRIX_SIZE     = 16;      /* 16x16 LED matrix */
static const float PIXEL_SIZE    = 28.0f;   /* size of each LED pixel */
static const float PIXEL_GAP     = 2.0f;    /* gap between pixels */
static const float GRID_SIZE     = 30.0f;   /* PIXEL_SIZE + PIXEL_GAP */
static const float MARGIN        = 20.0f;   /* margin around the matrix */

/* Status panel below the matrix */
static const float STATUS_HEIGHT = 60.0f;

static const float WINDOW_W = MARGIN * 2 + MATRIX_SIZE * GRID_SIZE;
static const float WINDOW_H = MARGIN * 2 + MATRIX_SIZE * GRID_SIZE + STATUS_HEIGHT;

/* ─── Global state for GLUT callbacks ────────────────────────────── */
static avr_t *g_avr = nullptr;
static peripherals_t *g_peripherals = nullptr;

const char escape_key = 0x1b;

/* ─── LED colour helper ──────────────────────────────────────────── */
static void set_led_color(bool is_on)
{
    if (is_on) {
        glColor3f(0.0f, 1.0f, 0.0f);  /* bright green */
    } else {
        glColor3f(0.15f, 0.15f, 0.15f); /* dark grey (off LED) */
    }
}

/* ─── Display callback ───────────────────────────────────────────── */
void display_callback(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Draw the LED matrix background */
    glColor3f(0.02f, 0.02f, 0.02f);
    glBegin(GL_QUADS);
    glVertex2f(MARGIN - 5, MARGIN - 5);
    glVertex2f(MARGIN + MATRIX_SIZE * GRID_SIZE + 5, MARGIN - 5);
    glVertex2f(MARGIN + MATRIX_SIZE * GRID_SIZE + 5, MARGIN + MATRIX_SIZE * GRID_SIZE + 5);
    glVertex2f(MARGIN - 5, MARGIN + MATRIX_SIZE * GRID_SIZE + 5);
    glEnd();

    /* Draw pixels */
    for (int y = 0; y < MATRIX_SIZE; y++) {
        for (int x = 0; x < MATRIX_SIZE; x++) {
            bool is_on = false;
            if (g_peripherals) {
                is_on = g_peripherals->display.framebuffer[y][x] != 0;
            }
            set_led_color(is_on);

            float px = MARGIN + x * GRID_SIZE;
            float py = MARGIN + y * GRID_SIZE;

            /* Draw rounded-ish LED (just a filled square for simplicity) */
            glBegin(GL_QUADS);
            glVertex2f(px, py);
            glVertex2f(px + PIXEL_SIZE, py);
            glVertex2f(px + PIXEL_SIZE, py + PIXEL_SIZE);
            glVertex2f(px, py + PIXEL_SIZE);
            glEnd();
        }
    }

    /* Draw status panel */
    float status_y = MARGIN + MATRIX_SIZE * GRID_SIZE + 15;

    /* Buzzer indicator */
    if (g_peripherals && g_peripherals->buzzer.active) {
        /* Draw speaker icon (filled circle) */
        glColor3f(1.0f, 0.6f, 0.0f); /* orange when active */
    } else {
        glColor3f(0.2f, 0.2f, 0.2f); /* grey when silent */
    }
    float cx = MARGIN + 15;
    float cy = status_y + 15;
    float r = 10.0f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * M_PI * i / 20.0f;
        glVertex2f(cx + r * cosf(angle), cy + r * sinf(angle));
    }
    glEnd();

    /* Buzzer frequency text area indicator (simple bar) */
    if (g_peripherals && g_peripherals->buzzer.active) {
        float freq_bar_w = (float)g_peripherals->buzzer.frequency_hz / 50.0f;
        if (freq_bar_w > 200.0f) freq_bar_w = 200.0f;
        glColor3f(1.0f, 0.6f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(cx + 20, cy - 3);
        glVertex2f(cx + 20 + freq_bar_w, cy - 3);
        glVertex2f(cx + 20 + freq_bar_w, cy + 3);
        glVertex2f(cx + 20, cy + 3);
        glEnd();
    }

    /* Draw key legend */
    /* We can't easily render text with basic GLUT without bitmap fonts,
     * but we can draw simple indicators. The key legend will be printed
     * to the terminal instead. */

    glutSwapBuffers();
}

/* ─── Keyboard callbacks ─────────────────────────────────────────── */
void key_down_callback(unsigned char key, int x, int y)
{
    UNUSED x;
    UNUSED y;

    switch (key)
    {
    case 'q':
    case escape_key:
        exit(0);
        break;

    /* Rotary encoder CW */
    case 'l':
    case 'd':
        cmd_push(CMD_ROTENC_CW, 1);
        break;

    /* Rotary encoder CCW */
    case 'j':
    case 'a':
        cmd_push(CMD_ROTENC_CCW, 1);
        break;

    /* Fast CW: 5 clicks */
    case 'L':
    case 'D':
        cmd_push(CMD_ROTENC_CW, 5);
        break;

    /* Fast CCW: 5 clicks */
    case 'J':
    case 'A':
        cmd_push(CMD_ROTENC_CCW, 5);
        break;

    /* Button press */
    case 'k':
    case 's':
    case ' ':
        cmd_push(CMD_BTN_PRESS, 0);
        break;

    default:
        break;
    }
}

void key_up_callback(unsigned char key, int x, int y)
{
    UNUSED x;
    UNUSED y;

    switch (key)
    {
    case 'k':
    case 's':
    case ' ':
        cmd_push(CMD_BTN_RELEASE, 0);
        break;

    default:
        break;
    }
}

void special_key_down_callback(int key, int x, int y)
{
    UNUSED x;
    UNUSED y;

    switch (key)
    {
    case GLUT_KEY_RIGHT:
        cmd_push(CMD_ROTENC_CW, 1);
        break;

    case GLUT_KEY_LEFT:
        cmd_push(CMD_ROTENC_CCW, 1);
        break;

    case GLUT_KEY_DOWN:
        cmd_push(CMD_BTN_PRESS, 0);
        break;

    default:
        break;
    }
}

void special_key_up_callback(int key, int x, int y)
{
    UNUSED x;
    UNUSED y;

    switch (key)
    {
    case GLUT_KEY_DOWN:
        cmd_push(CMD_BTN_RELEASE, 0);
        break;

    default:
        break;
    }
}

/* ─── Timer callback (refresh display) ───────────────────────────── */
void timer_callback(int i)
{
    UNUSED i;
    glutTimerFunc(1000 / 30, timer_callback, 0); /* 30 FPS */

    /* Always redisplay to keep the UI responsive */
    glutPostRedisplay();
}

/* ─── Quadrature phase table (same as rotenc.c) ─────────────────── */
static const rotenc_pins_t quad_table[ROTENC_STATE_COUNT] = {
    {0, 0}, {1, 0}, {1, 1}, {0, 1}
};

/* Drive one quadrature phase transition, then run AVR for enough
 * cycles to let the pin-change ISR execute before the next phase. */
static void
drive_one_phase(avr_t *avr, rotenc_t *rotenc, int dir)
{
    if (dir > 0) {
        if (++rotenc->phase >= ROTENC_STATE_COUNT)
            rotenc->phase = 0;
    } else {
        if (--rotenc->phase < 0)
            rotenc->phase = ROTENC_STATE_COUNT - 1;
    }
    avr_raise_irq(rotenc->irq + IRQ_ROTENC_OUT_A_PIN,
                   quad_table[rotenc->phase].a_pin);
    avr_raise_irq(rotenc->irq + IRQ_ROTENC_OUT_B_PIN,
                   quad_table[rotenc->phase].b_pin);
    /* Run ~200 instructions so the ISR can fire and complete */
    for (int i = 0; i < 200; i++)
        avr_run(avr);
}

/* Process all queued input commands from the AVR thread context. */
static void
process_commands(avr_t *avr, rotenc_t *rotenc)
{
    struct input_cmd cmd;
    while (cmd_pop(&cmd)) {
        switch (cmd.type) {
        case CMD_ROTENC_CW:
            for (int c = 0; c < cmd.count; c++)
                for (int p = 0; p < 4; p++)
                    drive_one_phase(avr, rotenc, +1);
            break;
        case CMD_ROTENC_CCW:
            for (int c = 0; c < cmd.count; c++)
                for (int p = 0; p < 4; p++)
                    drive_one_phase(avr, rotenc, -1);
            break;
        case CMD_BTN_PRESS:
            avr_raise_irq(
                rotenc->irq + IRQ_ROTENC_OUT_BUTTON_PIN, 0);
            break;
        case CMD_BTN_RELEASE:
            avr_raise_irq(
                rotenc->irq + IRQ_ROTENC_OUT_BUTTON_PIN, 1);
            break;
        }
    }
}

/* ─── AVR run thread (real-time throttled) ───────────────────────── */
static void *
avr_run_thread(void *avr_ptr)
{
    avr_t *avr = (avr_t *)avr_ptr;

    uint64_t wall_start = get_wall_usec();
    uint64_t sim_start  = get_sim_usec(avr);

    while (true)
    {
        const char *reason = exit_reason(avr);

        if (reason)
        {
            printf("Exiting: %s\n", reason);
            break;
        }

        /* Process any queued input commands from the GLUT thread */
        process_commands(avr, &g_peripherals->rotary_encoder);

        avr_run(avr);

        /* Throttle: if simulated time has run ahead of wall-clock time, sleep */
        uint64_t sim_elapsed  = get_sim_usec(avr) - sim_start;
        uint64_t wall_elapsed = get_wall_usec() - wall_start;

        if (sim_elapsed > wall_elapsed + 500) {
            /* We're ahead by more than 0.5ms — sleep to catch up */
            uint64_t sleep_us = sim_elapsed - wall_elapsed;
            if (sleep_us > 10000) sleep_us = 10000; /* cap at 10ms to stay responsive */
            struct timespec ts = {
                .tv_sec  = 0,
                .tv_nsec = (long)(sleep_us * 1000)
            };
            nanosleep(&ts, nullptr);
        }
    }
    avr_terminate(avr);
    return NULL;
}

/* ─── Entry point ────────────────────────────────────────────────── */
void create_and_set_up_glut_window(void)
{
    int argc = 0;
    glutInit(&argc, nullptr);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize((int)WINDOW_W, (int)WINDOW_H);
    glutCreateWindow("Kitchen Timer Simulator");

    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_W, WINDOW_H, 0, -1, 1); /* top-left origin */

    glutDisplayFunc(display_callback);
    glutKeyboardFunc(key_down_callback);
    glutKeyboardUpFunc(key_up_callback);
    glutSpecialFunc(special_key_down_callback);
    glutSpecialUpFunc(special_key_up_callback);
    glutTimerFunc(1000 / 30, timer_callback, 0);

    /* Disable key repeat so we get clean press/release events */
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
}

void simulate_with_graphics(avr_t *avr, peripherals_t *peripherals)
{
    g_avr = avr;
    g_peripherals = peripherals;

    create_and_set_up_glut_window();

    printf("\n");
    printf("┌──────────────────────────────────────────────┐\n");
    printf("│  Kitchen Timer Simulator                     │\n");
    printf("├──────────────────────────────────────────────┤\n");
    printf("│  Controls:                                   │\n");
    printf("│    j / a / Left arrow  : rotate CCW          │\n");
    printf("│    l / d / Right arrow : rotate CW           │\n");
    printf("│    J / A               : fast rotate CCW     │\n");
    printf("│    L / D               : fast rotate CW      │\n");
    printf("│    k / s / Space / Down: button press        │\n");
    printf("│    (hold for long press, double-tap for      │\n");
    printf("│     double press)                            │\n");
    printf("│    q / Escape          : quit                │\n");
    printf("└──────────────────────────────────────────────┘\n");
    printf("\n");

    /* Graphics needs to be on the main thread, so we put the AVR in a
     * separate one */
    pthread_t run;
    pthread_create(&run, NULL, avr_run_thread, (void *)avr);

    glutMainLoop();

    pthread_join(run, NULL);
}
