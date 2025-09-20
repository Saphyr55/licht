
target("licht.launcher", function()
    set_kind("shared")
    set_group("engine")

    add_deps("licht.core", "licht.engine")

    target_files_default({
        public = true
    })

    add_defines("LICHT_LAUNCHER_EXPORTS")

    if is_plat("windows") then
        add_syslinks("User32")
    end
end)

