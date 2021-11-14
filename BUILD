cc_library(
    name = "execution",
    srcs = [
        "bulk.h",
        "dummy.cpp",
        "inline_task.h",
        "intrusive_forward_list.h",
        "invoke_traits.h",
        "sync_wait.h",
        "then.h",
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
        "bulk_tests.cpp",
        "intrusive_forward_list_tests.cpp",
        "invoke_traits_tests.cpp",
        "then_tests.cpp",
    ],
    deps = [
        ":execution",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "benchmark",
    srcs = [
        "execution_benchmark.cpp",
    ],
    deps = [
        ":execution",
        "@com_google_benchmark//:benchmark_main",
    ],
)
