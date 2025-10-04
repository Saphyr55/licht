target("licht.core", function()
    set_kind("shared")
    set_group("engine")

    add_packages("libsdl3", "lua")

    target_files_default({
        public = true
    })

    add_defines("LICHT_CORE_EXPORTS")
end)

target("licht.core.tests", function()
    set_kind("binary")
    set_group("engine.tests")

    add_deps("licht.core")

    add_packages("catch2")

    add_includedirs("tests")

    add_files("tests/**.cpp")
end)
