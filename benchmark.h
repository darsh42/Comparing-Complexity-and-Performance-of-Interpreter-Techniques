#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

// #define __PROFILE__

// In your header file
#define HOOK __attribute__((noinline)) __attribute__((visibility("default"))) void

#ifdef __PROFILE__

extern HOOK ENTER_PROCESSING(void);
extern HOOK EXIT_PROCESSING(void);
extern HOOK ENTER_DISPATCHING(void);
extern HOOK EXIT_DISPATCHING(void);

#define PROFILE_ENTER_INSTRUCTION ENTER_PROCESSING();
#define PROFILE_EXIT_INSTRUCTION  EXIT_PROCESSING();
#define PROFILE_ENTER_INTERPRETER ENTER_DISPATCHING();
#define PROFILE_EXIT_INTERPRETER  EXIT_DISPATCHING();
#endif // __PROFILE__
#endif // __BENCHMARK_H__
