link_libraries(crt crt_helper gcc c m)
file(GLOB_RECURSE SOURCES "src/*.c")

add_definitions(-DSWILIB_MODERN)

add_executable(89ThemeEngine_ELKA ${SOURCES})
target_sdk_setup(89ThemeEngine_ELKA ELKA)
target_compile_options(89ThemeEngine_ELKA BEFORE PUBLIC -std=c11 -Wall -Werror)
set_target_properties(89ThemeEngine_ELKA PROPERTIES OUTPUT_NAME "89ThemeEngine")
