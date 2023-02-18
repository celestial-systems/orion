#include <assert.h>
#include <malloc.h>
#include <string.h>

/* Directories */
#define SRC "src"
#define BIN "bin"
#define OUT "orion-x86_64.x"
#define LDARGS "-Tsrc/linker.ld", "-static"

#define CARGS                                                                  \
  "-Wall", "-Wextra", "-Wno-unused", "-Wno-unused-parameter",                  \
      "-ffreestanding", "-fno-stack-protector", "-fno-pic", "-mno-80387",      \
      "-mno-mmx", "-mno-3dnow", "-mno-sse", "-mno-sse2", "-mno-red-zone",      \
      "-mcmodel=kernel"

#define CBUILDER_IMPLEMENTATION
#include "./build/chol/cbuilder.h"

char *cc = CC;

void build_kernel(const char *cc, const char **srcs, size_t srcs_count,
                  const char *bin, const char *out) {
  if (!fs_exists(bin))
    fs_create_dir(bin);

  char *o_files[128];
  size_t o_files_count = 0;

  build_cache_t c;
  if (build_cache_load(&c) != 0)
    LOG_FATAL("Build cache is corrupted");

  bool rebuild_all = false;

  /* Compile files in all source directories */
  for (size_t i = 0; i < srcs_count; ++i) {
    int status;

    /* If a header file was modified, everything needs to be rebuilt. Check for
     * that here */
    FOREACH_IN_DIR(
        srcs[i], dir, ent,
        {
          if (strcmp(fs_ext(ent.name), "h") != 0)
            continue;

          char *src = FS_JOIN_PATH(srcs[i], ent.name);
          if (src == NULL)
            FATAL_FUNC_FAIL("malloc");

          int64_t m_cached = build_cache_get(&c, src);
          int64_t m_now;
          if (fs_time(src, &m_now, NULL) != 0)
            LOG_FATAL("Could not get last modified time of '%s'", src);

          if (m_cached != m_now) {
            if (!rebuild_all)
              rebuild_all = true;

            build_cache_set(&c, src, m_now);
          }

          free(src);
        },
        status);

    if (status != 0)
      LOG_FATAL("Failed to open directory '%s'", srcs[i]);

    /* Rebuild source files */
    FOREACH_IN_DIR(
        srcs[i], dir, ent,
        {
          if (strcmp(fs_ext(ent.name), "c") != 0)
            continue;

          assert(o_files_count < sizeof(o_files) / sizeof(o_files[0]));

          char *out = build_file(cc, &c, bin, srcs[i], ent.name, rebuild_all);
          o_files[o_files_count++] = out;
        },
        status);

    if (status != 0)
      LOG_FATAL("Failed to open directory '%s'", srcs[i]);
  }

  if (o_files_count == 0)
    LOG_INFO("Nothing to rebuild");
  else {
    if (build_cache_save(&c) != 0)
      LOG_FATAL("Failed to save build cache");

    COMPILE("ld", o_files, o_files_count, "-o", out, LDARGS);

    for (size_t i = 0; i < o_files_count; ++i)
      free(o_files[i]);
  }

  build_cache_free(&c);
}

int main(int argc, const char **argv) {
  args_t a = build_init(argc, argv);
  build_set_usage("[clean] [OPTIONS]");

  flag_cstr(NULL, "CC", "The C compiler path", &cc);

  args_t stripped;
  build_parse_args(&a, &stripped);

  const char *subcmd = args_shift(&a);
  if (subcmd != NULL) {
    if (a.c > 0) {
      build_arg_error("Unexpected argument '%s' for '%s'", a.v[0], subcmd);
      exit(EXIT_FAILURE);
    }

    if (strcmp(subcmd, "clean") == 0) {
      build_clean(BIN);

      if (fs_exists(BIN "/" OUT))
        fs_remove_file(BIN "/" OUT);
    } else {
      build_arg_error("Unknown subcommand '%s'", subcmd);
      exit(EXIT_FAILURE);
    }
  } else {
    const char *srcs[] = {SRC};
    build_kernel(cc, srcs, sizeof(srcs) / sizeof(srcs[0]), BIN, BIN "/" OUT);
  }

  free(stripped.base);
  return EXIT_SUCCESS;
}