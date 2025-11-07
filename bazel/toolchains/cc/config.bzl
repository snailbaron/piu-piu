load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "action_config",
    "env_entry",
    "env_set",
    "feature",
    "flag_group",
    "flag_set",
    "tool",
)
load("@info//:info.bzl", "info")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")

tool_targets_map = {
    "clang": "@llvm//:bin/clang",
    "clangxx": "@llvm//:bin/clang++",
    "llvm_ar": "@llvm//:bin/llvm-ar",
}

tool_targets = struct(**tool_targets_map)
tool_attrs = {
    "_" + name: attr.label(allow_single_file = True, default = path)
    for name, path in tool_targets_map.items()
}

def _cc_toolchain_config_impl(ctx):
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "linux-x86_64-clang++",
        compiler = "clang++",
        features = [
            feature(
                name = "default",
                enabled = True,
                flag_sets = [
                    flag_set(
                        actions = [
                            ACTION_NAMES.cpp_compile,
                            ACTION_NAMES.cpp_link_dynamic_library,
                            ACTION_NAMES.cpp_link_executable,
                        ],
                        flag_groups = [
                            flag_group(flags = ["-stdlib=libc++"]),
                        ],
                    ),
                    flag_set(
                        actions = [ACTION_NAMES.cpp_link_executable],
                        flag_groups = [
                            flag_group(flags = ["-fuse-ld=lld"]),
                            flag_group(flags = ["-static-libstdc++"]),
                            flag_group(flags = ["-l:libc++abi.a"]),
                            flag_group(flags = ["-static-libgcc"]),
                        ],
                    ),
                ],
                env_sets = [
                    env_set(
                        actions = [ACTION_NAMES.cpp_link_executable],
                        env_entries = [
                            env_entry(
                                "LIBRARY_PATH",
                                info.llvm_root + "lib/x86_64-unknown-linux-gnu",
                            ),
                        ],
                    ),
                ],
            ),
        ],
        action_configs = [
            action_config(
                action_name = ACTION_NAMES.c_compile,
                enabled = True,
                tools = [tool(tool = ctx.file._clang)],
            ),
            action_config(
                action_name = ACTION_NAMES.cpp_compile,
                enabled = True,
                tools = [tool(tool = ctx.file._clangxx)],
            ),
            action_config(
                action_name = ACTION_NAMES.cpp_link_static_library,
                enabled = True,
                tools = [tool(tool = ctx.file._llvm_ar)],
                implies = ["archiver_flags"],
            ),
            action_config(
                action_name = ACTION_NAMES.cpp_link_executable,
                enabled = True,
                tools = [tool(tool = ctx.file._clangxx)],
            ),
        ],
        cxx_builtin_include_directories = [
            info.llvm_root + "/include/x86_64-unknown-linux-gnu/c++/v1",
            info.llvm_root + "/include/c++/v1",
            info.llvm_root + "/lib/clang/21/include",
            "/usr/local/include",
            "/usr/include",
        ],
    )

cc_toolchain_config = rule(
    implementation = _cc_toolchain_config_impl,
    attrs = tool_attrs,
)
