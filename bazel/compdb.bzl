"""Rules and aspects to generate compile_commands.json"""

load("@info//:info.bzl", "workspace_root")
load("@rules_cc//cc:action_names.bzl", "ACTION_NAMES")
load("@rules_cc//cc:find_cc_toolchain.bzl", "find_cc_toolchain", "use_cc_toolchain")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")
load("@rules_cc//cc/common:cc_info.bzl", "CcInfo")

CompdbInfo = provider(
    doc = "Command lines for multiple sets of files",
    fields = {
        "file_arguments": """\
dictionary:
  key: file path relative to execroot
  value: list of string arguments to compile the file
""",
    },
)

def _compdb_aspect_impl(target, ctx):
    source_file_targets = []
    if hasattr(ctx.rule.attr, "srcs"):
        source_file_targets += ctx.rule.attr.srcs
    if hasattr(ctx.rule.attr, "hdrs"):
        source_file_targets += ctx.rule.attr.hdrs

    source_file_paths = []
    for t in source_file_targets:
        source_file_paths.extend(
            [f.path for f in t[DefaultInfo].files.to_list()],
        )

    cc_toolchain = find_cc_toolchain(ctx)
    compilation_context = target[CcInfo].compilation_context

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    compiler_path = cc_common.get_tool_for_action(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.cpp_compile,
    )
    compile_variables = cc_common.create_compile_variables(
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        user_compile_flags = ctx.fragments.cpp.copts + ctx.fragments.cpp.cxxopts,
        include_directories = compilation_context.includes,
        quote_include_directories = compilation_context.quote_includes,
        system_include_directories = compilation_context.system_includes,
        framework_include_directories = compilation_context.framework_includes,
        preprocessor_defines = compilation_context.defines,
    )
    command_line = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.cpp_compile,
        variables = compile_variables,
    )

    file_arguments = {}
    if hasattr(ctx.rule.attr, "deps"):
        for dep in ctx.rule.attr.deps:
            file_arguments |= dep[CompdbInfo].file_arguments

    for path in source_file_paths:
        file_arguments[path] = [compiler_path, "-xc++"] + command_line

    return CompdbInfo(file_arguments = file_arguments)

compdb_aspect = aspect(
    implementation = _compdb_aspect_impl,
    attr_aspects = ["deps"],
    required_providers = [CcInfo],
    toolchains = use_cc_toolchain(),
    fragments = ["cpp"],
)

def _compdb_impl(ctx):
    file_arguments = {}
    for src in ctx.attr.srcs:
        file_arguments |= src[CompdbInfo].file_arguments

    compile_commands_data = []
    for path, arguments in file_arguments.items():
        compile_commands_data.append({
            "file": path,
            "arguments": arguments,
            "directory": workspace_root,
        })

    out_file = ctx.actions.declare_file("compile_commands.json")
    ctx.actions.write(
        output = out_file,
        content = json.encode_indent(compile_commands_data, indent = "  "),
    )

    return DefaultInfo(files = depset([out_file]))

compdb = rule(
    implementation = _compdb_impl,
    attrs = {
        "_info": attr.label(allow_single_file = True, default = "@info"),
        "srcs": attr.label_list(
            mandatory = True,
            aspects = [compdb_aspect],
            doc = "targets to build compilation database for",
        ),
    },
)
