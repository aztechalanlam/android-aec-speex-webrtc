cmake_minimum_required(VERSION 3.10.2)

project("speex_native_rebuild")

# Enable standard optimization macros for Android ARM architecture performance
add_definitions(-DFIXED_POINT -DUSE_KISS_FFT -DHAVE_CONFIG_H)

# Setup search scopes for cross-library header resolution
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ==========================================
# 1. BUILD TARGET: libspeex.so (Codec Suite)
# ==========================================
set(SPEEX_CODEC_SRCS
        libspeex/bits.c
        libspeex/cb_search.c
        libspeex/exc_8_128_table.c
        libspeex/exc_10_32_table.c
        libspeex/exc_10_16_table.c
        libspeex/exc_20_32_table.c
        libspeex/exc_5_256_table.c
        libspeex/exc_5_64_table.c
        libspeex/gain_table.c
        libspeex/gain_table_lbr.c
        #libspeex/he_cksum.c
        libspeex/hexc_table.c
        libspeex/hexc_10_32_table.c
        libspeex/filters.c
        libspeex/vq.c
        libspeex/high_lsp_tables.c
        libspeex/lpc.c
        libspeex/lsp.c
        libspeex/lsp_tables_nb.c
        libspeex/ltp.c
        libspeex/modes.c
        libspeex/modes_wb.c
        libspeex/nb_celp.c
        libspeex/quant_lsp.c
        libspeex/sb_celp.c
        libspeex/speex.c
        libspeex/speex_callbacks.c
        libspeex/speex_header.c
        libspeex/window.c
        # If your version includes VBR / Stereo, append those compilation targets:
        libspeex/vbr.c
        libspeex/stereo.c
        )

add_library(speex SHARED ${SPEEX_CODEC_SRCS})
target_link_libraries(speex log)

# ==========================================
# 2. BUILD TARGET: libspeex_aec.so (Echo Cancellation Suite)
# ==========================================
set(SPEEX_AEC_SRCS
        speex_aec_interface.c           # Repository's JNI interface glue
        libspeexdsp/mdf.c     # Multi-Delay Block Frequency Domain Adaptive Filter
        libspeexdsp/fftwrap.c
        libspeexdsp/kiss_fft.c
        libspeexdsp/kiss_fftr.c
        libspeexdsp/preprocess.c
        libspeexdsp/filterbank.c
        )

add_library(speex_aec SHARED ${SPEEX_AEC_SRCS})
# Link against log and the sibling codec library if dependent
target_link_libraries(speex_aec speex log)

# Redirect where SHARED (.so) libraries are placed
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

# Redirect where STATIC (.a) libraries are placed
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
set(DISTRIBUTION_DIR "${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}")



add_custom_command(
        TARGET speex_aec POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DISTRIBUTION_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:speex_aec>" "${DISTRIBUTION_DIR}/"
        COMMENT "Copying build output to distribution folder..."
)

add_custom_command(
        TARGET speex POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DISTRIBUTION_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:speex>" "${DISTRIBUTION_DIR}/"
        COMMENT "Copying build output to distribution folder..."
)
