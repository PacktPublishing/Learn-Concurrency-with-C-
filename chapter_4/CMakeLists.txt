get_filename_component(CHAPTER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CHAPTER_SOURCES
    listing4_1.cpp
    listing4_2.cpp
    listing4_3.cpp
    listing4_4.cpp
    listing4_5.cpp
    listing4_6.cpp
    listing4_7.cpp
    listing4_8.cpp
)

foreach(source ${CHAPTER_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)
    set(prefixed_target "${CHAPTER_NAME}_${target_name}")
    add_executable(${prefixed_target} ${source})
    set_property(TARGET ${prefixed_target} PROPERTY CXX_STANDARD 26)
endforeach()