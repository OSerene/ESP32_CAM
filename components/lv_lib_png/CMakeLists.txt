if(ESP_PLATFORM)

file(GLOB SOURCES lv_lib_png/*.c)


idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS .
                       REQUIRES lvgl)

endif()
