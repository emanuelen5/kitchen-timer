/*
 * buzzer_virt.h
 *
 * Virtual buzzer peripheral for simavr.
 * Monitors PB1 (OC1A) pin activity to detect if the toneAC buzzer
 * is producing sound. Can infer approximate frequency from the
 * Timer1 ICR1 register.
 */

#ifndef __BUZZER_VIRT_H__
#define __BUZZER_VIRT_H__

#include "sim_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    IRQ_BUZZER_PIN_IN = 0,
    IRQ_BUZZER_COUNT
};

typedef struct buzzer_virt_t {
    avr_irq_t *irq;
    struct avr_t *avr;

    uint8_t active;             /* 1 if buzzer is currently producing sound */
    avr_cycle_count_t last_toggle_cycle; /* cycle count of last pin toggle */
    uint32_t toggle_count;      /* number of pin toggles seen recently */
    uint32_t frequency_hz;      /* estimated frequency in Hz */
} buzzer_virt_t;

void buzzer_virt_init(struct avr_t *avr, buzzer_virt_t *part);

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_VIRT_H__ */
