cc_library(
    name = "execution",
    srcs = [
        "dummy.cpp",
        "invoke_traits.h",
        "sync_wait.h",
        "thread_pool.h",
    ],
    hdrs = [
        "execution.h",
    ],
    copts = ["-pthread"],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "test",
    srcs = [
        "invoke_traits_tests.cpp",
        "then_tests.cpp",
    ],
    deps = [
        ":execution",
        "@com_google_googletest//:gtest_main",
    ],
)
