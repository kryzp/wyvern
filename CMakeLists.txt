cmake_minimum_required(VERSION 3.7)
project(wyvern)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME}

	public/wvn/root.cpp
	public/wvn/camera.cpp
	public/wvn/time.cpp

	public/wvn/util/types.cpp

	public/wvn/devenv/log_mgr.cpp
	public/wvn/devenv/console.cpp
	public/wvn/devenv/profiler.cpp

	public/wvn/actor/actor.cpp
	public/wvn/actor/actor_mgr.cpp
	public/wvn/actor/event.cpp
	public/wvn/actor/event_mgr.cpp
	public/wvn/actor/scene_mgr.cpp

	public/wvn/audio/audio_mgr.cpp
	public/wvn/audio/sound.cpp

	public/wvn/input/input_mgr.cpp
	public/wvn/input/v_key.cpp

	public/wvn/animation/animation_mgr.cpp

	public/wvn/graphics/rendering_mgr.cpp
	public/wvn/graphics/mesh.cpp
	public/wvn/graphics/mesh_mgr.cpp
	public/wvn/graphics/texture.cpp
	public/wvn/graphics/texture_mgr.cpp
	public/wvn/graphics/shader.cpp
	public/wvn/graphics/shader_mgr.cpp
	public/wvn/graphics/render_target.cpp
	public/wvn/graphics/font.cpp
	public/wvn/graphics/image.cpp
	public/wvn/graphics/material.cpp
	public/wvn/graphics/gpu_buffer.cpp
	public/wvn/graphics/gpu_buffer_mgr.cpp

	public/wvn/network/network_mgr.cpp
	public/wvn/network/ip_address.cpp
	public/wvn/network/tcp_socket.cpp
	public/wvn/network/udp_socket.cpp
	public/wvn/network/tcp_listener.cpp

	public/wvn/io/endian.cpp
	public/wvn/io/stream.cpp
	public/wvn/io/file_stream.cpp
	public/wvn/io/memory_stream.cpp

	public/wvn/physics/physics_mgr.cpp
	public/wvn/physics/rigidbody.cpp

	public/wvn/resource/resource_mgr.cpp

	public/wvn/maths/circle.cpp
	public/wvn/maths/colour.cpp
	public/wvn/maths/complex.cpp
	public/wvn/maths/line.cpp
	public/wvn/maths/mat3x2.cpp
	public/wvn/maths/mat3x3.cpp
	public/wvn/maths/mat4x3.cpp
	public/wvn/maths/mat4x4.cpp
	public/wvn/maths/polygon.cpp
	public/wvn/maths/quad.cpp
	public/wvn/maths/quaternion.cpp
	public/wvn/maths/timer.cpp
	public/wvn/maths/triangle.cpp

	public/wvn/plugin/plugin_loader.cpp
	public/wvn/plugin/system/sdl2_plugin.cpp
	public/wvn/plugin/graphics/vulkan_plugin.cpp
	public/wvn/plugin/graphics/opengl_plugin.cpp
	public/wvn/plugin/audio/openal_plugin.cpp

	private/backend/system/sdl2/sdl2_backend.cpp

	private/backend/graphics/vulkan/vk_backend.cpp
	private/backend/graphics/vulkan/vk_buffer.cpp
	private/backend/graphics/vulkan/vk_buffer_mgr.cpp
	private/backend/graphics/vulkan/vk_util.cpp
	private/backend/graphics/vulkan/vk_texture.cpp
	private/backend/graphics/vulkan/vk_texture_mgr.cpp
	private/backend/graphics/vulkan/vk_texture_sampler.cpp
	private/backend/graphics/vulkan/vk_shader_mgr.cpp

	private/backend/graphics/opengl/opengl_backend.cpp

	private/backend/audio/openal/openal_backend.cpp
)

target_include_directories(${PROJECT_NAME}

	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/public

	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/private
)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

set(SDL2_ENABLED true CACHE BOOL "Use SDL2 as the system implementation")
set(VK_ENABLED true CACHE BOOL "Use Vulkan as the renderer implementation")
set(OPENGL_ENABLED false CACHE BOOL "Use OpenGL as the renderer implementation")
set(OPENAL_ENABLED true CACHE BOOL "Use OpenAL as the audio implementation")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_definitions(WVN_DEBUG)

if (SDL2_ENABLED)
	add_compile_definitions(WVN_USE_SDL2)
	target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
endif()

if (VK_ENABLED)
	add_compile_definitions(WVN_USE_VULKAN)
	target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
endif()

if (OPENGL_ENABLED)
	add_compile_definitions(WVN_USE_OPENGL)
	# todo: link opengl
endif()

if (OPENAL_ENABLED)
	add_compile_definitions(WVN_USE_OPENAL)
endif()

add_executable(wvn_test test/src/main.cpp)
target_include_directories(wvn_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)
target_link_libraries(wvn_test PUBLIC ${PROJECT_NAME})

add_custom_command(TARGET wvn_test
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/test/res/ $<TARGET_FILE_DIR:wvn_test>/res
)
