
target("licht.schema", function()
    set_kind("shared")
    set_group("engine")

    add_deps("licht.core", "licht.renderer")

    target_files_default({
        public = true
    })

    add_defines("LICHT_SCHEMA_EXPORTS")
end)
