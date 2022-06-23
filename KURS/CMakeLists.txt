cmake_minimum_required(
     VERSION 3.15
)

# Should be setted before project directive
set(CMAKE_TOOLCHAIN_FILE
     ${CMAKE_SOURCE_DIR}/toolchain/arm-none-eabi-gcc.cmake
)

get_filename_component(PRJ_ID ${CMAKE_CURRENT_LIST_DIR} NAME)
#================================================================================
project(
${PRJ_ID}
     LANGUAGES C ASM
)

set(LIBOPENCM3_DIR
     ${CMAKE_SOURCE_DIR}/../libopencm3
)

set(OOCD_CONFIG
     ${CMAKE_SOURCE_DIR}/toolchain/openocd_discovery.cfg
)

set(OOCD_COMMANDS
     -c init -c "reset halt" -c "flash write_image erase ${PROJECT_NAME}" -c "verify_image ${PROJECT_NAME}" -c reset -c shutdown
)

#================================================================================
# Main target
add_executable(
     ${PROJECT_NAME} src/main.c)
     
target_compile_definitions(
     ${PROJECT_NAME} PRIVATE
     -DSTM32F4
)
target_include_directories(
     ${PROJECT_NAME} PRIVATE
     ${LIBOPENCM3_DIR}/include
)
target_compile_options(
     ${PROJECT_NAME} PRIVATE
     -mcpu=cortex-m4
     -mthumb
     -mfpu=fpv4-sp-d16
     -mfloat-abi=hard

     -fdata-sections
     -ffunction-sections

     -Wall

     $<$<CONFIG:Debug>:-Og>
)
target_link_libraries(
     ${PROJECT_NAME}
     ${LIBOPENCM3_DIR}/lib/libopencm3_stm32f4.a
     m # стандартная математическая библиотека
)
target_link_options(
     ${PROJECT_NAME} PRIVATE
     -T${CMAKE_SOURCE_DIR}/toolchain/stm32f407vgt6.ld
     -mcpu=cortex-m4
     -mthumb
     -mfpu=fpv4-sp-d16
     -mfloat-abi=hard
     -specs=nano.specs
     -lc
#     -lm
     -lnosys
     -Wl,-Map=${PROJECT_NAME}.map,--cref
     -Wl,--gc-sections
)


# Print executable size
add_custom_command(
     TARGET ${PROJECT_NAME}
     POST_BUILD
     COMMAND arm-none-eabi-size ${PROJECT_NAME})

# Create hex file
add_custom_command(
     TARGET ${PROJECT_NAME}
     POST_BUILD
     COMMAND arm-none-eabi-objcopy -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
     COMMAND arm-none-eabi-objcopy -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
)

# Program target
add_custom_target(
     flash
     COMMAND openocd -f ${OOCD_CONFIG} ${OOCD_COMMANDS}
)
