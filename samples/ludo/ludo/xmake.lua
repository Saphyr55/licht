target("ludo", function()
    set_kind("shared")
    add_rules("glsl")
    set_group("ludo")

    local deps = {
        "licht.core",
        "licht.engine",
        "licht.rhi",
        "licht.rhi.vulkan",
        "licht.messaging",
        "licht.scene",
        "licht.renderer",
        "stb"
    }

    add_deps(unpack(deps))

    target_files_default({
        public = true
    })

    add_files("../assets/shaders/**.frag", "../assets/shaders/**.vert")

    add_defines("LICHT_LUDO_EXPORTS")
end)
