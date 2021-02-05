function(link_static_library_dependencies tgt_name)

   
  list(APPEND interface_includes $<TARGET_PROPERTY:${tgt_name},INTERFACE_INCLUDE_DIRECTORIES>)
  function(_recursively_collect_dependencies input_target)
	set(_input_link_libraries LINK_LIBRARIES)
	get_target_property(_input_type ${input_target} TYPE)
	if (${_input_type} STREQUAL "INTERFACE_LIBRARY")
	  set(_input_link_libraries INTERFACE_LINK_LIBRARIES)
	endif()
	get_target_property(public_dependencies ${input_target} ${_input_link_libraries})
	foreach(dependency IN LISTS public_dependencies)
	  if(TARGET ${dependency})
		get_target_property(alias ${dependency} ALIASED_TARGET)
		#TODO: Fix this:
		#list(APPEND interface_includes $<TARGET_PROPERTY:${dependency},INTERFACE_INCLUDE_DIRECTORIES>)
		#message(STATUS "$<TARGET_PROPERTY:${dependency},INTERFACE_INCLUDE_DIRECTORIES>")
		if (TARGET ${alias})
		  set(dependency ${alias})
		endif()
		get_target_property(_type ${dependency} TYPE)
		if (${_type} STREQUAL "STATIC_LIBRARY")
		  list(APPEND static_libs ${dependency})
		endif()
		#message(STATUS "Detected dependency: ${dependency}")
		#message(STATUS "\tType: ${_type}")
		get_property(library_already_added
		  GLOBAL PROPERTY _${tgt_name}_static_bundle_${dependency})
		if (NOT library_already_added)
		  set_property(GLOBAL PROPERTY _${tgt_name}_static_bundle_${dependency} ON)
		  _recursively_collect_dependencies(${dependency})
		endif()
	  else()
		#message(STATUS "Adding external dependency to bundle: ${dependency}")
		#list(APPEND static_libs ${dependency})
	  endif()
	endforeach()
	set(static_libs ${static_libs} PARENT_SCOPE)
  endfunction()

  _recursively_collect_dependencies(${tgt_name})


  list(REMOVE_DUPLICATES static_libs)
  list(REMOVE_DUPLICATES interface_includes)

  set(bundled_tgt_full_name $<TARGET_FILE:${tgt_name}>)
	foreach(tgt IN LISTS static_libs)
	  list(APPEND static_libs_full_names $<TARGET_FILE:${tgt}>)
	  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/rel_${tgt_name}_${tgt}.txt
		COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/rel_${tgt_name}_${tgt}.txt
		DEPENDS $<TARGET_FILE:${tgt}>
		 )
	   target_sources(${tgt_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/rel_${tgt_name}_${tgt}.txt)
	   set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/rel_${tgt_name}_${tgt}.txt PROPERTIES SYMBOLIC TRUE)
	endforeach()
	
	add_custom_command(TARGET ${tgt_name} PRE_LINK
		COMMAND ${CMAKE_COMMAND} -E remove -f ${bundled_tgt_full_name}
		COMMENT "Cleaning bundled lib ${bundled_tgt_full_name}"
		VERBATIM
	)

  if (CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$")
    set(ar_tool ${CMAKE_AR})
    if (CMAKE_INTERPROCEDURAL_OPTIMIZATION)
      set(ar_tool ${CMAKE_CXX_COMPILER_AR})
    endif()
	#https://stackoverflow.com/questions/3821916/how-to-merge-two-ar-static-libraries-into-one'
	set(_ar_script ${tgt_name}.ar.in)
	file(GENERATE OUTPUT ${_ar_script} CONTENT "CREATE ${bundled_tgt_full_name}\naddlib ${bundled_tgt_full_name}\n\save\nend") 
    add_custom_command(TARGET ${tgt_name} POST_BUILD
      COMMAND ${ar_tool} cqT ${bundled_tgt_full_name} ${bundled_tgt_full_name} ${static_libs_full_names}
	  COMMAND ${ar_tool} -M ${bundled_tgt_full_name} < ${_ar_script}
      #BYPRODUCTS ${bundled_tgt_full_name}
      COMMENT "Linking static library dependencies into ${tgt_name}"
      VERBATIM)
  elseif(MSVC)
	get_filename_component(_msvc_tools_path "${CMAKE_CXX_COMPILER}" DIRECTORY)
	set(LIB_CMD "${_msvc_tools_path}/lib.exe")
	add_custom_command(TARGET ${tgt_name} POST_BUILD
      COMMAND ${LIB_CMD} /NOLOGO /OUT:${bundled_tgt_full_name} ${bundled_tgt_full_name} ${static_libs_full_names}
      #BYPRODUCTS ${bundled_tgt_full_name}
      COMMENT "Bundling ${bundled_tgt_full_name}"
      VERBATIM)
  else()
    message(FATAL_ERROR "Unknown bundle scenario!")
  endif()

  
 #Related to TODO above:
 #set_target_properties(${tgt_name} 
  #  PROPERTIES 
	#INTERFACE_INCLUDE_DIRECTORIES interface_includes)

endfunction()