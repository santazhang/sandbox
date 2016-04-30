cc_library(
    name = "b",
    srcs = ["b.cc"],
    hdrs = ["b.h"],
    deps = ["//a"]
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [":b"]
)
