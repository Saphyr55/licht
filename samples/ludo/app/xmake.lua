
target("ludo.app", function()
    set_kind("binary")
    set_group("ludo")

    local ludo_projectdir = path.join(os.projectdir(), "samples/ludo")
    local runtime_enginedir = path.join(os.projectdir(), "runtime")

    set_runargs("--projectdir", ludo_projectdir, "--enginedir", runtime_enginedir)

    add_deps("ludo", "licht.core", "licht.launcher", "licht.engine")

    target_files_default({
        public = false
    })

end)
