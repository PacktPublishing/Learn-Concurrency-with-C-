file(GLOB LISTING_SOURCES "listing6_*.cpp")

foreach(source ${LISTING_SOURCES})
    get_filename_component(target_name ${source} NAME_WE)
    add_executable(${target_name} ${source})
    set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 26)
endforeach()
