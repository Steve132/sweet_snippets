#This is a tiny cmake library that allows you to add resource files (like shaders or textures) to a build

function(add_resources RESTARGETNAME)
	if (NOT ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
		foreach(RESOURCE ${ARGN})
			set(RSRC "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE}")
			set(RDST "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE}")
			add_custom_command(
				OUTPUT ${RDST}
				COMMAND ${CMAKE_COMMAND} -E copy ${RSRC} ${RDST}
				DEPENDS ${RSRC}
				COMMENT "Now copying ${RSRC} to ${RDST}")
		endforeach(RESOURCE)
	endif()
	add_custom_target(${RESTARGETNAME} DEPENDS ${ARGN})
endfunction(add_resources)

function(target_link_resources TARGETNAME RESTARGETNAME)
	add_dependencies(${TARGETNAME} ${RESTARGETNAME})
endfunction()


#Example usage
#add_executable(mycode code.cpp)
#add_resources(mycode_shaders glsl/shader1.vs glsl/shader2.fs)
#target_link_resources(mycode mycode_shaders)
