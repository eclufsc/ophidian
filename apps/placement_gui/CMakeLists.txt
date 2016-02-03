
find_package(OpenGL)

if(SFML_FOUND)

  include_directories(${SFML_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
  add_executable( placement_gui main.cpp application.cpp render_window.cpp GUI.cpp ${HEADERS} )
  target_link_libraries ( placement_gui LINK_PUBLIC entity standard_cell netlist placement ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
endif()



 
 


