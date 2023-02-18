#define hang()                                                                 \
  do {                                                                         \
    asm volatile("cli");                                                       \
    while (1)                                                                  \
      asm volatile("hlt");                                                     \
  } while (0)

#define ORI_PACKED __attribute__((packed))