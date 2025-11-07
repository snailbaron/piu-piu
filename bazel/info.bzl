def _escape(string):
    return string.replace("\\", "\\\\").replace('"', '\\"')

def _info_impl(repository_ctx):
    repository_ctx.file("info.bzl", content = """\
info = struct(
    workspace_root = "{workspace_root}",
    llvm_root = "{llvm_root}",
)
""".format(
        workspace_root = _escape(str(repository_ctx.workspace_root)),
        llvm_root = str(repository_ctx.path(Label("@llvm//:BUILD.bazel")).dirname),
    ))

    repository_ctx.file("BUILD.bazel", content = """\
exports_files(["info.bzl"])
""")

info = repository_rule(
    implementation = _info_impl,
)
