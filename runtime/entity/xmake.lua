target("licht.entity", function()
    set_kind("shared")
    set_group("engine")

    add_deps("licht.core")

    target_files_default({
        public = true
    })

    add_defines("LICHT_ENTITY_EXPORTS")
end)

target("licht.entity.tests", function()
    set_kind("binary")
    set_group("engine.tests")

    add_deps("licht.core", "licht.entity")

    add_packages("catch2")

    add_includedirs("tests")

    add_files("tests/**.cpp")

    add_defines("LICHT_ENTITY_EXPORTS")
end)