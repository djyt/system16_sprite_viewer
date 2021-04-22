# -----------------------------------------------------------------------------
# Sprite Viewer Windows Setup
# -----------------------------------------------------------------------------

# Library Locations
set(lib_base c:/coding/lib)
set(boost_dir ${lib_base}/boost_1_74_0)
set(sdl2_dir ${lib_base}/SDL2-2.0.12)
set(sdl2_ttf_dir ${lib_base}/SDL2_ttf-2.0.15)

# I couldn't find an easy way to set this up on Windows, so setup manually...
set(SDL2TTF_INCLUDE_DIRS ${sdl2_ttf_dir}/include)
set(SDL2TTF_LIBRARIES ${sdl2_ttf_dir}/lib/x64/SDL2_ttf.lib)

# Platform Specific Libraries
set(platform_link_libs

)

# Platform Specific Link Directories
set(platform_link_dirs

)