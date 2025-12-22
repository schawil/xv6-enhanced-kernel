#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// ============================================================
// COMPARATIVE BENCHMARK
// Measures CPU-bound performance to quantify heat penalty
// ============================================================

#define ITERATIONS 1000

uint64
benchmark(void)
{
    volatile long x = 0;
    uint64 start = uptime();

    for (int iter = 0; iter < ITERATIONS; iter++)
    {
        for (int i = 0; i < 100000; i++)
        {
            x = (x + i * 17) % 1000000;
        }
    }

    uint64 end = uptime();
    return end - start; // Return elapsed ticks
}

int main(int argc, char *argv[])
{
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("        HEAT-AWARE SCHEDULER - PERFORMANCE BENCHMARK\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Running CPU-bound benchmark...\n");
    printf("Iterations: %d\n", ITERATIONS);
    printf("\n");

    printf("[Phase 1] SHORT burst (stays COLD)\n");
    uint64 cold_time = benchmark();
    printf("  Time: %d ticks | Expected: process stays COLD, no penalty\n", cold_time);

    sleep(200); // Cool down

    printf("\n");
    printf("[Phase 2] LONG burst (gets HOT)\n");

    // Run for longer to trigger heat
    uint64 hot_start = uptime();
    for (int round = 0; round < 5; round++)
    {
        benchmark();
    }
    uint64 hot_time = uptime() - hot_start;

    printf("  Time: %d ticks | Expected: process gets HOT, penalty applied\n", hot_time);

    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("RESULTS:\n");
    printf("  Cold (1x benchmark): %d ticks\n", cold_time);
    printf("  Hot  (5x benchmark): %d ticks\n", hot_time);
    printf("  Expected ratio: ~1:5 without heat-aware\n");
    printf("  Actual ratio: 1:%.1f\n", (float)hot_time / cold_time);
    printf("\n");

    if (hot_time > cold_time * 6)
    {
        printf("✓ HEAT PENALTY DETECTED (ratio > 1:6)\n");
        printf("  The scheduler successfully throttled the hot process.\n");
    }
    else
    {
        printf("⚠ No significant heat penalty (ratio < 1:6)\n");
        printf("  Either: heat threshold not reached, or scheduler unchanged.\n");
    }

    printf("\n");

    exit(0);
}