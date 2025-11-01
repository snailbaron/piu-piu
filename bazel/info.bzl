def _info_repo_impl(repository_ctx):
    path = repository_ctx.path(Label("//:MODULE.bazel")).dirname
    repository_ctx.file("info.bzl", content = """\
workspace_root = "{}"
""".format(path))
    repository_ctx.file("BUILD.bazel", content = 'exports_files(["info"])')

info_repo = repository_rule(
    implementation = _info_repo_impl,
)

