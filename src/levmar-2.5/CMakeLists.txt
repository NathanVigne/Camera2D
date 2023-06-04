set(LAPACK_LIB ${PROJECT_SOURCE_DIR}/src/levmar-2.5/LAPACK/x64/liblapack.lib)
set(BLAS_LIB ${PROJECT_SOURCE_DIR}/src/levmar-2.5/BLAS/x64/libblas.lib)

add_library(levmar STATIC
  ${LEVMAR_PATH}lm.c
  ${LEVMAR_PATH}Axb.c
  ${LEVMAR_PATH}misc.c
  ${LEVMAR_PATH}lmlec.c
  ${LEVMAR_PATH}lmbc.c
  ${LEVMAR_PATH}lmblec.c
  ${LEVMAR_PATH}lmbleic.c
  ${LEVMAR_PATH}levmar.h
  ${LEVMAR_PATH}misc.h
  ${LEVMAR_PATH}compiler.h
)

target_link_libraries(levmar ${LAPACK_LIB} ${BLAS_LIB})
