project(CryptoPP)

add_library(Crypto_LIB STATIC IMPORTED GLOBAL)

set(Crypto_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "" FORCE)
set_target_properties(Crypto_LIB PROPERTIES
        IMPORTED_LOCATION ${Crypto_INCLUDE_DIR}/lib/libcryptopp.a)