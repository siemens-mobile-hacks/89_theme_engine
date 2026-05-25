set(CMAKE_C_STANDARD 11)
add_compile_options(-Os)

# E71v45
add_executable(E71v45 src/patch.c src/patch.S)
define_patch(E71v45 ELKA E71 45)
