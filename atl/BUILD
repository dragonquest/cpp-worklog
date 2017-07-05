cc_library(
    name = "atl",
    hdrs = [
        "file.h",
        "optional.h",
        "stream.h",
        "string.h",
        "time.h",
        "colors.h",

        "status.h",
        "statusor.h",
    ],
    srcs = [
        "file.cc",
        "string.cc",
        "time.cc",
    ],
    deps = [
        "@boost//:filesystem",
        "@boost//:algorithm",
        "@boost//:integer",
        "@boost//:concept",
        "@boost//:type_index",
        "//gtl",
    ],
    visibility = ["//visibility:public"],
)
