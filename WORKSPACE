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
  url = "https://github.com/NeilBetham/bazel-arm-none/archive/ddee28e26bb323afa49bb41eb7497cae7e7844b9.tar.gz",
  sha256 = "14ccffeedd4f57b5470fd065ae27644de7bc9c9826fca39ce77af3039ab0c6f6",
  strip_prefix = "bazel-arm-none-ddee28e26bb323afa49bb41eb7497cae7e7844b9",
)
load("@bazel_arm_none//:deps.bzl", "toolchain_deps")
toolchain_deps()

# LWIP for networking stack
http_archive(
  name = "lwip",
  url = "http://download.savannah.nongnu.org/releases/lwip/lwip-2.1.2.zip",
  sha256 = "5e0ae1887bef5b27e35f92636a3e52f3bc67f1944f136d6c79d18e5d972f76b0",
  strip_prefix = "lwip-2.1.2",
  build_file = "@//deps/lwip:lwip.BUILD",
)

# FMT for string formatting
http_archive(
  name = "fmt",
  url = "https://github.com/fmtlib/fmt/archive/refs/tags/8.0.1.tar.gz",
  sha256 = "b06ca3130158c625848f3fb7418f235155a4d389b2abc3a6245fb01cb0eb1e01",
  strip_prefix = "fmt-8.0.1",
  build_file = "@//deps/fmt:fmt.BUILD",
)

# Picohttpparser for http request parsing
http_archive(
  name = "picohttp",
  url = "https://github.com/h2o/picohttpparser/archive/657ede0fdf49b806e48c85f128cc55fe8905ed6b.tar.gz",
  sha256 = "869abb24ae5d84456e6a9da982d075ee65ec1328b5d8c5103ba4410c50bbc6a0",
  strip_prefix = "picohttpparser-657ede0fdf49b806e48c85f128cc55fe8905ed6b",
  build_file = "@//deps/picohttp:picohttp.BUILD",
)


