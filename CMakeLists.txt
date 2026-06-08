cmake_minimum_required(VERSION 4.3.2)
project(HyplogProject C)

set(I_VAL 0 CACHE STRING "Interactivity level (0 or 1)")
set(TLEN_VAL 512 CACHE STRING "Max text length")
set(LAST_VAL 5 CACHE STRING "How many elements to show")
set(PATH_VAL "~/.hyplog/" CACHE STRING "The directory to save the logs")


if(PATH_VAL MATCHES "^~")
    set(HOME_DIR $ENV{HOME})
    string(REPLACE "~" "${HOME_DIR}" PATH_VAL "${PATH_VAL}")
endif()


add_executable(hyplog hyplog.c)


target_compile_definitions(hyplog PRIVATE 
    I=${I_VAL}
    TLEN=${TLEN_VAL}
    LAST=${LAST_VAL}
    PATH="${PATH_VAL}"
)

message(STATUS "Configuring Hyplog with:")
message(STATUS "  Interactivity (I): ${I_VAL}")
message(STATUS "  Max Text Len (TLEN): ${TLEN_VAL}")
message(STATUS "  Last Elements (LAST): ${LAST_VAL}")
message(STATUS "  Log Path (PATH): ${PATH_VAL}")

