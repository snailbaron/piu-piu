_common_fb_attrs = {
    "_flatc": attr.label(
        cfg = "exec",
        default = "@flatbuffers//:flatc",
        executable = True,
    ),
    "src": attr.label(
        allow_single_file = True,
        doc = "source .fbs file",
        mandatory = True,
    ),
}

def _fb_header_impl(ctx):
    if ctx.file.src.extension != "fbs":
        fail("fb_header: 'src' file must have .fbs extension")

    base_name = ctx.file.src.basename.removesuffix(".fbs")
    header_name = base_name + "_generated.h"

    out_file = ctx.actions.declare_file(header_name)
    ctx.actions.run(
        outputs = [out_file],
        inputs = [ctx.file.src],
        executable = ctx.executable._flatc,
        arguments = [
            "--cpp",
            "--scoped-enums",
            "--no-emit-min-max-enum-values",
            "-o",
            out_file.dirname,
            ctx.file.src.path,
        ],
    )

    return DefaultInfo(files = depset([out_file]))

fb_header = rule(
    implementation = _fb_header_impl,
    attrs = _common_fb_attrs,
)

def _fb_python_impl(ctx):
    if ctx.file.src.extension != "fbs":
        fail("fb_header: 'src' file must have .fbs extension")

    base_name = ctx.file.src.basename.removesuffix(".fbs")
    source_name = base_name + "_generated.py"

    out_file = ctx.actions.declare_file(source_name)

    ctx.actions.run(
        outputs = [out_file],
        inputs = [ctx.file.src],
        executable = ctx.executable._flatc,
        arguments = [
            "--python",
            "--gen-onefile",
            "--python-typing",
            #"--python-no-type-prefix-suffix",
            "-o",
            out_file.dirname,
            ctx.file.src.path,
        ],
    )

    return DefaultInfo(files = depset([out_file]))

fb_python = rule(
    implementation = _fb_python_impl,
    attrs = _common_fb_attrs,
)
