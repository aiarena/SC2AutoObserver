add_executable(Observer)

target_sources(Observer
    PRIVATE
        CameraModule.h
        CameraModule.cpp
        Observer.cpp
        Timer.hpp
        Tools.h
        $<$<PLATFORM_ID:Windows>:ToolsWindows.cpp>
        $<$<PLATFORM_ID:Darwin>:ToolsUnix.cpp>
)

include_directories(SYSTEM
    ${PROJECT_SOURCE_DIR}/cpp-sc2/include
    ${PROJECT_SOURCE_DIR}/cpp-sc2/contrib/protobuf/src
    ${PROJECT_BINARY_DIR}/cpp-sc2/generated
    "."
)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX-")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything \
        -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded \
        -Wno-switch-enum -Wno-weak-vtables -Wno-exit-time-destructors \
        -Wno-float-equal -Wno-global-constructors"
    )
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

link_directories(${PROJECT_BINARY_DIR}/cpp-sc2/bin)

target_link_libraries(Observer
    sc2api sc2utils
)

if (APPLE)
    target_link_libraries(Observer "-framework Carbon")
endif ()
