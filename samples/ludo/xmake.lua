
----------------------------------------------------
-- Ludo --------------------------------------------
----------------------------------------------------
target("ludo", function()
    set_kind("shared")
    add_rules("glsl")

    add_deps("licht.core", "licht.engine", "licht.rhi", "licht.rhi.vulkan")

    add_includedirs("ludo", {public = true})

    add_files("ludo/**.cpp")
    add_headerfiles("ludo/**.hpp")

    add_files("$(projectdir)/shaders/**.frag", "$(projectdir)/shaders/**.vert")

    add_defines("LICHT_LUDO_EXPORTS")

    set_group("ludo")
end)

target("ludo.app", function()
    set_kind("binary")

    local ludo_projectdir = path.join(os.projectdir(), "samples/ludo")
    local enginedir = path.join(os.projectdir(), "engine")

    set_runargs("--projectdir", ludo_projectdir, "--enginedir", enginedir)

    add_deps("ludo", "licht.core", "licht.launcher", "licht.engine")

    add_includedirs("app")
    add_headerfiles("app/**.hpp")

    add_files("app/**.cpp")

    set_group("ludo")
end)
