----------------------------------------------------
-- Ludo --------------------------------------------
----------------------------------------------------
target("ludo", function()
    set_kind("shared")
    add_rules("glsl")
    set_group("ludo")

    local deps = {
        "licht.core",
        "licht.engine",
        "licht.rhi",
        "licht.rhi.vulkan",
        "stb"
    }

    add_deps(unpack(deps))

    add_includedirs("ludo", {
        public = true
    })

    add_files("ludo/**.cpp")
    add_headerfiles("ludo/**.hpp")

    add_files("$(projectdir)/assets/shaders/**.frag", "$(projectdir)/assets/shaders/**.vert")

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
    add_headerfiles("app/**.hpp")

    add_files("app/**.cpp")
end)
