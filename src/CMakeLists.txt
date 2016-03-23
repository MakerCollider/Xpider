CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# SET(SRC_LIST main.c)
FIND_PACKAGE(OpenCV REQUIRED)


INCLUDE_DIRECTORIES(/usr/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

#ADD_EXECUTABLE(sf_controller ${PROJECT_BINARY_DIR}/src ${PROJECT_BINARY_DIR}/lib)
ADD_EXECUTABLE(sf_controller main.c ../lib/SF_Controller.c)
ADD_EXECUTABLE(FaceTrack FaceTrack.cpp ../lib/SF_Controller.c)
ADD_EXECUTABLE(GeometryDetect GeometryDetect.cpp ../lib/SF_Controller.c)
# ADD_EXECUTABLE(sf_controller ${SRC_LIST})

#SET_TARGET_PROPERTIES(sf_controller PROPERTIES LINKER_LANGUAGE C)

TARGET_LINK_LIBRARIES(sf_controller mraa pthread)
TARGET_LINK_LIBRARIES(FaceTrack mraa pthread ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(GeometryDetect mraa pthread ${OpenCV_LIBS})

# TARGET_LINK_LIBRARIES(hellomraa mraa)
# TARGET_LINK_LIBRARIES(i2c_HMC5883L mraa m)
# TARGET_LINK_LIBRARIES(cycle-pwm3 mraa)
# TARGET_LINK_LIBRARIES(blink-io mraa)
# TARGET_LINK_LIBRARIES(analogin_a0 mraa)
# TARGET_LINK_LIBRARIES(isr_pin6 mraa)
# TARGET_LINK_LIBRARIES(gpio_read6 mraa)
# TARGET_LINK_LIBRARIES(spi_mcp4261 mraa)
# TARGET_LINK_LIBRARIES(mmap-io2 mraa)
# TARGET_LINK_LIBRARIES(blink_onboard mraa)
# TARGET_LINK_LIBRARIES(uart mraa)
# TARGET_LINK_LIBRARIES(mraa-gpio mraa)
# TARGET_LINK_LIBRARIES(mraa-i2c mraa)
# TARGET_LINK_LIBRARIES(spi_max7219 mraa)