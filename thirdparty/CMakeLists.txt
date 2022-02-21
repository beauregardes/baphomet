set(headers ${headers}
    thirdparty/glad/include/glad/gl.h
    thirdparty/glad/include/KHR/khrplatform.h

    thirdparty/imgui/include/backends/imgui_impl_glfw.h
    thirdparty/imgui/include/backends/imgui_impl_opengl3.h
    thirdparty/imgui/include/backends/imgui_impl_opengl3_loader.h
    thirdparty/imgui/include/imconfig.h
    thirdparty/imgui/include/imgui.h
    thirdparty/imgui/include/imgui_internal.h
    thirdparty/imgui/include/imstb_rectpack.h
    thirdparty/imgui/include/imstb_textedit.h
    thirdparty/imgui/include/imstb_truetype.h

    thirdparty/implot/include/implot.h
    thirdparty/implot/include/implot_internal.h

    thirdparty/stb/stb_image.h
    thirdparty/stb/stb_image_write.h
)

set(srcs ${srcs}
    thirdparty/glad/src/gl.c

    thirdparty/imgui/src/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/src/backends/imgui_impl_opengl3.cpp
    thirdparty/imgui/src/imgui.cpp
    thirdparty/imgui/src/imgui_demo.cpp
    thirdparty/imgui/src/imgui_draw.cpp
    thirdparty/imgui/src/imgui_tables.cpp
    thirdparty/imgui/src/imgui_widgets.cpp

    thirdparty/implot/src/implot.cpp
    thirdparty/implot/src/implot_demo.cpp
    thirdparty/implot/src/implot_items.cpp
)