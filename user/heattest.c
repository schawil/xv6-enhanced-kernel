#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// ============================================================
// HEAT-AWARE SCHEDULER - MULTI-PROCESS BENCHMARK
// ============================================================
// Tests multiple CPU-bound processes to demonstrate:
// 1. Fair heat distribution (all processes get hot)
// 2. Proportional slowdown (all equally penalized)
// 3. System remains responsive (shell still works)
// ============================================================

#define NUM_WORKERS 4
#define TEST_DURATION 15  // seconds

// Shared memory for results (simulated via file)
struct worker_result {
  int pid;
  uint64 iterations;
  uint64 start_time;
  uint64 end_time;
};

// CPU-intensive workload
uint64
cpu_workload(int duration_ticks)
{
  volatile long x = 0;
  uint64 iterations = 0;
  uint64 start = uptime();
  
  while(uptime() - start < duration_ticks) {
    // CPU-bound computation
    for(int i = 0; i < 100000; i++) {
      x = (x + i * 17) % 1000000;
    }
    iterations++;
  }
  
  return iterations;
}

int
main(int argc, char *argv[])
{
  int duration = TEST_DURATION;
  
  if(argc > 1) {
    duration = atoi(argv[1]);
  }
  
  printf("\n");
  printf("╔════════════════════════════════════════════════════════════╗\n");
  printf("║     HEAT-AWARE SCHEDULER - MULTI-PROCESS                ║\n");
  printf("╚════════════════════════════════════════════════════════════╝\n");
  printf("\n");
  printf("Configuration:\n");
  printf("  • Number of workers: %d\n", NUM_WORKERS);
  printf("  • Test duration: %d seconds\n", duration);
  printf("  • Expected behavior:\n");
  printf("    1. All workers start with equal performance\n");
  printf("    2. After ~1 sec, all reach HOT threshold\n");
  printf("    3. Scheduler applies PROPORTIONAL slowdown to all\n");
  printf("    4. All workers get ~equal iterations (fairness)\n");
  printf("    5. Shell remains responsive (test: type commands)\n");
  printf("\n");
  printf("Starting %d CPU-bound workers...\n", NUM_WORKERS);
  printf("\n");
  
  // Create workers
  int worker_pids[NUM_WORKERS];
  
  for(int i = 0; i < NUM_WORKERS; i++) {
    int pid = fork();
    
    if(pid == 0) {
      // CHILD: Worker process
      
      printf("[Worker %d] Started (PID %d)\n", i, getpid());
      
      uint64 start = uptime();
      uint64 iterations = cpu_workload(duration * 100); // ticks = sec * 100
      uint64 end = uptime();
      
      printf("[Worker %d] Finished | PID=%d | iterations=%d | time=%d ticks\n",
            i, getpid(), iterations, end - start);
      
      // Write result to "file" (simulated via exit code for simplicity)
      // In real impl, use pipe or shared memory
      exit(0);
      
    } else if(pid > 0) {
      // PARENT: Track worker
      worker_pids[i] = pid;
      // (results removed — not used)
      
    } else {
      printf("ERROR: fork() failed for worker %d\n", i);
      exit(1);
    }
  }
  
  // Monitor progress
  printf("\n");
  printf("═══════════════════════════════════════════════════════════\n");
  printf("    MONITORING (check kernel logs for [HEAT] messages)\n");
  printf("═══════════════════════════════════════════════════════════\n");
  printf("\n");
  printf("While workers run, try these commands in another shell:\n");
  printf("  • ls         (should work instantly)\n");
  printf("  • echo test  (should work instantly)\n");
  printf("  • cat README (should work instantly)\n");
  printf("\n");
  printf("This proves the system remains RESPONSIVE despite CPU load.\n");
  printf("\n");
  
  // Wait for all workers
  for(int i = 0; i < NUM_WORKERS; i++) {
    int status;
    wait(&status);
    printf("[Parent] Worker %d (PID %d) exited\n", i, worker_pids[i]);
  }
  
  printf("\n");
  printf("╔════════════════════════════════════════════════════════════╗\n");
  printf("║                    BENCHMARK COMPLETE                      ║\n");
  printf("╚════════════════════════════════════════════════════════════╝\n");
  printf("\n");
  printf("EXPECTED OBSERVATIONS:\n");
  printf("  ✓ All workers completed\n");
  printf("  ✓ Similar iteration counts (±10%%) → FAIRNESS\n");
  printf("  ✓ Kernel logs show [HEAT] SKIPPED for ALL workers\n");
  printf("  ✓ Shell commands executed instantly → RESPONSIVENESS\n");
  printf("\n");
/*   printf("NEXT STEPS FOR REPORT:\n");
  printf("  1. Take screenshot of this output\n");
  printf("  2. Take screenshot of kernel [HEAT] logs\n");
  printf("  3. Take screenshot of shell responsiveness test\n");
  printf("  4. Compare iteration counts → should be similar\n"); */
 
  exit(0);
}