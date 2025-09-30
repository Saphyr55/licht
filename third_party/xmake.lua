
target("stb", function()
    set_kind("headeronly")

    add_includedirs("stb", {public = true})
    
    add_headerfiles("stb/stb_image.h")
end)
