"""Rules for binary data format"""

def _data_pack_impl(ctx):
    out_data_file = ctx.actions.declare_file(ctx.label.name + ".piuf")
    ctx.actions.run(
        outputs = [out_data_file],
        inputs = [ctx.file.meta, ctx.file.sheet],
        executable = ctx.executable._pack,
        arguments = [
            ctx.file.sheet.path,
            ctx.file.meta.path,
            out_data_file.path,
        ],
    )

    return DefaultInfo(files = depset([out_data_file]))

data_pack = rule(
    implementation = _data_pack_impl,
    attrs = {
        "_pack": attr.label(
            default = "//src/packer:pack",
            executable = True,
            cfg = "exec",
        ),
        "meta": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "path to meta YAML file",
        ),
        "sheet": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "path to PNG sheet",
        ),
    },
)
