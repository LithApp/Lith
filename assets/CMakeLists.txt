
set(LITH_RESOURCES
    assets.qrc
)

qt_add_library(LithAssets STATIC
    ${LITH_RESOURCES}
)

# Needed to link in Linux
set_property(TARGET LithAssets PROPERTY POSITION_INDEPENDENT_CODE ON)
