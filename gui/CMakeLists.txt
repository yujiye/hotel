set(SRC
    datasourcechangeintegrator.cpp
    statusbar.cpp
    planningwidget.cpp

    dialogs/editreservation.cpp

    planningwidget/context.cpp
    planningwidget/datebarwidget.cpp
    planningwidget/planningboardlayout.cpp
    planningwidget/planningboardreservationitem.cpp
    planningwidget/planningboardwidget.cpp
    planningwidget/reservationrenderer.cpp
    planningwidget/roomlistwidget.cpp
    planningwidget/tool.cpp
    planningwidget/newreservationtool.cpp
)

set(SRC_INCLUDES
    datasourcechangeintegrator.h
    datastreamobserveradapter.h
    statusbar.h
    planningwidget.h

    dialogs/editreservation.h

    planningwidget/context.h
    planningwidget/datebarwidget.h
    planningwidget/planningboardlayout.h
    planningwidget/planningboardreservationitem.h
    planningwidget/planningboardwidget.h
    planningwidget/reservationrenderer.h
    planningwidget/roomlistwidget.h
    planningwidget/tool.h
    planningwidget/newreservationtool.h
)

set(CMAKE_AUTOMOC ON)
add_library(hotel_gui ${SRC} ${SRC_INCLUDES})
target_link_libraries(hotel_gui hotel Qt5::Widgets)
