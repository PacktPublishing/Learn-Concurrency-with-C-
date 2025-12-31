get_filename_component(CHAPTER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CHAPTER_SOURCES
    listing2_1.cpp
    listing2_2.cpp
    listing2_3.cpp
    listing2_4.cpp
    listing2_5.cpp
    listing2_6.cpp
    listing2_7.cpp
    listing2_8.cpp
    listing2_9.cpp
    listing2_10.cpp
    listing2_11.cpp
)

foreach(source ${CHAPTER_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)
    set(prefixed_target "${CHAPTER_NAME}_${target_name}")
    add_executable(${prefixed_target} ${source})
    set_property(TARGET ${prefixed_target} PROPERTY CXX_STANDARD 26)
endforeach()