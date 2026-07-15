get_all_sources("${CMAKE_CURRENT_SOURCE_DIR}/src" LIB_SOURCES)

add_library(app_lib ${LIB_SOURCES} lib.def)

target_include_directories(app_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(BUILD_SHARED_LIBS)
    # Здесь могут быть специфичные настройки для динамической библиотеки
    # set_target_properties(app_lib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()