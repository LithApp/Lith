# This may be a candidate for QT_GLOBAL_LRELEASE_TARGET but it works so...
add_custom_target(TranslationTarget)

set(COMPILED_TRANSLATIONS_DIR "${CMAKE_BINARY_DIR}/app")

set(GENERATED_TRANSLATION_PARTS_DIR "${CMAKE_BINARY_DIR}/translations_tmp")
file(MAKE_DIRECTORY "${GENERATED_TRANSLATION_PARTS_DIR}")

set(COMPLETED_TRANSLATION_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# If no translations exist, generate at least the default english file
set(default_translation_file_path "${COMPLETED_TRANSLATION_SOURCES_DIR}/Lith_en.ts")
file(GLOB translation_files
    CONFIGURE_DEPENDS
    ${COMPLETED_TRANSLATION_SOURCES_DIR}/*.ts
)
if(NOT "${default_translation_file_path}" IN_LIST translation_files)
    list(APPEND translation_files "${default_translation_file_path}")
endif()

file(GLOB generated_temporary_translations
    CONFIGURE_DEPENDS
    ${GENERATED_TRANSLATION_PARTS_DIR}/*.ts
)

# This didn't work with glob because the files got created during build, TODO sometimes maybe
set(generated_temporary_translations
    ${GENERATED_TRANSLATION_PARTS_DIR}/Lith_partial_App_en.ts
    ${GENERATED_TRANSLATION_PARTS_DIR}/Lith_partial_Core_en.ts
    ${GENERATED_TRANSLATION_PARTS_DIR}/Lith_partial_Style_en.ts
    ${GENERATED_TRANSLATION_PARTS_DIR}/Lith_partial_UI_en.ts
)

# Install the composed source files back in the source tree to not accidentally delete them
# I'll see if this is what I actually want. Using a separate repo or some service sounds more reasonable...
qt_add_lupdate(TranslationTarget
    TS_FILES ${translation_files}
    SOURCES "${generated_temporary_translations}"
)
# We need to explicitely depend on the other targets so they're updated before we start composing the big final one
add_dependencies(TranslationTarget_lupdate Lith_lupdate LithCore_lupdate LithStyle_lupdate LithUI_lupdate)

message("Translation files: ${translation_files}")
message("generated_temporary_translations files: ${generated_temporary_translations}")
set_source_files_properties(${generated_temporary_translations} PROPERTIES
    OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}"
)
qt_add_lrelease(TranslationTarget
    TS_FILES ${translation_files}
    QM_FILES_OUTPUT_VARIABLE translations_compiled_files
)
message("translations_compiled_files files: ${translations_compiled_files}")
message("QM will be here: ${translations_compiled_files}")

qt_add_resources(Lith "translations"
    PREFIX "/i18n/translations"
    BASE "${CMAKE_CURRENT_BINARY_DIR}"
    FILES "${translations_compiled_files}")

install(FILES ${translations_compiled_files} DESTINATION "translations")
