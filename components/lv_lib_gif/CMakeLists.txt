if(ESP_PLATFORM)

file(GLOB SOURCES lv_lib_gif/*.c)


idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS .
                       REQUIRES lvgl)

endif()
