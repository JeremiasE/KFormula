set(karboncommon_LIB_SRCS 
    KarbonDocument.cpp
    KarbonGlobal.cpp
    KarbonCursor.cpp
    KarbonGradientHelper.cpp
    KarbonOutlinePaintingStrategy.cpp

    commands/KarbonLayerReorderCommand.cpp
    commands/KarbonBooleanCommand.cpp
)

kde4_add_library( karboncommon SHARED ${karboncommon_LIB_SRCS} )

target_link_libraries( karboncommon ${QT_QT3SUPPORT_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS} flake kostore komain kotext )

set_target_properties(karboncommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS karboncommon  ${INSTALL_TARGETS_DEFAULT_ARGS})
