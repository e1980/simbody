#ifndef SimTK_SIMMATRIX_SMALLMATRIX_MIXED_H_
#define SimTK_SIMMATRIX_SMALLMATRIX_MIXED_H_

/* -------------------------------------------------------------------------- *
 *                      SimTK Core: SimTK Simmatrix(tm)                       *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK Core biosimulation toolkit originating from      *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2005-9 Stanford University and the Authors.         *
 * Authors: Michael Sherman                                                   *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

/**@file
 * This file defines global functions and class members which use a mix 
 * of Vec, Row, and Mat types and hence need to wait until everything is 
 * defined. Some of them may depend on Lapack also.
 */

namespace SimTK {

    // DOT PRODUCT

// Dot product and corresponding infix operator*(). Note that
// the '*' operator is just a matrix multiply so is strictly 
// row*column to produce a scalar (1x1) result.
//
// In keeping with Matlab precedent, however, the explicit dot()
// function is defined on two column vectors
// v and w such that dot(v,w)= ~v * w. That means we use the
// Hermitian transpose of the elements of v, and the elements of
// w unchanged. If v and/or w are rows, we first convert them
// to vectors via *positional* transpose. So no matter what shape
// these have on the way in it is always the Hermitian transpose
// (or complex conjugate for scalars) of the first item times
// the unchanged elements of the second item.


template <int M, class E1, int S1, class E2, int S2> inline
typename CNT<typename CNT<E1>::THerm>::template Result<E2>::Mul 
dot(const Vec<M,E1,S1>& r, const Vec<M,E2,S2>& v) {
    typename CNT<typename CNT<E1>::THerm>::template Result<E2>::Mul sum(dot(reinterpret_cast<const Vec<M-1,E1,S1>&>(r), reinterpret_cast<const Vec<M-1,E2,S2>&>(v)) + CNT<E1>::transpose(r[M-1])*v[M-1]);
    return sum;
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<typename CNT<E1>::THerm>::template Result<E2>::Mul 
dot(const Vec<1,E1,S1>& r, const Vec<1,E2,S2>& v) {
    typename CNT<typename CNT<E1>::THerm>::template Result<E2>::Mul sum(CNT<E1>::transpose(r[0])*v[0]);
    return sum;
}

// dot product (row * conforming vec)
template <int N, class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
operator*(const Row<N,E1,S1>& r, const Vec<N,E2,S2>& v) {
    typename CNT<E1>::template Result<E2>::Mul sum(reinterpret_cast<const Row<N-1,E1,S1>&>(r)*reinterpret_cast<const Vec<N-1,E2,S2>&>(v) + r[N-1]*v[N-1]);
    return sum;
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
operator*(const Row<1,E1,S1>& r, const Vec<1,E2,S2>& v) {
    typename CNT<E1>::template Result<E2>::Mul sum(r[0]*v[0]);
    return sum;
}

// Alternate acceptable forms for dot().
template <int N, class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
dot(const Row<N,E1,S1>& r, const Vec<N,E2,S2>& v) {
    return dot(r.positionalTranspose(),v);
}
template <int M, class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
dot(const Vec<M,E1,S1>& v, const Row<M,E2,S2>& r) {
    return dot(v,r.positionalTranspose());
}
template <int N, class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul 
dot(const Row<N,E1,S1>& r, const Row<N,E2,S2>& s) {
    return dot(r.positionalTranspose(),s.positionalTranspose());
}

    // OUTER PRODUCT

// Outer product and corresponding infix operator*(). Note that
// the '*' operator is just a matrix multiply so is strictly 
// column_mX1 * row_1Xm to produce an mXm matrix result.
//
// Although Matlab doesn't provide outer(), to be consistent
// we'll treat it as discussed for dot() above. That is, outer
// is defined on two column vectors
// v and w such that outer(v,w)= v * ~w. That means we use the
// elements of v unchanged but use the Hermitian transpose of
// the elements of w. If v and/or w are rows, we first convert them
// to vectors via *positional* transpose. So no matter what shape
// these have on the way in it is always the unchanged elements of
// the first item times the Hermitian transpose (or complex
// conjugate for scalars) of the elements of the second item.

template <int M, class E1, int S1, class E2, int S2> inline
Mat<M,M, typename CNT<E1>::template Result<typename CNT<E2>::THerm>::Mul>
outer(const Vec<M,E1,S1>& v, const Vec<M,E2,S2>& w) {
    Mat<M,M, typename CNT<E1>::template Result<typename CNT<E2>::THerm>::Mul> m;
    for (int i=0; i<M; ++i)
        m[i] = v[i] * ~w;
    return m;
}

// This is the general conforming case of Vec*Row (outer product)
template <int M, class E1, int S1, class E2, int S2> inline
typename Vec<M,E1,S1>::template Result<Row<M,E2,S2> >::Mul
operator*(const Vec<M,E1,S1>& v, const Row<M,E2,S2>& r) {
    return Vec<M,E1,S1>::template Result<Row<M,E2,S2> >::MulOp::perform(v,r);
}


// Alternate acceptable forms for outer().
template <int M, class E1, int S1, class E2, int S2> inline
Mat<M,M, typename CNT<E1>::template Result<E2>::Mul>
outer(const Vec<M,E1,S1>& v, const Row<M,E2,S2>& r) {
    return outer(v,r.positionalTranspose());
}
template <int M, class E1, int S1, class E2, int S2> inline
Mat<M,M, typename CNT<E1>::template Result<E2>::Mul>
outer(const Row<M,E1,S1>& r, const Vec<M,E2,S2>& v) {
    return outer(r.positionalTranspose(),v);
}
template <int M, class E1, int S1, class E2, int S2> inline
Mat<M,M, typename CNT<E1>::template Result<E2>::Mul>
outer(const Row<M,E1,S1>& r, const Row<M,E2,S2>& s) {
    return outer(r.positionalTranspose(),s.positionalTranspose());
}

    // MAT*VEC, ROW*MAT (conforming)

// vec = mat * vec (conforming)
template <int M, int N, class ME, int CS, int RS, class E, int S> inline
typename Mat<M,N,ME,CS,RS>::template Result<Vec<N,E,S> >::Mul
operator*(const Mat<M,N,ME,CS,RS>& m,const Vec<N,E,S>& v) {
    typename Mat<M,N,ME,CS,RS>::template Result<Vec<N,E,S> >::Mul result;
    for (int i=0; i<M; ++i)
        result[i] = m[i]*v;
    return result;
}

// row = row * mat (conforming)
template <int M, class E, int S, int N, class ME, int CS, int RS> inline
typename Row<M,E,S>::template Result<Mat<M,N,ME,CS,RS> >::Mul
operator*(const Row<M,E,S>& r, const Mat<M,N,ME,CS,RS>& m) {
    typename Row<M,E,S>::template Result<Mat<M,N,ME,CS,RS> >::Mul result;
    for (int i=0; i<N; ++i)
        result[i] = r*m(i);
    return result;
}

    // NONCONFORMING MULTIPLY

    // Nonconforming: Vec on left: v*r v*m v*sym v*v
    // Result has the shape of the "most composite" (deepest) argument.

// elementwise multiply (vec * nonconforming row)
template <int M, class E1, int S1, int N, class E2, int S2> inline
typename Vec<M,E1,S1>::template Result<Row<N,E2,S2> >::MulNon
operator*(const Vec<M,E1,S1>& v, const Row<N,E2,S2>& r) {
    return Vec<M,E1,S1>::template Result<Row<N,E2,S2> >::MulOpNonConforming::perform(v,r);
}
// elementwise multiply (vec * nonconforming mat)
template <int M, class E1, int S1, int MM, int NN, class E2, int CS2, int RS2> inline
typename Vec<M,E1,S1>::template Result<Mat<MM,NN,E2,CS2,RS2> >::MulNon
operator*(const Vec<M,E1,S1>& v, const Mat<MM,NN,E2,CS2,RS2>& m) {
    return Vec<M,E1,S1>::template Result<Mat<MM,NN,E2,CS2,RS2> >
                ::MulOpNonConforming::perform(v,m);
}
// elementwise multiply (vec * nonconforming symmat)
template <int M, class E1, int S1, int MM, class E2, int RS2> inline
typename Vec<M,E1,S1>::template Result<SymMat<MM,E2,RS2> >::MulNon
operator*(const Vec<M,E1,S1>& v, const SymMat<MM,E2,RS2>& m) {
    return Vec<M,E1,S1>::template Result<SymMat<MM,E2,RS2> >
                ::MulOpNonConforming::perform(v,m);
}
// elementwise multiply (vec * nonconforming vec)
template <int M, class E1, int S1, int MM, class E2, int S2> inline
typename Vec<M,E1,S1>::template Result<Vec<MM,E2,S2> >::MulNon
operator*(const Vec<M,E1,S1>& v1, const Vec<MM,E2,S2>& v2) {
    return Vec<M,E1,S1>::template Result<Vec<MM,E2,S2> >
                ::MulOpNonConforming::perform(v1,v2);
}

    // Nonconforming: Row on left -- r*v r*r r*m r*sym


// (row or mat) = row * mat (nonconforming)
template <int M, class E, int S, int MM, int NN, class ME, int CS, int RS> inline
typename Row<M,E,S>::template Result<Mat<MM,NN,ME,CS,RS> >::MulNon
operator*(const Row<M,E,S>& r, const Mat<MM,NN,ME,CS,RS>& m) {
    return Row<M,E,S>::template Result<Mat<MM,NN,ME,CS,RS> >
        ::MulOpNonConforming::perform(r,m);
}
// (row or vec) = row * vec (nonconforming)
template <int N, class E1, int S1, int M, class E2, int S2> inline
typename Row<N,E1,S1>::template Result<Vec<M,E2,S2> >::MulNon
operator*(const Row<N,E1,S1>& r, const Vec<M,E2,S2>& v) {
    return Row<N,E1,S1>::template Result<Vec<M,E2,S2> >
        ::MulOpNonConforming::perform(r,v);
}
// (row1 or row2) = row1 * row2(nonconforming)
template <int N1, class E1, int S1, int N2, class E2, int S2> inline
typename Row<N1,E1,S1>::template Result<Row<N2,E2,S2> >::MulNon
operator*(const Row<N1,E1,S1>& r1, const Row<N2,E2,S2>& r2) {
    return Row<N1,E1,S1>::template Result<Row<N2,E2,S2> >
        ::MulOpNonConforming::perform(r1,r2);
}

    // Nonconforming: Mat on left -- m*v m*r m*sym

// (mat or vec) = mat * vec (nonconforming)
template <int M, int N, class ME, int CS, int RS, int MM, class E, int S> inline
typename Mat<M,N,ME,CS,RS>::template Result<Vec<MM,E,S> >::MulNon
operator*(const Mat<M,N,ME,CS,RS>& m,const Vec<MM,E,S>& v) {
    return Mat<M,N,ME,CS,RS>::template Result<Vec<MM,E,S> >
        ::MulOpNonConforming::perform(m,v);
}
// (mat or row) = mat * row (nonconforming)
template <int M, int N, class ME, int CS, int RS, int NN, class E, int S> inline
typename Mat<M,N,ME,CS,RS>::template Result<Row<NN,E,S> >::MulNon
operator*(const Mat<M,N,ME,CS,RS>& m,const Row<NN,E,S>& r) {
    return Mat<M,N,ME,CS,RS>::template Result<Row<NN,E,S> >
        ::MulOpNonConforming::perform(m,r);
}

// (mat or sym) = mat * sym (nonconforming)
template <int M, int N, class ME, int CS, int RS, int Dim, class E, int S> inline
typename Mat<M,N,ME,CS,RS>::template Result<SymMat<Dim,E,S> >::MulNon
operator*(const Mat<M,N,ME,CS,RS>& m,const SymMat<Dim,E,S>& sy) {
    return Mat<M,N,ME,CS,RS>::template Result<SymMat<Dim,E,S> >
        ::MulOpNonConforming::perform(m,sy);
}

    // CROSS PRODUCT

// Cross product and corresponding operator%, but only for 2- and 3-Vecs
// and Rows. It is OK to mix same-length Vecs & Rows; cross product is
// defined elementwise and never transposes the individual elements.
//
// We also define crossMat(v) below which produces a 2x2 or 3x3
// matrix M such that M*w = v % w for w the same length vector (or row) as v.
// TODO: the 3d crossMat is skew symmetric but currently there is no
// SkewMat class defined so we have to return a full 3x3.

// For 3d cross product, we'll follow Matlab and make the return type a
// Row if either or both arguments are Rows, although I'm not sure that's
// a good idea! Note that the definition of crossMat means crossMat(r)*v
// will yield a column, while r%v is a Row.

template <class E1, int S1, class E2, int S2> inline
Vec<3,typename CNT<E1>::template Result<E2>::Mul>
cross(const Vec<3,E1,S1>& a, const Vec<3,E2,S2>& b) {
    return Vec<3,typename CNT<E1>::template Result<E2>::Mul>
        (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
template <class E1, int S1, class E2, int S2> inline
Vec<3,typename CNT<E1>::template Result<E2>::Mul>
operator%(const Vec<3,E1,S1>& a, const Vec<3,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
cross(const Vec<3,E1,S1>& a, const Row<3,E2,S2>& b) {
    return Row<3,typename CNT<E1>::template Result<E2>::Mul>
        (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
operator%(const Vec<3,E1,S1>& a, const Row<3,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
cross(const Row<3,E1,S1>& a, const Vec<3,E2,S2>& b) {
    return Row<3,typename CNT<E1>::template Result<E2>::Mul>
        (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
operator%(const Row<3,E1,S1>& a, const Vec<3,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
cross(const Row<3,E1,S1>& a, const Row<3,E2,S2>& b) {
    return Row<3,typename CNT<E1>::template Result<E2>::Mul>
        (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]);
}
template <class E1, int S1, class E2, int S2> inline
Row<3,typename CNT<E1>::template Result<E2>::Mul>
operator%(const Row<3,E1,S1>& a, const Row<3,E2,S2>& b) {return cross(a,b);}

// 2d cross product just returns a scalar. This is the z-value you
// would get if the arguments were treated as 3-vectors with 0
// z components.

template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
cross(const Vec<2,E1,S1>& a, const Vec<2,E2,S2>& b) {
    return a[0]*b[1]-a[1]*b[0];
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
operator%(const Vec<2,E1,S1>& a, const Vec<2,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
cross(const Row<2,E1,S1>& a, const Vec<2,E2,S2>& b) {
    return a[0]*b[1]-a[1]*b[0];
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
operator%(const Row<2,E1,S1>& a, const Vec<2,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
cross(const Vec<2,E1,S1>& a, const Row<2,E2,S2>& b) {
    return a[0]*b[1]-a[1]*b[0];
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
operator%(const Vec<2,E1,S1>& a, const Row<2,E2,S2>& b) {return cross(a,b);}

template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
cross(const Row<2,E1,S1>& a, const Row<2,E2,S2>& b) {
    return a[0]*b[1]-a[1]*b[0];
}
template <class E1, int S1, class E2, int S2> inline
typename CNT<E1>::template Result<E2>::Mul
operator%(const Row<2,E1,S1>& a, const Row<2,E2,S2>& b) {return cross(a,b);}

    // CROSS MAT

// Calculate matrix M(v) such that M(v)*w = v % w. We produce the
// same M regardless of whether v is a column or row.
template <class E, int S> inline
Mat<3,3,E>
crossMat(const Vec<3,E,S>& v) {
    return Mat<3,3,E>(Row<3,E>( E(0), -v[2],  v[1]),
                      Row<3,E>( v[2],  E(0), -v[0]),
                      Row<3,E>(-v[1],  v[0],  E(0)));
}
template <class E, int S> inline
Mat<3,3,E> crossMat(const Row<3,E,S>& r) {return crossMat(r.positionalTranspose());}

// Calculate M(v) such that M(v)*w = v0*w1-v1*w0 = v % w (a scalar). Whether v is column
// or row we create the same M, which must be a row.
template <class E, int S> inline
Row<2,E>
crossMat(const Vec<2,E,S>& v) {
    return Row<2,E>(-v[1], v[0]);
}
template <class E, int S> inline
Row<2,E> crossMat(const Row<2,E,S>& r) {return crossMat(r.positionalTranspose());}

    // DETERMINANT

/// Special case 1x1 determinant. No computation.
template <class E, int CS, int RS> inline
const E& det(const Mat<1,1,E,CS,RS>& m) {
    return m(0,0);
}

/// Special case 2x2 determinant. 3 flops.
template <class E, int CS, int RS> inline
E det(const Mat<2,2,E,CS,RS>& m) {
    return E(m(0,0)*m(1,1) - m(0,1)*m(1,0));
}

/// Special case 3x3 determinant. 14 flops.
template <class E, int CS, int RS> inline
E det(const Mat<3,3,E,CS,RS>& m) {
    return E(  m(0,0)*(m(1,1)*m(2,2)-m(1,2)*m(2,1))
             - m(0,1)*(m(1,0)*m(2,2)-m(1,2)*m(2,0))
             + m(0,2)*(m(1,0)*m(2,1)-m(1,1)*m(2,0)));
}

/// Calculate the determinant of a square matrix larger than 3x3
/// by recursive template expansion. The matrix elements must be 
/// multiplication compatible for this to compile successfully. 
/// All scalar element types are acceptable; some composite types 
/// will also work but the result is probably meaningless.
/// The determinant suffers from increasingly bad scaling as the
/// matrices get bigger; you should consider an alternative if
/// possible (see Golub and van Loan, "Matrix Computations").
/// This uses M*det(M-1) + 4*M flops. For 4x4 that's 60 flops,
/// for 5x5 it's 320, and it grows really fast from there!
/// TODO: this is not the right way to calculate determinant --
/// should calculate LU factorization at 2/3 n^3 flops, then
/// determinant is product of LU's diagonals.
template <int M, class E, int CS, int RS> inline
E det(const Mat<M,M,E,CS,RS>& m) {
    typename CNT<E>::StdNumber sign(1);
    E                          result(0);
    // We're always going to drop the first row.
    const Mat<M-1,M,E,CS,RS>& m2 = m.getSubMat<M-1,M>(1,0);
    for (int j=0; j < M; ++j) {
        // det() here is recursive but terminates at 3x3 above.
        result += sign*m(0,j)*det(m2.dropCol(j));
        sign = -sign;
    }
    return result;
}

    // INVERSE

/// Specialized 1x1 lapackInverse(): costs one divide.
template <class E, int CS, int RS> inline
typename Mat<1,1,E,CS,RS>::TInvert lapackInverse(const Mat<1,1,E,CS,RS>& m) {
    typedef typename Mat<1,1,E,CS,RS>::TInvert MInv;
    return MInv( E(typename CNT<E>::StdNumber(1)/m(0,0)) );
}

/// General inverse of small, fixed-size, square (mXm), non-singular matrix with 
/// scalar elements: use Lapack's LU routine with pivoting. This will only work 
/// if the element type E is a scalar type, although negator<> and conjugate<> 
/// are OK. This routine is <em>not</em> specialized for small matrix sizes other
/// than 1x1, while the inverse() method is specialized for other small sizes
/// for speed, possibly losing some numerical stability. The inverse() function
/// ends up calling this one at sizes for which it has no specialization. 
/// Normally you should call inverse(), but you can call lapackInverse() 
/// explicitly if you want to ensure that the most stable algorithm is used.
/// @see inverse()
template <int M, class E, int CS, int RS> inline
typename Mat<M,M,E,CS,RS>::TInvert lapackInverse(const Mat<M,M,E,CS,RS>& m) {
    // Copy the source matrix, which has arbitrary row and column spacing,
    // into the type for its inverse, which must be dense, columnwise
    // storage. That means its column spacing will be M and row spacing
    // will be 1, as Lapack expects for a Full matrix.
    typename Mat<M,M,E,CS,RS>::TInvert inv = m; // dense, columnwise storage

    // We'll perform the inversion ignoring negation and conjugation altogether, 
    // but the TInvert Mat type negates or conjugates the result. And because 
    // of the famous Sherman-Eastman theorem, we know that 
    // conj(inv(m))==inv(conj(m)), and of course -inv(m)==inv(-m).
    typedef typename CNT<E>::StdNumber Raw;   // Raw is E without negator or conjugate.
    Raw* rawData = reinterpret_cast<Raw*>(&inv(0,0));
    int ipiv[M], info;

    // This replaces "inv" with its LU facorization and pivot matrix P, such that
    // P*L*U = m (ignoring negation and conjugation).
    Lapack::getrf<Raw>(M,M,rawData,M,&ipiv[0],info);
    SimTK_ASSERT1(info>=0, "Argument %d to Lapack getrf routine was bad", -info);
    SimTK_ERRCHK1_ALWAYS(info==0, "lapackInverse(Mat<>)",
        "Matrix is singular so can't be inverted (Lapack getrf info=%d).", info);

    // The workspace size must be at least M. For larger matrices, Lapack wants
    // to do factorization in blocks of size NB in which case the workspace should
    // be M*NB. However, we will assume that this matrix is small (in the sense
    // that it fits entirely in cache) so the minimum workspace size is fine and
    // we don't need an extra getri call to find the workspace size nor any heap
    // allocation.

    Raw work[M];
    Lapack::getri<Raw>(M,rawData,M,&ipiv[0],&work[0],M,info);
    SimTK_ASSERT1(info>=0, "Argument %d to Lapack getri routine was bad", -info);
    SimTK_ERRCHK1_ALWAYS(info==0, "lapackInverse(Mat<>)",
        "Matrix is singular so can't be inverted (Lapack getri info=%d).", info);
    return inv;
}


/// Specialized 1x1 inverse: costs one divide.
template <class E, int CS, int RS> inline
typename Mat<1,1,E,CS,RS>::TInvert inverse(const Mat<1,1,E,CS,RS>& m) {
    typedef typename Mat<1,1,E,CS,RS>::TInvert MInv;
    return MInv( E(typename CNT<E>::StdNumber(1)/m(0,0)) );
}

/// Specialized 2x2 inverse: costs one divide plus 9 flops.
template <class E, int CS, int RS> inline
typename Mat<2,2,E,CS,RS>::TInvert inverse(const Mat<2,2,E,CS,RS>& m) {
    const E               d  ( det(m) );
    const typename CNT<E>::TInvert ood( CNT<E>::StdNumber(1)/d );
    return typename Mat<2,2,E,CS,RS>::TInvert( E( ood*m(1,1)), E(-ood*m(0,1)),
                                               E(-ood*m(1,0)), E( ood*m(0,0)));
}

/// Specialized 3x3 inverse: costs one divide plus 45 flops (for real-valued
/// matrices). No pivoting done here so this may be subject to numerical errors 
/// that Lapack would avoid. Call lapackInverse() instead if you're worried.
/// @see lapackInverse()
template <class E, int CS, int RS> inline
typename Mat<3,3,E,CS,RS>::TInvert inverse(const Mat<3,3,E,CS,RS>& m) {
    // Calculate determinants for each 2x2 submatrix with first row removed.
    // (See the specialized 3x3 determinant routine above.) We're calculating
    // this explicitly here because we can re-use the intermediate terms.
    const E d00(m(1,1)*m(2,2)-m(1,2)*m(2,1)),
            d01(m(1,0)*m(2,2)-m(1,2)*m(2,0)),
            d02(m(1,0)*m(2,1)-m(1,1)*m(2,0));

    // This is the 3x3 determinant and its inverse.
    const E d  (m(0,0)*d00 - m(0,1)*d01 + m(0,2)*d02);
    const typename CNT<E>::TInvert 
            ood(typename CNT<E>::StdNumber(1)/d);

    // The other six 2x2 determinants we can't re-use, but we can still
    // avoid some copying by calculating them explicitly here.
    const E d10(m(0,1)*m(2,2)-m(0,2)*m(2,1)),
            d11(m(0,0)*m(2,2)-m(0,2)*m(2,0)),
            d12(m(0,0)*m(2,1)-m(0,1)*m(2,0)),
            d20(m(0,1)*m(1,2)-m(0,2)*m(1,1)),
            d21(m(0,0)*m(1,2)-m(0,2)*m(1,0)),
            d22(m(0,0)*m(1,1)-m(0,1)*m(1,0));

    return typename Mat<3,3,E,CS,RS>::TInvert
       ( E( ood*d00), E(-ood*d10), E( ood*d20),
         E(-ood*d01), E( ood*d11), E(-ood*d21),
         E( ood*d02), E(-ood*d12), E( ood*d22) ); 
}

/// For any matrix larger than 3x3, we just punt to the Lapack implementation.
/// @see lapackInverse()
template <int M, class E, int CS, int RS> inline
typename Mat<M,M,E,CS,RS>::TInvert inverse(const Mat<M,M,E,CS,RS>& m) {
    return lapackInverse(m);
}

// Implement the Mat<>::invert() method using the above specialized 
// inverse functions. This will only compile if M==N.
template <int M, int N, class ELT, int CS, int RS> inline
typename Mat<M,N,ELT,CS,RS>::TInvert 
Mat<M,N,ELT,CS,RS>::invert() const {
    return SimTK::inverse(*this);
}

} //namespace SimTK


#endif //SimTK_SIMMATRIX_SMALLMATRIX_MIXED_H_
