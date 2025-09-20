target("licht.core", function()
    set_kind("shared")
    set_group("engine")

    add_packages("libsdl3", "lua")

    target_files_default({
        public = true
    })

    add_defines("LICHT_CORE_EXPORTS")
end)

