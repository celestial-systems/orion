#include "common/ultra_protocol.h"
#include "sys/x86_64/utils.h"

void
main(struct ultra_boot_context* boot_ctx, uint32_t magic)
{
  while (magic != ULTRA_MAGIC)
    hang();

  while (1)
    asm volatile("hlt");
}