cmake_minimum_required(VERSION 3.15)
project(AlephSQL LANGUAGES CXX VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
option(BUILD_ALEPH_SQL_TESTS "Build AlephSQL tests" ON)
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(ALEPH_PLATFORM_LINUX TRUE)
  message(STATUS "[INFO] Platform: Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(ALEPH_PLATFORM_WINDOWS TRUE)
  message(STATUS "[INFO] Platform: Windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(ALEPH_PLATFORM_MACOS TRUE)
  message(STATUS "[INFO] Platform: macOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
  set(ALEPH_PLATFORM_BSD TRUE)
  message(STATUS "[INFO] Platform: FreeBSD")
else()
  message(WARNING "[WARNING] Unknown platform: ${CMAKE_SYSTEM_NAME} - Build may fail")
endif()
find_package(SQLite3 REQUIRED)
set(header_directory
  include/aleph/
  include/aleph/sql/
  include/aleph/sql/impl/
  include/aleph/util/
  include/aleph/thread
)
set(lib_files
  src/sql_database_sqlite_lib.cc
)
add_library(aleph SHARED ${lib_files})
if(ALEPH_PLATFORM_WINDOWS)
  target_compile_definitions(aleph PRIVATE ALEPH_SQLDATABASE_WIN32)
  target_compile_definitions(aleph PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_executable(test_aleph_sql test/test_sqlite3_async_db.cc)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(
      test_aleph_sql
      PRIVATE
      aleph
    )
    target_compile_definitions(aleph PRIVATE ALEPH_SQLDATABASE_WIN32)
    target_link_libraries(aleph PRIVATE SQLite3::SQLite3)
else()
    target_link_libraries(test_aleph_sql PRIVATE aleph SQLite3::SQLite3)
endif()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/AlephSQLConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/AlephSQLConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AlephSQL
)
install(TARGETS aleph
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/aleph
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp"
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/AlephSQLConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AlephSQL
)