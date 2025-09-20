
target("licht.core.tests", function()
    set_kind("binary")
    set_group("engine.tests")

    add_deps("licht.core")

    add_packages("catch2")

    add_includedirs("core")

    add_files("tests/licht/core/**.cpp")
end)
