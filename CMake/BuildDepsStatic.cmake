include(FetchContent)

# Global Settings for Static Builds
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) 

# --- 1. ZLIB ---
FetchContent_Declare(
  zlib
  URL https://github.com/madler/zlib/archive/refs/tags/v1.3.1.tar.gz
)
FetchContent_MakeAvailable(zlib)

# --- 2. LIBJPEG-TURBO ---
FetchContent_Declare(
  libjpeg-turbo
  URL https://github.com/libjpeg-turbo/libjpeg-turbo/archive/refs/tags/3.0.1.tar.gz
)
set(ENABLE_SHARED OFF CACHE BOOL "" FORCE)
set(ENABLE_STATIC ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libjpeg-turbo)

# --- 3. LIBPNG ---
FetchContent_Declare(
  libpng
  URL https://github.com/pnggroup/libpng/archive/refs/tags/v1.6.40.tar.gz
)
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
# Ensure it uses our fetched zlib
set(ZLIB_INCLUDE_DIR $<TARGET_PROPERTY:zlibstatic,INTERFACE_INCLUDE_DIRECTORIES> CACHE PATH "" FORCE)
FetchContent_MakeAvailable(libpng)

# --- 4. LIBTIFF ---
FetchContent_Declare(
  libtiff
  URL https://gitlab.com/libtiff/libtiff/-/archive/v4.6.0/libtiff-v4.6.0.tar.gz
)
# Disable unwanted extras to keep it "no dependencies"
set(tiff-tools OFF CACHE BOOL "" FORCE)
set(tiff-tests OFF CACHE BOOL "" FORCE)
set(tiff-docs OFF CACHE BOOL "" FORCE)
set(jpeg ON CACHE BOOL "" FORCE)
set(zlib ON CACHE BOOL "" FORCE)
set(pixarlog ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libtiff)

# --- SUMMARY ---
message(STATUS "Static Libraries configured: zlib, jpeg-static, png_static, tiff")
