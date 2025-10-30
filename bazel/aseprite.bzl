"""Rules to export .aseprite files"""

def _aseprite_export_impl(ctx):
    output_files = []

    for input_file in ctx.files.srcs:
        stem = input_file.basename.removesuffix("." + input_file.extension)

        out_png = ctx.actions.declare_file(stem + ".png")
        out_json = ctx.actions.declare_file(stem + ".json")

        ctx.actions.run(
            outputs = [out_png, out_json],
            inputs = [input_file],
            executable = "aseprite",
            arguments = [
                "--batch",
                "--sheet=" + out_png.path,
                "--data=" + out_json.path,
                "--format=json-array",
                "--filename-format={title}:{tag}:{tagframe}",
                input_file.path,
            ],
        )

        output_files.extend([out_png, out_json])

    return DefaultInfo(files = depset(output_files))

aseprite_export = rule(
    implementation = _aseprite_export_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            doc = "source .aseprite files",
        ),
    },
)

def _aseprite_export_sheet_impl(ctx):
    out_png = ctx.actions.declare_file(ctx.label.name + ".png")
    out_json = ctx.actions.declare_file(ctx.label.name + ".json")

    ctx.actions.run(
        outputs = [out_png, out_json],
        inputs = ctx.files.srcs,
        executable = "aseprite",
        arguments = [
            "--batch",
            "--sheet=" + out_png.path,
            "--data=" + out_json.path,
            "--format=json-array",
            "--filename-format={title}:{tag}:{tagframe}",
            "--sheet-type=packed",
        ] + [f.path for f in ctx.files.srcs],
    )

    return DefaultInfo(files = depset([out_png, out_json]))

aseprite_export_sheet = rule(
    implementation = _aseprite_export_sheet_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            doc = "source .aseprite files",
            mandatory = True,
        ),
    },
)
