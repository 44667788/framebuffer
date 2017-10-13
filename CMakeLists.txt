cmake_minimum_required(VERSION 3.8)
project(framebuffer)

set(PNG_INC /home/zeke/mips-install/png/include)
set(JPG_INC /home/zeke/mips-install/jpeg9/include)

include_directories(${PNG_INC} ${JPG_INC})

set(JPG_LIB /home/zeke/mips-install/jpeg9/lib)
set(PNG_LIB /home/zeke/mips-install/png/lib)
set(Z_LIB /home/zeke/mips-install/libz/lib)

link_directories(${JPG_LIB} ${PNG_LIB} ${Z_LIB})

set(CMAKE_C_COMPILER mipsel-linux-gcc)
set(CMAKE_C_STANDARD 99)


set(SOURCE_FILES main.c framebuffer.h framebuffer.c image.h image.c str_utils.h str_utils.c)
add_executable(framebuffer ${SOURCE_FILES})
target_link_libraries(framebuffer png jpeg dl m z)