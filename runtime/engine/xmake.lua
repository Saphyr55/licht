
target("licht.engine", function()
    set_kind("shared")
    set_group("engine")

    add_deps("licht.core")

    target_files_default({
        public = true
    })

    add_defines("LICHT_ENGINE_EXPORTS")
end)