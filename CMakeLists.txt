CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Sudoko_Solver)

FIND_PACKAGE(OpenCV REQUIRED)

FILE(GLOB SRCS src/*.cpp)
FILE(GLOB HDRS include/*.hpp)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

INCLUDE_DIRECTORIES(include)

ADD_EXECUTABLE(sudoku_solve ${SRCS} ${HDRS})

TARGET_LINK_LIBRARIES(sudoku_solve ${OpenCV_LIBS})
