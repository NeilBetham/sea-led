load("@bazel_arm_none//:utils.bzl", "gdb_bin")

cc_library(
  name = "sys-timing",
  hdrs = ["include/timing.h", "include/registers/core.h", "include/registers/register_helpers.h"],
  srcs = ["src/timing.cpp"],
  strip_include_prefix = "include",
  copts = [
   "-ggdb",
    "-mthumb",
    "-mcpu=cortex-m4",
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=softfp",
    "-specs=nano.specs",
    "-specs=nosys.specs",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "sea-led-lib",
  hdrs = glob(["include/**/*.h"]),
  srcs = glob(["src/**/*.cpp"], exclude = ["src/startup.cpp", "src/handlers.cpp", "src/timing.cpp"]),
  strip_include_prefix = "include/",
  deps = [
    "@lwip//:lwip-lib",
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
)

cc_binary(
  name = "sea-led",
  srcs = [
      "src/startup.cpp",
      "src/handlers.cpp",
  ],
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
    "-lsupc++",
  ],
)

gdb_bin()

