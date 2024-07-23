#include "application.h"
void init_application(application_t *app)
{
    for (int8_t i = 0; i < MAX_TIMERS; i++)
    {
        init_state_machine(&app->state_machines[i]);
    }
    app->active_state_machine_index = 0;
}

