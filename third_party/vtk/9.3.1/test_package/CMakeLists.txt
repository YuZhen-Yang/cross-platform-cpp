cmake_minimum_required(VERSION 3.19)
project(VTKTest CXX)

find_package(VTK REQUIRED COMPONENTS CommonCore GUISupportQt);

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES}
)