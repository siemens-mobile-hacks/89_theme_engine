# elf2vkp: https://github.com/siemens-mobile-hacks/elf2vkp

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_LIBRARY_ARCHITECTURE arm-none-eabi)

find_program(ARM_C_COMPILER arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX} NO_CACHE REQUIRED)
find_program(ARM_CXX_COMPILER arm-none-eabi-g++${CMAKE_EXECUTABLE_SUFFIX} NO_CACHE REQUIRED)

set(CMAKE_C_COMPILER ${ARM_C_COMPILER} CACHE INTERNAL "CMAKE_C_COMPILER")
set(CMAKE_CXX_COMPILER ${ARM_CXX_COMPILER} CACHE INTERNAL "CMAKE_CXX_COMPILER")

add_compile_options(-mcpu=arm926ej-s -mthumb-interwork -msoft-float -mlittle-endian -nostdlib -nostdinc)
include_directories(../sdk/dietlibc/include ../sdk/swilib/include/)
add_link_options(-Wl,-z,max-page-size=1,--use-blx)

set(CMAKE_C_FLAGS "-nostdlib -nostdinc")
set(CMAKE_CXX_FLAGS "-nostdlib -nostdinc")
set(CMAKE_ASM_FLAGS "-nostdlib -nostdinc")
set(FULLFLASHES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../elfloader3/ff")

function(define_patch target platform phone svn)
	set_property(TARGET ${target} PROPERTY POSITION_INDEPENDENT_CODE OFF)
	set_property(TARGET ${target} PROPERTY SUFFIX ".elf")
	target_compile_definitions(${target} PUBLIC -D${phone}_${svn})
	target_link_options(${target} PUBLIC -Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/${phone}_${svn}.ld)
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND elf2vkp --section-names --chunk-size 512 -f "${FULLFLASHES_PATH}/${phone}sw${svn}.bin" -i "../${target}.elf" -o "../${target}.vkp"
	)

	if (platform STREQUAL "SG" OR platform STREQUAL "SGOLD")
		message("Compiling for target ${phone}v${svn} (SGOLD)")
		target_compile_definitions(${target} PUBLIC SGOLD)
	elseif (platform STREQUAL "NSG" OR platform STREQUAL "NEWSGOLD")
		message("Compiling for target ${phone}v${svn} (NEWSGOLD)")
		target_compile_definitions(${target} PUBLIC NEWSGOLD)
	elseif (platform STREQUAL "EL" OR platform STREQUAL "ELKA")
		message("Compiling for target ${phone}v${svn} (ELKA)")
		target_compile_definitions(${target} PUBLIC NEWSGOLD ELKA)
	else()
		message(FATAL "Invalid platform: ${platform}")
	endif()
endfunction()
