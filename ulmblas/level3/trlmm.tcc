#ifndef ULMBLAS_LEVEL3_TRLMM_TCC
#define ULMBLAS_LEVEL3_TRLMM_TCC 1

#include <ulmblas/auxiliary/memorypool.h>
#include <ulmblas/config/blocksize.h>
#include <ulmblas/level1extensions/gescal.h>
#include <ulmblas/level3/mkernel/mgemm.h>
#include <ulmblas/level3/mkernel/mtrlmm.h>
#include <ulmblas/level3/ukernel/ugemm.h>
#include <ulmblas/level3/pack/gepack.h>
#include <ulmblas/level3/pack/trlpack.h>
#include <ulmblas/level3/trlmm.h>

namespace ulmBLAS {

template <typename IndexType, typename Alpha, typename TA, typename TB>
void
trlmm(IndexType    m,
      IndexType    n,
      const Alpha  &alpha,
      bool         conjA,
      bool         unitDiag,
      const TA     *A,
      IndexType    incRowA,
      IndexType    incColA,
      TB           *B,
      IndexType    incRowB,
      IndexType    incColB)
{
    typedef decltype(Alpha(0)*TA(0)*TB(0))  T;

    const IndexType MC = BlockSize<T>::MC;
    const IndexType NC = BlockSize<T>::NC;

    const IndexType MR = BlockSizeUGemm<T>::MR;
    const IndexType NR = BlockSizeUGemm<T>::NR;

    const IndexType mb = (m+MC-1) / MC;
    const IndexType nb = (n+NC-1) / NC;

    const IndexType mc_ = m % MC;
    const IndexType nc_ = n % NC;

    static MemoryPool<T> memoryPool;

    if (alpha==Alpha(0)) {
        gescal(m, n, Alpha(0), B, incRowB, incColB);
        return;
    }

    T  *A_ = memoryPool.allocate(MC*MC+MR);
    T  *B_ = memoryPool.allocate(MC*NC+NR);

    for (IndexType j=0; j<nb; ++j) {
        IndexType nc = (j!=nb-1 || nc_==0) ? NC : nc_;

        for (IndexType l=mb-1; l>=0; --l) {
            IndexType kc = (l!=mb-1 || mc_==0) ? MC   : mc_;

            gepack_B(kc, nc, false,
                     &B[l*MC*incRowB+j*NC*incColB], incRowB, incColB,
                     B_);

            trlpack(kc, conjA, unitDiag,
                    &A[l*MC*(incRowA+incColA)], incRowA, incColA,
                    A_);

            mtrlmm(kc, nc, alpha, A_, B_,
                   &B[l*MC*incRowB+j*NC*incColB], incRowB, incColB);

            for (IndexType i=l+1; i<mb; ++i) {
                IndexType mc = (i!=mb-1 || mc_==0) ? MC : mc_;

                gepack_A(mc, kc, conjA,
                         &A[i*MC*incRowA+l*MC*incColA], incRowA, incColA,
                         A_);

                mgemm(mc, nc, kc, alpha, A_, B_, T(1),
                      &B[i*MC*incRowB+j*NC*incColB], incRowB, incColB);
            }
        }
    }

    memoryPool.release(A_);
    memoryPool.release(B_);
}

} // namespace ulmBLAS

#endif // ULMBLAS_LEVEL3_TRLMM_TCC
