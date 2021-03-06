find_package(Doxygen 1.8.4 QUIET)

if( DOXYGEN_FOUND )
  if( DOXYGEN_DOT_FOUND )

    set(userdoc_doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.txt)
    set(userdoc_tarball
      ${DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY}/NorMIT-Pla.tar.gz
      )

    configure_file(Doxyfile.txt.in ${userdoc_doxyfile})

    # Generate doxygen documentation
    add_custom_target(normit-cpp-doc
      COMMAND ${DOXYGEN_EXECUTABLE} ${userdoc_doxyfile}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )

    add_dependencies(normit-doc normit-cpp-doc)

    add_custom_target(normit-cpp-doc-tarball-clean
      COMMAND ${CMAKE_COMMAND} -E remove ${userdoc_tarball}
      )

    add_custom_target(normit-cpp-doc-tarball
      DEPENDS normit-cpp-doc normit-cpp-doc-tarball-clean
      COMMAND ${CMAKE_COMMAND} -E tar cfz ${userdoc_tarball} html
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
    add_dependencies(normit-doc-tarballs normit-cpp-doc-tarball)

    set_target_properties(normit-cpp-doc normit-cpp-doc-tarball
      PROPERTIES LABELS Documentation)

    # Need to add EXCLUDE_FROM_DEFAULT_BUILD otherwise these targets would be built each time when build is started
    # in the Visual Studio IDE (regardless of DOCUMENTATION_TARGET_IN_ALL setting).
    set_target_properties(normit-cpp-doc PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)
    set_target_properties(normit-cpp-doc-tarball-clean PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)
    set_target_properties(normit-cpp-doc-tarball PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)

    set( NorMIT-Plan_ADDITIONAL_FILE_PATTERN "*.c \  *.cc \  *.cxx \  *.cpp \  *.c++ \  *.hxx \  *.hpp \  *.h++ \  *.txx \  *.C \  *.CC \  *.C++ \  *.moc") 

    configure_file( Doxyfile.txt.in
      ${CMAKE_CURRENT_BINARY_DIR}/DevDoxyfile.txt
      )

  else( DOXYGEN_DOT_FOUND )
    message( WARNING
      "Warning: Dot not found - Documentation will not be created"
      )
  endif( DOXYGEN_DOT_FOUND )
endif( DOXYGEN_FOUND )
