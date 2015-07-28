project(ucl-study-manager)

# Minimal CMAKE version
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5 5.2.0 COMPONENTS Core Concurrent DBus Widgets WebKit WebKitWidgets REQUIRED)

set(UCL_STUDY_VERSION_MAJOR 0)
set(UCL_STUDY_VERSION_MINOR 1)
set(UCL_STUDY_VERSION_PATCH 0)
set(UCL_STUDY_VERSION "${UCL_STUDY_VERSION_MAJOR}.${UCL_STUDY_VERSION_MINOR}.${UCL_STUDY_VERSION_PATCH}")

set(CPACK_PACKAGE_VERSION_MAJOR ${QZEITGEIST_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${QZEITGEIST_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${QZEITGEIST_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ucl-study-manager")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CMAKE_INSTALL_PREFIX "/")

#use CPack for packaging code up
include(CPack)

set(ucl_study_manager_HEADERS
    study.h 
    Lib/tarball.h 
    Model/step.h 
    Model/part.h 
    Model/participant.h 
    Model/uploadjob.h 
    Services/webviewservice.h 
    Services/requestservice.h 
    Services/progressreportservice.h 
    Services/uploadservice.h 
    UI/mainwindow.h 
    Widgets/progressbutton.h 
    Widgets/progressbarbutton.h
)

set(ucl_study_manager_SRCS
    main.cpp
    study.cpp 
    Lib/tarball.cpp 
    Model/step.cpp 
    Model/part.cpp 
    Model/participant.cpp 
    Model/uploadjob.cpp 
    Services/webviewservice.cpp 
    Services/requestservice.cpp 
    Services/progressreportservice.cpp 
    Services/uploadservice.cpp 
    UI/mainwindow.cpp 
    Widgets/progressbutton.cpp 
    Widgets/progressbarbutton.cpp
)

set(ucl_study_manager_UIS
    mainwindow.ui
    uploadingtoolbarform.ui
)

SET(ucl_study_manager_RESOURCES
    icons.qrc
)

qt5_wrap_ui(ucl_UIS_H ${ucl_study_manager_UIS})
qt5_add_resources(ucl_study_manager_RESOURCES_RCC ${ucl_study_manager_RESOURCES})


add_executable(ucl-study-manager ${ucl_study_manager_SRCS} ${ucl_UIS_H} ${ucl_study_manager_RESOURCES_RCC} ${ucl_study_manager_HEADERS})
target_link_libraries(ucl-study-manager qzeitgeist5 ${QT_QTDBUS_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QDBUS_LDFLAGS})
qt5_use_modules(ucl-study-manager Core DBus Test Concurrent Widgets WebKitWidgets)

install(FILES ${CMAKE_SOURCE_DIR}/data/ucl-study-manager.desktop DESTINATION usr/share/applications/)
install(FILES ${CMAKE_SOURCE_DIR}/data/ucl-study-manager.png DESTINATION usr/share/pixmaps/)
install(FILES ${CMAKE_SOURCE_DIR}/data/multitasking.conf DESTINATION etc/xdg/UCL/)
install(TARGETS ucl-study-manager DESTINATION usr/bin)

