# Configuring using ly ccip 80mm

exists( ../../include/lib_ly_ccip/lib_ly_ccip.lib ) {
    exists( ../../include/lib_ly_ccip/ly_ccip.h ){
        message( "Configuring using ly ccip 80mm ..." )
        DEFINES += USING_LY_CCIP_80MM_DEFINE

        INCLUDEPATH += ../../include/lib_ly_ccip

        CONFIG(debug, debug|release) {
            LIBS += -llib_ly_ccipD -L../../include/lib_ly_ccip
        } else {
            LIBS += -llib_ly_ccip -L../../include/lib_ly_ccip
        }
    }
}
