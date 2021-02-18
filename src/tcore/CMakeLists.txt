aux_source_directory(. TCORE_SRC)

add_library(tcore OBJECT ${TCORE_SRC})

target_link_libraries(tcore PUBLIC texas_lib)

target_include_directories(tcore PUBLIC ..)