// kernel/activity.h
#ifndef ACTIVITY_H
#define ACTIVITY_H

struct activity {
    int cpu_percent;       // Charge CPU (0-100)
    int mem_percent;       // RAM utilisée (0-100)
    int user_activity;     // 1 = Clavier actif récemment, 0 = Inactif
    int interrupts;        // Nb total d'interruptions
    int state;             // 0 = LIBRE, 1 = OCCUPE
    char timestamp[32];    // Horodatage "YYYY-MM-DD HH:MM:SS"
};

// Seuil d'activité pour le clavier (en ticks). 10 secondes approx.
#define ACTIVITY_TIMEOUT_TICKS 10000 

#endif