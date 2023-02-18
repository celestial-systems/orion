#include "ultra_protocol.h"
#include "utils.h"

void ori_main(struct ultra_boot_context *boot_ctx, uint32_t magic) {
  while (magic != ULTRA_MAGIC)
    hang();

  while (1)
    asm volatile("hlt");
}