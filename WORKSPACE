workspace(name = "sea-led")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# External dep compilation tools
http_archive(
  name = "rules_foreign_cc",
  url = "https://github.com/bazelbuild/rules_foreign_cc/archive/3b72ab3468cc8b101352dbe681353a2f8821a057.tar.gz",
  sha256 = "682fa59997d214d42743d822a1284780fd8fb0db4dd88bcb0725904b423cef20",
  strip_prefix="rules_foreign_cc-3b72ab3468cc8b101352dbe681353a2f8821a057",
)
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
rules_foreign_cc_dependencies()

# Bazel Toolchain for ARM embedded
http_archive(
  name = "bazel_arm_none",
  url = "https://github.com/NeilBetham/bazel-arm-none/archive/59ae6a1e64d49fb6ba10cab971a0fcc9dc5ffb46.tar.gz",
  sha256 = "eaf339bb45d0f702caf3499d7699789314a2473c59374b713ec2876ac43aeb9c",
  strip_prefix = "bazel-arm-none-59ae6a1e64d49fb6ba10cab971a0fcc9dc5ffb46",
)
load("@bazel_arm_none//:deps.bzl", "toolchain_deps")
toolchain_deps()
