#ifndef ULMBLAS_SRC_LEVEL1EXTENSIONS_GECOPY_H
#define ULMBLAS_SRC_LEVEL1EXTENSIONS_GECOPY_H 1

namespace ulmBLAS {

template <typename IndexType, typename MX, typename MY>
    void
    gecopy(IndexType      m,
           IndexType      n,
           const MX       *X,
           IndexType      incRowX,
           IndexType      incColX,
           MY             *Y,
           IndexType      incRowY,
           IndexType      incColY);

} // namespace ulmBLAS

#endif // ULMBLAS_SRC_LEVEL1EXTENSIONS_GECOPY_H 1

#include <src/level1extensions/gecopy.tcc>