/*
 * buzzer_virt.c
 *
 * Virtual buzzer peripheral for simavr.
 * Monitors PB1 (OC1A) pin transitions to detect buzzer activity.
 * Estimates frequency from the interval between pin toggles.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sim_avr.h"
#include "buzzer_virt.h"

/*
 * Called when PB1 (OC1A) changes state.
 * Each full cycle of the buzzer tone consists of two toggles.
 */
static void
buzzer_pin_hook(struct avr_irq_t *irq, uint32_t value, void *param)
{
    (void)irq;
    (void)value;
    buzzer_virt_t *part = (buzzer_virt_t *)param;
    avr_t *avr = part->avr;

    avr_cycle_count_t now = avr->cycle;
    avr_cycle_count_t delta = now - part->last_toggle_cycle;

    if (delta > 0 && part->last_toggle_cycle > 0) {
        /*
         * Two toggles = one full period.
         * Frequency = avr_freq / (2 * delta_cycles_per_toggle)
         */
        uint32_t freq = avr->frequency / (2 * (uint32_t)delta);
        part->frequency_hz = freq;
        part->active = 1;
    }

    part->last_toggle_cycle = now;
    part->toggle_count++;
}

/*
 * Periodic check to detect when the buzzer stops.
 * If we haven't seen a toggle in a while, mark it as inactive.
 */
static avr_cycle_count_t
buzzer_timeout_check(avr_t *avr, avr_cycle_count_t when, void *param)
{
    buzzer_virt_t *part = (buzzer_virt_t *)param;

    /* If no toggle for more than ~50ms worth of cycles, consider inactive */
    avr_cycle_count_t timeout_cycles = avr->frequency / 20; /* 50ms */
    avr_cycle_count_t since_last = avr->cycle - part->last_toggle_cycle;

    if (since_last > timeout_cycles) {
        part->active = 0;
        part->frequency_hz = 0;
    }

    /* Re-register: check again in ~20ms */
    return when + (avr->frequency / 50);
}

static const char *irq_names[IRQ_BUZZER_COUNT] = {
    [IRQ_BUZZER_PIN_IN] = "<buzzer.pin_in",
};

void
buzzer_virt_init(avr_t *avr, buzzer_virt_t *part)
{
    memset(part, 0, sizeof(*part));
    part->avr = avr;
    part->active = 0;
    part->frequency_hz = 0;
    part->last_toggle_cycle = 0;
    part->toggle_count = 0;

    part->irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_BUZZER_COUNT, irq_names);

    avr_irq_register_notify(part->irq + IRQ_BUZZER_PIN_IN,
                            buzzer_pin_hook, part);

    /* Register periodic timeout check */
    avr_cycle_timer_register(avr, avr->frequency / 50, buzzer_timeout_check, part);
}
