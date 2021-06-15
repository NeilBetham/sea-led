load("@bazel_arm_none//:utils.bzl", "gdb_bin")

cc_library(
  name = "sea-led-lib",
  hdrs = glob(["include/**/*.h"]),
  srcs = glob(["src/**/*.c"], exclude = ["src/startup.c"]),
  strip_include_prefix = "include/",
  copts = [
    "-ggdb",
    "-mthumb",
    "-mcpu=cortex-m4",
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=softfp",
    "-specs=nano.specs",
    "-specs=nosys.specs",
  ],
)

cc_binary(
  name = "sea-led",
  srcs = ["src/startup.c"],
  deps = [
    ":sea-led-lib",
    ":ld/TM4C129ENCZAD.ld",
  ],
  copts = [
    "-ggdb",
    "-mthumb",
    "-mcpu=cortex-m4",
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=softfp",
    "-specs=nano.specs",
    "-specs=nosys.specs",
  ],
  linkopts = [
    "-T $(location :ld/TM4C129ENCZAD.ld)",
    "-e Reset_Handler",
    "-mthumb",
    "-mcpu=cortex-m4",
    "-specs=nano.specs",
    "-specs=nosys.specs",
    "-lm",
  ],
)

gdb_bin()
