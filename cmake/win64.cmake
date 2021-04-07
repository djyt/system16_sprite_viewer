# -----------------------------------------------------------------------------
# CannonBall Windows Setup
# -----------------------------------------------------------------------------

# Library Locations
set(lib_base c:/coding/lib)
set(boost_dir ${lib_base}/boost_1_74_0)
set(sdl2_dir ${lib_base}/SDL2-2.0.12)

# Use OpenGL for rendering.
set(OPENGL 1)

# Platform Specific Libraries
set(platform_link_libs
    opengl32 # For OpenGL
    glu32    # For OpenGL
)

# Platform Specific Link Directories
set(platform_link_dirs

)