get_filename_component(CHAPTER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CHAPTER_SOURCES
    listing1_1.cpp
    listing1_2.cpp
    listing1_3.cpp
    listing1_4.cpp
    listing1_5.cpp
    listing1_6.cpp
    listing1_7.cpp
    listing1_8.cpp
    listing1_9.cpp
    listing1_10.cpp
    listing1_11.cpp
)

foreach(source ${CHAPTER_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)
    set(prefixed_target "${CHAPTER_NAME}_${target_name}")
    add_executable(${prefixed_target} ${source})
    set_property(TARGET ${prefixed_target} PROPERTY CXX_STANDARD 26)
endforeach()