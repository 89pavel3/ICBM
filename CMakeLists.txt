cmake_minimum_required(VERSION 3.10.0)

project(ICBM CXX)
set(CMAKE_CXX_STANDARD 17)
include(cmake/StandardProjectSettings.cmake)

add_library(default INTERFACE)
target_compile_features(default INTERFACE cxx_std_17)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(default)
get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
set(ENV_FOR_C CC)
set(ENV_FOR_CXX CXX)
set(ENV_FOR_Fortran FC)
foreach(language ${languages})
  if(ENV_FOR_${language})
    list(APPEND ENV "${ENV_FOR_${language}}=${CMAKE_${language}_COMPILER}")
  else()
    list(APPEND ENV "${language}=${CMAKE_${language}_COMPILER}")
  endif()
endforeach()


include(cmake/conan.cmake)
conan_cmake_run(
  CONANFILE
  conanfile.txt
  BASIC_SETUP
  BUILD
  missing
  ENV
  CONAN_CMAKE_PROGRAM="${CMAKE_COMMAND}"
  ${ENV}
)

enable_testing()
add_subdirectory(external/doctest)
include(external/doctest/doctest.cmake)


set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_INLINE_SOURCES YES)
set(DOXYGEN_HTML_EXTRA_STYLESHEET doxygen-awesome.css)
find_package(Doxygen)
doxygen_add_docs(Doxygen src)

add_subdirectory(src)

