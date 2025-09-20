
----------------------------------------------------
-- Ludo --------------------------------------------
----------------------------------------------------
target("ludo", function()
    set_kind("shared")
    set_group("ludo")

    add_deps("licht.core", "licht.engine", "licht.rhi", "licht.rhi.vulkan")

    add_includedirs("ludo", {public = true})

    add_files("ludo/**.cpp")
    add_headerfiles("ludo/**.hpp")

    add_defines("LICHT_LUDO_EXPORTS")
end)

target("ludo.app", function()
    set_kind("binary")
    set_group("ludo")

    local ludo_projectdir = path.join(os.projectdir(), "samples/ludo")
    local enginedir = path.join(os.projectdir(), "engine")

    set_runargs("--projectdir", ludo_projectdir, "--enginedir", enginedir)

    add_deps("ludo", "licht.core", "licht.launcher", "licht.engine")

    add_includedirs("app")

    add_files("app/**.cpp")
    add_headerfiles("app/**.hpp")

    before_run(function(target)
        import("core.base.task")
        import("core.project.project")

        task.run("compile-shaders", {
            output = path.join(target:targetdir(), "shaders"),
            files = {
                "shaders/main.frag",
                "shaders/main.vert"
            }
        })

    end)

end)
