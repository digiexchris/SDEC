# Collect all .cpp and .h files into two variables
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "inc/*.h" "inc/*.hpp")


Include(FetchContent)

FetchContent_Declare(
  etl
  GIT_REPOSITORY https://github.com/ETLCPP/etl
  GIT_TAG        20.38.10
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        10.2.0
)

FetchContent_MakeAvailable(etl)
FetchContent_MakeAvailable(fmt)


# Create a library or executable
add_library(MAIN_SOURCES ${SOURCES} ${HEADERS}) # replace 'MyLibrary' with your library name

target_link_libraries(MAIN_SOURCES PRIVATE etl::etl)
target_link_libraries(MAIN_SOURCES PRIVATE fmt::fmt)

# # Link the fmt and etl libraries to your target
# target_link_libraries(MAIN_SOURCES PUBLIC fmt::fmt etl ST_PROJECT_INTERFACE)

# # Add the include directories of the fmt and etl libraries to your target
# target_include_directories(MAIN_SOURCES PUBLIC ${fmt_SOURCE_DIR}/include ${etl_SOURCE_DIR}/include)