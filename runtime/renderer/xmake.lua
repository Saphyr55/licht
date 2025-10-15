add_requires("tinygltf")

target("licht.renderer", function()
    set_kind("shared")
    set_group("engine")

    add_packages("tinygltf", {
        public = false
    })

    add_deps("licht.core", "licht.rhi")

    target_files_default({
        public = true
    })

    add_defines("LICHT_RENDERER_EXPORTS")
end)
