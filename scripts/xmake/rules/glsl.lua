rule("glsl", function()

    set_extensions(".glsl", ".vert", ".frag")

    on_build_file(function(target, sourcefile, opt)
        local output = path.join(target:targetdir(), "shaders")

        cprint("${blue}Compiling '" .. sourcefile .. "'' shader...")
        cprint("${blue}output = %s", output)

        os.mkdir(output)

        local filename = path.filename(sourcefile)
        local output_binary_source = path.join(output, filename .. ".spv")

        os.vrunv("glslc", {
            sourcefile,
            "-o",
            output_binary_source
        })
    end)
end)
