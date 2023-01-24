#define hang() do { asm volatile("cli"); while(1) asm volatile("hlt"); } while(0)

#define _PACKED_ __attribute__((packed))