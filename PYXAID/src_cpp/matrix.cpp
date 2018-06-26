/***********************************************************
 * Copyright (C) 2013 Alexey V. Akimov
 * This file is distributed under the terms of the
 * GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * http://www.gnu.org/copyleft/gpl.txt
***********************************************************/

#include "matrix.h"
#include <cstdlib>
#include <cstdio>
using namespace std;

matrix::matrix(vector<vector<double> >& re_part,vector<vector<double> >& im_part){
/*****************************************************************
  Constructor: creates a matrix from 2 2D-arrays - real and imaginary parts
*****************************************************************/
  if(re_part.size()!=im_part.size()){ 
    cout<<"Error in matrix constructor: y-dimensions(num of rows) of the real and imaginary arrays are not equal\n"; exit(0); 
  }
  if(re_part[0].size()!=im_part[0].size()){
    cout<<"Error in matrix constructor: x-dimensions(num of cols) of the real and imaginary arrays are not equal\n"; exit(0);
  }
  n_rows = re_part.size();
  n_cols = re_part[0].size();
  n_elts = n_rows * n_cols;

  M = new complex<double>[n_elts];
  int n = 0;
  for(int i=0;i<n_rows;i++){ 
    for(int j=0;j<n_cols;j++){
      M[n] = complex<double>(re_part[i][j],im_part[i][j]); n++;
    }
  }
 
}


matrix::matrix(const matrix& obj){
  n_rows = obj.n_rows;
  n_cols = obj.n_cols;
  n_elts = obj.n_elts;

  M = new complex<double>[n_elts];
  for(int i=0;i<n_elts;i++){ M[i] = obj.M[i];  }

//  cout<<"Copy constructor\n";
}

matrix matrix::operator-(){
  matrix tmp(n_rows,n_cols);
  for(int i=0;i<n_elts;i++){ tmp.M[i]=-M[i]; }
  return tmp;
}

matrix matrix::operator*(const matrix& ob){
// (n_rows x ob.n_cols) = (n_rows x n_cols) * (ob.n_rows * ob.n_cols)
// n_cols must be equal to ob.n_rows
  if(n_cols!=ob.n_rows){ 
    std::cout<<"Matrix multiplication error: Dimensions of operands must match\n";
    std::cout<<"You try to muplitpy matrix "<<n_rows<<" by "<<n_cols<<" and the matrix "
             <<ob.n_rows<<" by "<<ob.n_cols<<"\n";
    std::cout<<"Exiting...\n";
    exit(0);  
  }
  else{
    int n=ob.n_cols;
    int kn; // k*n
    int rn; // row*n
    int rncols; // row*n_cols
   
    matrix Temp(n_rows,n);

    rn = 0;
    rncols = 0;
    for(int row=0;row<n_rows;row++){
      for(int col=0;col<n;col++){
        complex<double> d(0.0,0.0);
        /* Standard formulation
        for(int k=0;k<n_cols;k++){ d+=M[row*n_cols+k]*ob.M[k*n+col];  }
        Temp.M[row*n+col] = d;
        */

        // More efficient formulation
        kn = 0;
        for(int k=0;k<n_cols;k++){ d+=M[rncols+k]*ob.M[kn+col]; kn += n; }
        Temp.M[rn+col] = d;
      }
      rn += n;
      rncols += n_cols;
    }
    return Temp;
  }
}

matrix matrix::operator+(const matrix& ob){ 
  matrix Temp(n_rows,n_cols);
  for(int i=0;i<n_elts;i++) {Temp.M[i]=M[i]+ob.M[i];}
  return Temp;
}

matrix matrix::operator-(const matrix& ob){
  matrix Temp(n_rows,n_cols);
  for(int i=0;i<n_elts;i++) {Temp.M[i]=M[i]-ob.M[i];}
  return Temp;
}

void matrix::operator+=(const matrix& ob){
  for(int i=0;i<n_elts;i++) {M[i]+=ob.M[i];}
}

void matrix::operator-=(const matrix& ob){
  for(int i=0;i<n_elts;i++) {M[i]-=ob.M[i];}
}

void matrix::operator*=(const double& f){
  for(int i=0;i<n_elts;i++) {M[i]*=f;}
}

void matrix::operator*=(const complex<double>& f){
  for(int i=0;i<n_elts;i++) {M[i]*=f;}
}


void matrix::operator*=(const matrix& ob){
// (n_rows x ob.n_cols) = (n_rows x n_cols) * (ob.n_rows * ob.n_cols)
// n_cols must be equal to ob.n_rows
  if(n_cols!=ob.n_rows){ 
    std::cout<<"Matrix multiplication error: Dimensions of operands must match\n";
    std::cout<<"You try to muplitpy matrix "<<n_rows<<" by "<<n_cols<<" and the matrix "
             <<ob.n_rows<<" by "<<ob.n_cols<<"\n";
    std::cout<<"Exiting...\n";
    exit(0);  
  }
  else{
    int n=ob.n_cols;
    // Counters
    int rncols; // row*n_cols
    int kn;     // k*n
    int rn;     // row*n
    complex<double> *TM;
    TM = new complex<double>[n_rows*n];
    //matrix Temp(n_rows,n);

    rn = 0;
    rncols = 0;
    for(int row=0;row<n_rows;row++){
      for(int col=0;col<n;col++){
        complex<double> d(0.0,0.0);

        kn = 0;
        for(int k=0;k<n_cols;k++){ d+=M[rncols+k]*ob.M[kn+col]; kn += n; }
        TM[rn+col] = d;
      }
      rn += n;
      rncols += n_cols;

    }

    for(int i=0;i<n_elts;i++){ M[i] = TM[i]; }

    delete [] TM;
  }
  
}


matrix matrix::operator/(double num){ 
  matrix m(n_rows,n_cols);
  for(int i=0;i<n_elts;i++){  m.M[i] = M[i]/num;  }
  return m;
}

matrix matrix::operator/(complex<double> num){
  matrix m(n_rows,n_cols);
  for(int i=0;i<n_elts;i++){  m.M[i] = M[i]/num;  }
  return m;
}

matrix& matrix::operator=(const matrix& ob){

  //if(this == &ob){ return *this; }
  //else{

  n_rows = ob.n_rows;
  n_cols = ob.n_cols;
  n_elts = ob.n_elts;

  // Lets comment below 2 lines - it may be not completely correct to do it this way
  // but assignment means there is a left-hand side already allocated - there is no
  // reason to delete storage and reallocate it again - just copy data staight ahead
  // This gives speedup of factor 2

  //delete [] M;  // This is very important and not very obvious - without this line - there will be a memory leak
  //M = new complex<double>[n_elts];
  
  //for(int i=0;i<n_elts;i++){ M[i] = ob.M[i];  }

  memcpy(M,ob.M,sizeof(complex<double>)*n_elts);  // this is slightly more efficient version than above
  
  return *this;


  //}

}

matrix matrix::operator=(double num){
  for(int i=0;i<n_elts;i++){ M[i] = num;  }
  return *this;
}

matrix matrix::operator=(complex<double> num){
  for(int i=0;i<n_elts;i++){ M[i] = num;  }
  return *this;
}

matrix operator*(const double& f,  const matrix& m1){
  matrix m(m1.n_rows,m1.n_cols);
  for(int i=0;i<m1.n_elts;i++){  m.M[i]=m1.M[i]*f;  }
  return m;
}

matrix operator*(const matrix &m1, const double  &f){
  matrix m(m1.n_rows,m1.n_cols);
  for(int i=0;i<m1.n_elts;i++){  m.M[i]=m1.M[i]*f;  }
  return m;
}

matrix operator*(const complex<double>& f,  const matrix& m1){
  matrix m(m1.n_rows,m1.n_cols);
  for(int i=0;i<m1.n_elts;i++){  m.M[i]=m1.M[i]*f;  }
  return m;
}

matrix operator*(const matrix &m1, const complex<double>  &f){
  matrix m(m1.n_rows,m1.n_cols);
  for(int i=0;i<m1.n_elts;i++){  m.M[i]=m1.M[i]*f;  }
  return m;
}

ostream& operator<<(ostream &strm,matrix ob){
  strm.setf(ios::showpoint);
  for(int i=0;i<ob.n_rows;i++){
    for(int j=0;j<ob.n_cols;j++){
      strm.precision(8);
      strm.width(10);
      strm<<left<<ob.M[i*ob.n_cols+j]<<"  ";
    }
    strm<<endl;
  }
  return strm;
}
istream& operator>>(istream& strm,matrix &ob){
//     Do not defined for general case       !!!      
  return strm;
}




matrix matrix::conj(){
  matrix m(n_rows,n_cols);
  for(int i=0;i<m.n_elts;i++){ m.M[i] = std::conj(M[i]); }
  return m;
}

matrix matrix::T(){
  matrix m(n_cols,n_rows);
  for(int i=0;i<n_rows;i++){
    for(int j=0;j<n_cols;j++){
      m.M[j*n_rows+i] = M[i*n_cols+j];
    }
  }
  return m;
}

matrix matrix::H(){
  matrix m(n_cols,n_rows);
  for(int i=0;i<n_rows;i++){
    for(int j=0;j<n_cols;j++){
      m.M[j*n_rows+i] = std::conj(M[i*n_cols+j]);
    }
  }
  return m;
}

void matrix::load_identity(){
  for(int i=0;i<n_elts;i++){ M[i] = complex<double>(0.0,0.0); }
  for(i=0;i<n_rows;i++){ M[i*n_cols+i] = complex<double>(1.0,0.0); }
}


matrix matrix::col(int i){
// takes given column and makes it n x 1 matrix
  matrix tmp(n_rows,1);
  for(int j=0;j<n_rows;j++){ tmp.M[j] = M[j*n_cols+i]; }
  return tmp;
}

matrix matrix::row(int i){
// takes given row and makes it 1 x n matrix
  matrix tmp(1,n_cols);
  for(int j=0;j<n_cols;j++){ tmp.M[j] = M[i*n_cols+j]; }
  return tmp;
}



void matrix::max_nondiagonal(int& row,int& col){
  double maxeps = norm(M[1]); row = 0; col = 1;
  double eps;
  for(int r=0;r<n_rows;r++){
    for(int c=r+1;c<n_cols;c++){
      eps = norm(M[r*n_cols+c]);
      if(eps>=maxeps){ row = r; col = c; maxeps = eps; }
    }
  }  
}
/*
void matrix::inverse(matrix& inv,double EPS,int max_num_iter,int is_cycle,int alg){
// Based on: EVECT * EVAL = M * EVECT =>  M * (EVECT* EVAL^-1 * EVECT^-1)
// But EVECT^-1 = EVECT^T => M^-1 = (EVECT* EVAL^-1 * EVECT^T)
  if(n_rows==n_cols){
    matrix eval(n_rows,n_cols);
    matrix evec(n_rows,n_cols);
    matrix einv(n_rows,n_cols);

    eigen(eval,evec,EPS,max_num_iter,is_cycle,alg);
    for(int i=0;i<n_rows;i++){ einv.M[i*n_cols+i] = 1.0/eval.M[i*n_cols+i]; }

    inv = evec*einv*(evec.T());
  }  
  else{ cout<<"Warning: in matrix::inverse - matrix is not square\n"; }
}
*/


void matrix::eigen0(matrix& EVAL, matrix& EVECT,double EPS,int max_num_iter,int is_cycle,int alg) {
// Description: Jacobi Eigenvalue Solver - only for complex hermitian matrix!
// EVECT * EVAL  =  M * EVECT
// V = P^T
// EVAL = V_M V_{M-1} ... V_0 * M * V_0^T * V_1^T ... V_M^T = Q^T * M * Q
// EVECT = Q = V_0^T * V_1^T ... V_M^T
// http://coderov.net/vma/140-eigenvalues/862-direct-method-of-rotation.html  <- this is strange, so use
// http://en.wikipedia.org/wiki/Jacobi_method_for_complex_Hermitian_matrices
// Note: Wikipedia source contains an error: matrix element for m=q and n=p should be changed from
// exp(-i*teta1)*cos(teta2) to -i*exp(-i*teta1)*cos(teta2) !!!
// Also in formula for tan(phi2) I assumed that the real part of H_{p,q} is used!

// For Shur decomposition see: http://ndickson.wordpress.com/2011/07/13/jacobi-eigenvalue-algorithm-schur-decomposition-and-wikipedia/
// My derivations are:
// V = |  c  -s* |
//     |  s   c* |
// // Case 1:                    // Case 2:
// c = 1 + sqrt(1 + a*b)         c = sqrt(b)
// -s* = a                       s = conj(sqrt(a))
// a = 2.0*Mij/(Mii-Mjj)         a = Mij/norm
// b = 2.0*Mji/(Mii-Mjj)         b = Mji/norm, norm =sqrt(|Mij|^2+|Mji|^2)

// New parameters: 
// is_cycle:
//       0 - will be using max non-diagonal element
//       1 - cyclic order will be used
//
// alg: - choose algorithm
//       0 - Shur decomposition
//       1 - Jacobi rotations


  int n = n_rows; // = n_cols
  int row, col, i, j, k, num_iter;
  double val,phi,eps;

  matrix V(n,n);
  matrix temp(n,n);

  for(i=0;i<n_elts;i++){     temp.M[i] = M[i];   }
  EVECT.load_identity();
  num_iter = 0;

/*
  // Define convergence criteria.
  k=0; eps = 0.0;
  for(i=0;i<temp.n_rows;i++){
    for(j=0;j<temp.n_cols;j++){
      if(i!=j) {eps+=norm(temp.M[k]); }
       k++;
    }
  }
*/
  
  row = 0; col = 1;

  do{
  //while(eps>EPS){
    num_iter++;

    //cout<<"num_iter = "<<num_iter<<"  eps = "<<eps<<endl;
    if(!is_cycle){  temp.max_nondiagonal(row,col); }   
    
    if(alg==0){
    // Shur rotation   
      complex<double> c,s;
      double L;
      // Case 1
      if(sqrt(norm(temp.M[row*n_cols+col]))<1e+14*sqrt(norm(temp.M[row*n_cols+row]-temp.M[col*n_cols+col]))){
        complex<double> a = 2.0*temp.M[row*n_cols+col]/(temp.M[row*n_cols+row]-temp.M[col*n_cols+col]);
        complex<double> b = 2.0*temp.M[col*n_cols+row]/(temp.M[row*n_cols+row]-temp.M[col*n_cols+col]);      
        c = 1.0 + sqrt(1.0 + a*b);
        s = std::conj(-a);
      }
      // Case 2
      else{
        complex<double> a = temp.M[row*n_cols+col];
        complex<double> b = temp.M[col*n_cols+row];
        double nrm = sqrt(norm(a)+norm(b));
        c = sqrt(b/nrm);
        s = std::conj(sqrt(a/nrm));
      }
      L = sqrt(norm(c) + norm(s));
      c = c/L;  s = s/L;

      V.load_identity();
      V.M[row*n_cols + row] = c;   V.M[row*n_cols + col] = std::conj(-s);
      V.M[col*n_cols + row] = s;   V.M[col*n_cols + col] = std::conj(c);
   }// Shur decomposition

   else if(alg==1){
     // Jacobi rotation
     double phi1, phi2;
     phi1 = atan2(    temp.M[row*n_cols+col].imag(), temp.M[row*n_cols+col].real());
     phi2 = atan2(2.0*temp.M[row*n_cols+col].real(),(temp.M[row*n_cols+row].real()-temp.M[col*n_cols+col].real()));
     double tet1,tet2;
     tet1 = 0.25*(M_PI - 2.0*phi1);
     tet2 = 0.5*phi2;

     double s1,s2,c1,c2;
     s1 = sin(tet1); s2 = sin(tet2);
     c1 = cos(tet1); c2 = cos(tet2);
 
     V.load_identity();
     V.M[row*n_cols + row] = complex<double>(-s1*s2,-c1*s2);   V.M[row*n_cols + col] = complex<double>(s1*c2,-c1*c2);
     V.M[col*n_cols + row] = complex<double>(-s1*c2,-c1*c2);   V.M[col*n_cols + col] = complex<double>(-s1*s2,c1*s2);

   }// Jacobi rotation



    //temp = V*temp*V.H();
    temp = V*temp*V.H();
 
    EVECT = EVECT*V.H();
    
    k = 0; eps = 0.0;
    for(i=0;i<temp.n_rows;i++){
      for(j=0;j<temp.n_cols;j++){
        if(i!=j) {eps+=norm(temp.M[k]); }
        k++;
      }// for j
    }// for i

    //cout<<"num_iter = "<<num_iter<<" eps = " <<eps<<endl;

    

    if(is_cycle){
      if(row<(n-2) && col<(n-1)) { col++; }
      else if(row<(n-2) && col==(n-1)) { row++; col = row + 1;}
      else if(row==(n-2) && col==(n-1)){ row = 0; col = 1; }
    }


//  }// while eps>EPS
  }while(eps>EPS && num_iter<max_num_iter);

  if(eps>EPS){
    cout<<"Error: In void matrix::eigen(matrix& EVAL, matrix& EVECT,double EPS,int max_num_iter,int is_cycle,int alg)\n";
    cout<<"Number of iterations num_iter = "<<num_iter<<" exceeded maximal number of iterations max_num_iter = "<<max_num_iter<<endl;
    cout<<"Precision achieved eps = "<<eps<<" is lower then requested accuracy EPS = "<<EPS<<endl;
    cout<<"Convergense failed. Exiting...\n";
    exit(0);
  }



  EVAL = temp;
}

void matrix::QR(matrix& w,matrix& R){
/****************************************************************************
 Very helpful resource:
 http://www.math.umn.edu/~olver/aims_/qr.pdf

 QR decomposition is basically the Gram-Schmidt orthogonaliztion procedue:

 M = Q * R, where Q - is a set of orthonormal vectors and R are the weights
*****************************************************************************/
  int row, col, i, j, k;
  double nrm; // norm
  complex<double> dot; // dot product
  int n = n_rows; // = n_cols

  for(i=0;i<n_elts;i++){     w.M[i] = M[i];   }


  for(i=0;i<n;i++){


    if(i>0){
      // w_k = w_k - (w_k,w_i)*w_i  k = i, i+1, ... n
      for(k=i;k<n;k++){

        dot = complex<double>(0.0,0.0);        
        for(j=0;j<n;j++){ dot = dot + (std::conj(w.M[j*n+k])*w.M[j*n+(i-1)]); }// for j

        dot = std::conj(dot); // This is very tricky part!!!  - arises in case of complex matrixes 

        for(j=0;j<n;j++){ w.M[j*n+k] = w.M[j*n+k] - dot*w.M[j*n+(i-1)];}


      }// for k
    }// i > 0

    
    // Simply normalize i-th column-vector
    nrm = 0.0;
    for(j=0;j<n;j++){ nrm += (std::conj(w.M[j*n+i])*w.M[j*n+i]).real(); }// for j
    nrm = sqrt(nrm);
    for(j=0;j<n;j++){ w.M[j*n+i] /= nrm; }


  }// for i


  // Now for R-matrix
  R = complex<double>(0.0,0.0);
  for(i=0;i<n;i++){
    for(j=i;j<n;j++){      
      for(k=0;k<n;k++){
        // R[i][j] = w_j * u_i, note w - is actually original matrix M, while u is what is now w.
        // Note: For complex (this) case the actual definition of the R[i][j] coefficients is:
        // R[j][i] = (w_j^*  x  u_i)^* = w_j * u_i^*, where ^* - denotes complex conjugation
        R.M[i*n+j] += (M[k*n+j]) * std::conj(w.M[k*n+i]);
      }// for k
    }// for j
  }// for i


}


void matrix::QR1(matrix& w,matrix& R){
/****************************************************************************
 Very helpful resource:
 http://www.math.umn.edu/~olver/aims_/qr.pdf

 QR decomposition is basically the Gram-Schmidt orthogonaliztion procedue:

 M = Q * R, where Q - is a set of orthonormal vectors and R are the weights

 This version is designed for tridiagonal matrices
*****************************************************************************/
  int row, col, i, j, k;
  double nrm; // norm
  complex<double> dot; // dot product
  int n = n_rows; // = n_cols

  for(i=0;i<n_elts;i++){     w.M[i] = M[i];   }


  for(i=0;i<n;i++){


    if(i>0){
      // w_k = w_k - (w_k,w_i)*w_i  k = i, i+1
      for(k=i;k<=min((n-1),(i+1));k++){

        dot = complex<double>(0.0,0.0);
        // k = i, i+1 - two or 1 term in dot product computations
        for(j=0;j<=min((i+2),(n-1));j++){ dot += (std::conj(w.M[j*n+k])*w.M[j*n+(i-1)]); }        

        dot = std::conj(dot); // This is very tricky part!!!  - arises in case of complex matrixes 


        // k = i or i+1
        for(j=0;j<=min(i,(n-1));j++) { w.M[j*n+k] = w.M[j*n+k] - dot*w.M[j*n+(i-1)];}


      }// for k
    }// i > 0

    
    // Simply normalize i-th column-vector
    nrm = 0.0;
    for(j=0;j<=min(n-1,(i+1));j++){ nrm += (std::conj(w.M[j*n+i])*w.M[j*n+i]).real(); }// for j
    nrm = sqrt(nrm);
    for(j=0;j<=min(n-1,(i+1));j++){ w.M[j*n+i] /= nrm; }


  }// for i


  // Now for R-matrix
  R = complex<double>(0.0,0.0);
  for(i=0;i<n;i++){
    for(j=i;j<=min(n-1,i+2);j++){      
      for(k=0;k<n;k++){
        // R[i][j] = w_j * u_i, note w - is actually original matrix M, while u is what is now w.
        // Note: For complex (this) case the actual definition of the R[i][j] coefficients is:
        // R[j][i] = (w_j^*  x  u_i)^* = w_j * u_i^*, where ^* - denotes complex conjugation
        //if(j-i==0 || j-i==1 || j-i==2){
          R.M[i*n+j] += (M[k*n+j]) * std::conj(w.M[k*n+i]);
        //}
      }// for k
    }// for j
  }// for i


}


void qr(double EPS,int n,matrix& eval,vector<double>& Eval){
// --------- Recursive QR iterations ------------
// eval - is the input tridiagonal matrix
// n - is a size of the problem

  matrix Q(n,n);
  matrix R(n,n);
  int iter = 0;
  int stop = 0;


  do{
       
    
    eval.QR1(Q,R);

    eval = 0.0;

    // The following stepas are basically the efficient way to do:
    // eval = R * Q
    // Fill out the main diagonal
    for(int i=0;i<n;i++){ 
      if(i==n-1){  eval.M[i*n+i]  = R.M[i*n+i]*Q.M[i*n+i];   }          // only 1 term here
      else{        eval.M[i*n+i]  = R.M[i*n+i]*Q.M[i*n+i] + R.M[i*n+(i+1)]*Q.M[(i+1)*n+i];}  // in fact just only 2 terms here
      // Wilkinson shift:
      //eval.M[i*n+i] += mu;
 
    }
    // Fill out upper diagonal
    for(i=0;i<n-1;i++){ 
      // j = i+1
      if(i==n-2){  eval.M[i*n+(i+1)]  = R.M[i*n+i]*Q.M[i*n+(i+1)] + 
                                        R.M[i*n+(i+1)]*Q.M[(i+1)*n+(i+1)];}   // only 2 terms here
      else{        eval.M[i*n+(i+1)]  = R.M[i*n+i]*Q.M[i*n+(i+1)] + 
                                        R.M[i*n+(i+1)]*Q.M[(i+1)*n+(i+1)] +
                                        R.M[i*n+(i+2)]*Q.M[(i+2)*n+(i+1)];}  // all 3 terms

      // The lower diagonal - is by hermitian symmetry:
      eval.M[(i+1)*n+i] = complex<double>(eval.M[i*n+(i+1)].real(), -eval.M[i*n+(i+1)].imag());
    }


    // m has a tridiagonal form, so judge convergence by the elements in
    // the closest off-diagonal
    stop = 0;
    for(i=0;i<(n-1);i++){  
      if(  (fabs(eval.M[i*n+(i+1)].real())<EPS) && (fabs(eval.M[i*n+(i+1)].imag())<EPS) ){

         // Element (i, j=i+1) is  "zero"
         int sz_up = i+1;
         int sz_dn = n-i-1;

         if(sz_up==1 && sz_dn==1){ // Here we just finished 2x2 matrix - done
           Eval[0] = eval.M[0].real();
           Eval[1] = eval.M[3].real();
         }
         else if(sz_up==1 && sz_dn>1){
           matrix dn(sz_dn,sz_dn); dn = 0.0;
           // copy diagonal elements
           for(int j=i+1;j<n;j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))] = eval.M[j*n+j]; }
           // upper off-diagonal elements
           for(j=i+1;j<(n-1);j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))+1] = eval.M[j*n+j+1]; }
           // lower off-diagonal elements
           for(j=i+2;j<n;j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))-1] = eval.M[j*n+j-1]; }

           vector<double> Eval_tmp(sz_dn,0.0);
           qr(EPS,sz_dn,dn,Eval_tmp);

           Eval[0] = eval.M[0].real();
           for(j=1;j<n;j++){ Eval[j] = Eval_tmp[j-1]; } 
           Eval_tmp.clear();

         }

         else if(sz_up>1 && sz_dn==1){
           matrix up(sz_up,sz_up); up = 0.0;
           // copy diagonal elements
           for(int j=0;j<(n-1);j++){ up.M[j*sz_up + j] = eval.M[j*n+j]; }
           // upper off-diagonal elements
           for(j=0;j<(n-2);j++){ up.M[j*sz_up + j+1] = eval.M[j*n+j+1]; }
           // lower off-diagonal elements
           for(j=1;j<(n-1);j++){ up.M[j*sz_up + j-1] = eval.M[j*n+j-1]; }

           vector<double> Eval_tmp(sz_up,0.0);
           qr(EPS,sz_up,up,Eval_tmp);


           for(j=0;j<(n-1);j++){ Eval[j] = Eval_tmp[j]; } 
           Eval[n-1] = eval.M[(n-1)*n+(n-1)].real();
           Eval_tmp.clear();

         }

         else{
         // General case - both matrices are at least 2x2
           matrix dn(sz_dn,sz_dn); dn = 0.0;
           // copy diagonal elements
           for(int j=i+1;j<n;j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))] = eval.M[j*n+j]; }
           // upper off-diagonal elements
           for(j=i+1;j<(n-1);j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))+1] = eval.M[j*n+j+1]; }
           // lower off-diagonal elements
           for(j=i+2;j<n;j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))-1] = eval.M[j*n+j-1]; }

           matrix up(sz_up,sz_up); up = 0.0;
           // copy diagonal elements
           for(j=0;j<(i+1);j++){ up.M[j*sz_up + j] = eval.M[j*n+j]; }
           // upper off-diagonal elements
           for(j=0;j<i;j++){ up.M[j*sz_up + j+1] = eval.M[j*n+j+1]; }
           // lower off-diagonal elements
           for(j=1;j<(i+1);j++){ up.M[j*sz_up + j-1] = eval.M[j*n+j-1]; }


           vector<double> Eval_tmp_up(sz_up,0.0);
           qr(EPS,sz_up,up,Eval_tmp_up);

           vector<double> Eval_tmp_dn(sz_dn,0.0);
           qr(EPS,sz_dn,dn,Eval_tmp_dn);

           for(j=0;j<sz_up;j++){ Eval[j] = Eval_tmp_up[j]; } 
           for(j=i+1;j<n;j++){ Eval[j] = Eval_tmp_dn[j-(i+1)]; } 

           Eval_tmp_up.clear();
           Eval_tmp_dn.clear();
 
         }



         stop = 1;
      }// if

    }// for i

    iter++;
  }while(!stop);


}


void qr(double EPS,int n,matrix& eval,vector<double>& Eval,matrix& Evec){
// Overloading qr function - to keep track of the transformation
// matrix
// --------- Recursive QR iterations ------------
// eval - is the input tridiagonal matrix
// n - is a size of the problem
  double mu,d,a1,b2;
  int i1,i2;
  matrix Q(n,n);
  matrix R(n,n);
  matrix Q_tmp(n,n);  Q_tmp.load_identity();
  matrix I(n,n); I.load_identity();
  int iter = 0;
  int stop = 0;

  Evec.load_identity();

  do{
       
    // Perhaps they mean - minimal diagonal value
    a1 = eval.M[0].real(); i1 = 0;
    for(int i=1;i<n;i++){  d = eval.M[i*n+i].real(); if(d<a1){ a1 = d; i1 = i;} }

/*
    // Wilkinson shifts
    b    = eval.M[(n-2)*n+(n-1)];

    delta = 0.5*(an_1 - an);
    if(delta<0){ sgn = -1.0; }
    else if(delta>0){ sgn = 1.0; }
    else if(delta==0.0){ sgn = 0.0; }

    b2    = norm(b);
    mu    = an - sgn*b2/(fabs(delta) + sqrt(delta*delta + b2));

*/
    if(n==2){ mu = 0.0; }
    else{ mu = a1; }


    (eval-mu*I).QR1(Q,R);

    Evec *= Q;

    eval = 0.0;

    // The following steps are basically the efficient way to do:
    // eval = R * Q
    // Fill out the main diagonal
    for(i=0;i<n;i++){ 
      if(i==n-1){  eval.M[i*n+i]  = R.M[i*n+i]*Q.M[i*n+i];   }          // only 1 term here
      else{        eval.M[i*n+i]  = R.M[i*n+i]*Q.M[i*n+i] + R.M[i*n+(i+1)]*Q.M[(i+1)*n+i];}  // in fact just only 2 terms here
      // Shift:
      eval.M[i*n+i] += mu;
 
    }
    // Fill out upper diagonal
    for(i=0;i<n-1;i++){ 
      // j = i+1
      if(i==n-2){  eval.M[i*n+(i+1)]  = R.M[i*n+i]*Q.M[i*n+(i+1)] + 
                                        R.M[i*n+(i+1)]*Q.M[(i+1)*n+(i+1)];}   // only 2 terms here
      else{        eval.M[i*n+(i+1)]  = R.M[i*n+i]*Q.M[i*n+(i+1)] + 
                                        R.M[i*n+(i+1)]*Q.M[(i+1)*n+(i+1)] +
                                        R.M[i*n+(i+2)]*Q.M[(i+2)*n+(i+1)];}  // all 3 terms

      // The lower diagonal - is by hermitian symmetry:
      eval.M[(i+1)*n+i] = complex<double>(eval.M[i*n+(i+1)].real(), -eval.M[i*n+(i+1)].imag());
    }


    // m has a tridiagonal form, so judge convergence by the elements in
    // the closest off-diagonal
    stop = 0;
    for(i=0;i<(n-1);i++){  
      if(  (fabs(eval.M[i*n+(i+1)].real())<EPS) && (fabs(eval.M[i*n+(i+1)].imag())<EPS) ){

         // Element (i, j=i+1) is  "zero"
         int sz_up = i+1;
         int sz_dn = n-i-1;

         if(sz_up==1 && sz_dn==1){ // Here we just finished 2x2 matrix - done
           Eval[0] = eval.M[0].real();
           Eval[1] = eval.M[3].real();
         }
         else if(sz_up==1 && sz_dn>1){
           matrix dn(sz_dn,sz_dn); dn = 0.0;
           // copy diagonal elements
           for(int j=i+1;j<n;j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))] = eval.M[j*n+j]; }
           // upper off-diagonal elements
           for(j=i+1;j<(n-1);j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))+1] = eval.M[j*n+j+1]; }
           // lower off-diagonal elements
           for(j=i+2;j<n;j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))-1] = eval.M[j*n+j-1]; }

           vector<double> Eval_tmp(sz_dn,0.0);
           matrix Q_dn(sz_dn,sz_dn);
           qr(EPS,sz_dn,dn,Eval_tmp,Q_dn);

           for(j=i+1;j<n;j++){
             for(int k=i+1;k<n;k++){
               Q_tmp.M[j*n+k] = Q_dn.M[(j-(i+1))*sz_dn + (k-(i+1))];
             }
           }

           Eval[0] = eval.M[0].real();
           for(j=1;j<n;j++){ Eval[j] = Eval_tmp[j-1]; } 
           Eval_tmp.clear();

         }

         else if(sz_up>1 && sz_dn==1){
           matrix up(sz_up,sz_up); up = 0.0;
           // copy diagonal elements
           for(int j=0;j<(n-1);j++){ up.M[j*sz_up + j] = eval.M[j*n+j]; }
           // upper off-diagonal elements
           for(j=0;j<(n-2);j++){ up.M[j*sz_up + j+1] = eval.M[j*n+j+1]; }
           // lower off-diagonal elements
           for(j=1;j<(n-1);j++){ up.M[j*sz_up + j-1] = eval.M[j*n+j-1]; }

           vector<double> Eval_tmp(sz_up,0.0);
           matrix Q_up(sz_up,sz_up);
           qr(EPS,sz_up,up,Eval_tmp,Q_up);

           for(j=0;j<(i+1);j++){
             for(int k=0;k<(i+1);k++){
               Q_tmp.M[j*n+k] = Q_up.M[j*sz_up+k];
             }
           }

           for(j=0;j<(n-1);j++){ Eval[j] = Eval_tmp[j]; } 
           Eval[n-1] = eval.M[(n-1)*n+(n-1)].real();
           Eval_tmp.clear();

         }

         else{
         // General case - both matrices are at least 2x2
           matrix dn(sz_dn,sz_dn); dn = 0.0;
           // copy diagonal elements
           for(int j=i+1;j<n;j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))] = eval.M[j*n+j]; }
           // upper off-diagonal elements
           for(j=i+1;j<(n-1);j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))+1] = eval.M[j*n+j+1]; }
           // lower off-diagonal elements
           for(j=i+2;j<n;j++){ dn.M[(j-(i+1))*sz_dn + (j-(i+1))-1] = eval.M[j*n+j-1]; }

           matrix up(sz_up,sz_up); up = 0.0;
           // copy diagonal elements
           for(j=0;j<(i+1);j++){ up.M[j*sz_up + j] = eval.M[j*n+j]; }
           // upper off-diagonal elements
           for(j=0;j<i;j++){ up.M[j*sz_up + j+1] = eval.M[j*n+j+1]; }
           // lower off-diagonal elements
           for(j=1;j<(i+1);j++){ up.M[j*sz_up + j-1] = eval.M[j*n+j-1]; }

           vector<double> Eval_tmp_up(sz_up,0.0);
           matrix Q_up(sz_up,sz_up);
           qr(EPS,sz_up,up,Eval_tmp_up,Q_up);

           vector<double> Eval_tmp_dn(sz_dn,0.0);
           matrix Q_dn(sz_dn,sz_dn);
           qr(EPS,sz_dn,dn,Eval_tmp_dn,Q_dn);


           for(j=0;j<(i+1);j++){
             for(int k=0;k<(i+1);k++){
               Q_tmp.M[j*n+k] = Q_up.M[j*sz_up+k];
             }
           }

           for(j=i+1;j<n;j++){
             for(int k=i+1;k<n;k++){
               Q_tmp.M[j*n+k] = Q_dn.M[(j-(i+1))*sz_dn + (k-(i+1))];
             }
           }


           for(j=0;j<sz_up;j++){ Eval[j] = Eval_tmp_up[j]; } 
           for(j=i+1;j<n;j++){ Eval[j] = Eval_tmp_dn[j-(i+1)]; } 

           Eval_tmp_up.clear();
           Eval_tmp_dn.clear();
 
         }

         Evec *= Q_tmp;

         stop = 1;
      }// if

    }// for i

    iter++;


  }while(!stop);

}

void matrix::eigen(double EPS,matrix& EVAL,matrix& EVECT,int opt){
// This is just a convenient interface
// opt - is option - choose the method

  vector<double> Eval(n_rows,0.0);
  EVAL = 0.0;

  if(opt==1){   eigen0(EVAL,EVECT,EPS,10000,0,0);  }  // this works up to ~ n =50 
  else if(opt==2){  eigen2(EPS,Eval,EVECT);
    for(int i=0;i<n_rows;i++){ EVAL.M[i*n_cols+i] = Eval[i]; } // this is fastest version, able to work up to ~n = 250
  }
  else if(opt==3){  eigen3(EPS,Eval,EVECT);
    for(int i=0;i<n_rows;i++){ EVAL.M[i*n_cols+i] = Eval[i]; } // this works up to ~n = 200, but is slow
  }
  

}

void matrix::eigen1(double EPS,vector<double>& Eval){
//-------------------------------------------------------------
// We do the job in reductionist way - once one of the elements on 
// the off-diagonal is smaller than EPS - we split the matrix into
// 2 blocks and then deal with each other independently - deflation
//-------------------------------------------------------------

  int n = n_rows; // = n_cols

  matrix Q(n,n);
  matrix R(n,n);
  matrix eval(n,n);


  tridiagonalize(eval);

  // To avoid error propagation we just set the off-tridiagonal elements to 0.0
  int k = 0;
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      if(abs(i-j)>1){ eval.M[k] = 0.0; } k++;
    }
  }

  qr(EPS,n,eval,Eval);

}


void matrix::eigen2(double EPS,vector<double>& Eval,matrix& Evec){
//-------------------------------------------------------------
// This is practically the same version as eigen1, but we also 
// keep track of the transformation matrixes - so to compute all
// eigenvectors
// The relation is:
// this * Evec = Evec * Eval  or (because Evec.H() * Evec = I)
// Eval = Evec.H() * this * Evec
//-------------------------------------------------------------

  int n = n_rows; // = n_cols

  Evec.load_identity();
  matrix Q(n,n);
  matrix T(n,n);

  // M =  H * T * H,  H^H = H^-1 = H, here H = Evec - to save space
  tridiagonalize(T,Evec);

  // To avoid error propagation we just set the off-tridiagonal elements to 0.0
  int k = 0;
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      if(abs(i-j)>1){ T.M[k] = 0.0; } k++;
    }
  }

  qr(EPS,n,T,Eval,Q);//    T = Q.H()*Eval*Q
  Evec *= Q;

}

void matrix::eigen3(double EPS,vector<double>& Eval,matrix& Evec){
//-------------------------------------------------------------
// This is practically the same version as eigen1, but we also 
// keep track of the transformation matrixes - so to compute all
// eigenvectors
// The relation is:
// this * Evec = Evec * Eval  or (because Evec.H() * Evec = I)
// Eval = Evec.H() * this * Evec
//-------------------------------------------------------------

  int n = n_rows; // = n_cols

  matrix X(n,1);
  complex<double> gs( (1.0/(sqrt(2.0)*n)), (1.0/(sqrt(2.0)*n)) );

  matrix m(n,n); m = *this;

  m.eigen1(EPS,Eval); // compute all eigenvalues


  for(int i=0;i<n;i++){

    // Instead of : A = m-Eval[i]*I;
    for(int j=0;j<n;j++){ m.M[j*n+j] -= Eval[i]; }
    // Initial guess
    for(j=0;j<n;j++){ X.M[j] = gs; }

    solve_linsys1(m,X,EPS,10000,1.4); // ~1.4 is optimum

    // Restore original m
    for(j=0;j<n;j++){ m.M[j*n+j] += Eval[i]; }

    // Compute norm of the solution vector
    double nrm = 0.0;
    for(j=0;j<n;j++){ nrm += norm(X.M[j]); }
    nrm = sqrt(1.0/nrm);
 
    // Normalize solution vector
    for(j=0;j<n;j++){ Evec.M[j*n+i] = nrm*X.M[j]; }

  }//for i

}



void matrix::tridiagonalize(matrix& T){
/****************************************************************************
  Here we will transfrom the matrix to tridiagonal form (T) using the Householder
  transformations. We assume our matrix is Hermitian or symmetric, otherwise
  the algorithm may not work

  General identity behind Householder:
  Y = P * X
  then P = I - 2*W*W^T
  W = (Y - X)/|Y-X|
  http://math.fullerton.edu/mathews/n2003/HouseholderMod.html

  This algorithm scales as O(N^3)
*****************************************************************************/
  int i,j,k,n;
  double nrm;
  complex<double> alp;

  n = n_rows; // = n_cols


  matrix w(n,1); 
  matrix v(n,1);

  for(i=0;i<n_elts;i++){ T.M[i] = M[i]; }

  for(i=0;i<(n-2);i++){

    w = 0.0;
    nrm = 0.0; for(j=i+1;j<n;j++){ nrm += (std::conj(T.M[j*n+i])*T.M[j*n+i]).real(); }  nrm = sqrt(nrm);

    if(abs(T.M[(i+1)*n+i])==0.0){ alp = complex<double>(1.0,0.0); }
    else{ alp = (T.M[(i+1)*n+i] / abs(T.M[(i+1)*n+i])) ; }
    
    w.M[i+1] = T.M[(i+1)*n+i] - alp*nrm; 

    nrm = (std::conj(w.M[i+1])*w.M[i+1]).real(); // norm of new vector x-y
    for(j=i+2;j<n;j++){ w.M[j] = T.M[j*n+i]; nrm += (std::conj(w.M[j])*w.M[j]).real(); }
    nrm = sqrt(nrm);

    // Normalize new vector (w)
    for(j=i+1;j<n;j++){ w.M[j] = w.M[j] / nrm; }
    
    // The following commented lines are only for mathematical and historical reason
    // Finally, projector
    // P = iden - 2.0 * w * (w.H());      
    // Do the transformation of the matrix
    // This is basic algorithm
    //T = P * T * P; // P = P.H = P^-1   // <-- this is most costly (both memory and time) place!

    // Let's optimize it:
    v = T * w;    
    T =  (T - 2.0*w*(v.H()) - 2.0*v*(w.H()) + 4.0*(w.H()*v).M[0]*w*(w.H()) );
    

  }// for i - transformation index

}


void matrix::tridiagonalize(matrix& T,matrix& H){
/****************************************************************************
  Here we will transfrom the matrix to tridiagonal form (T) using the Householder
  transformations. We assume our matrix is Hermitian or symmetric, otherwise
  the algorithm may not work

  The full transformation is kept in matrix H

  General identity behind Householder:
  Y = P * X
  then P = I - 2*W*W^T
  W = (Y - X)/|Y-X|
  http://math.fullerton.edu/mathews/n2003/HouseholderMod.html

  This algorithm scales as O(N^3)
*****************************************************************************/
  int i,j,k,n;
  double nrm;
  complex<double> alp;

  n = n_rows; // = n_cols

  matrix I(n,n);  I.load_identity();
  matrix tmp1(n,n);
  matrix tmp2(n,n);
  matrix P(n,n);
  H.load_identity();
  matrix w(n,1); 
  matrix v(n,1);

  for(i=0;i<n_elts;i++){ T.M[i] = M[i]; }

  for(i=0;i<(n-2);i++){

    w = 0.0;
    nrm = 0.0; for(j=i+1;j<n;j++){ nrm += (std::conj(T.M[j*n+i])*T.M[j*n+i]).real(); }  nrm = sqrt(nrm);

    if(abs(T.M[(i+1)*n+i])==0.0){ alp = complex<double>(1.0,0.0); }
    else{ alp = (T.M[(i+1)*n+i] / abs(T.M[(i+1)*n+i])) ; }
    
    w.M[i+1] = T.M[(i+1)*n+i] - alp*nrm; 

    nrm = (std::conj(w.M[i+1])*w.M[i+1]).real(); // norm of new vector x-y
    for(j=i+2;j<n;j++){ w.M[j] = T.M[j*n+i]; nrm += (std::conj(w.M[j])*w.M[j]).real(); }
    nrm = sqrt(nrm);

    // Normalize new vector (w)
    for(j=i+1;j<n;j++){ w.M[j] = w.M[j] / nrm; }
    
    // The following commented lines are only for mathematical and historical reason
    // Finally, projector
    // P = iden - 2.0 * w * (w.H());      
    // Do the transformation of the matrix
    // This is basic algorithm
    //T = P * T * P; // P = P.H = P^-1   // <-- this is most costly (both memory and time) place!

    // Let's optimize it:
    v = T * w;    
    P = w*(w.H());

//  The following 2 lines are what we actually doing:
    T =  (T - 2.0*w*(v.H()) - 2.0*v*(w.H()) + 4.0*(w.H()*v).M[0]*P );
    P *= -2.0;
    P += I;
    H = H * P;  

  }// for i - transformation index

}



matrix exp(matrix& m1,complex<double> scl,double eps){
/****************************************************************************
  Computes  exp(m1*scl)
  Works only for Hermitian m1: m1.H() = m1
*****************************************************************************/
  if(m1.n_rows!=m1.n_cols){ cout<<"Error in exp: Can not exponentiate non-square matrix\n"; exit(0); }
  int n = m1.n_rows;
  matrix evec(n,n),eval(n,n);//,inv_evec(n,n);

  m1.eigen(eps,eval,evec,2);
  for(int i=0;i<n;i++){  eval.M[i*n+i] = exp(eval.M[i*n+i].real()*scl); }
  //evec.direct_inverse(eps,inv_evec);  inv_evec = evec.H()
  return (evec*eval*evec.H());
}
 

matrix sin(matrix& m1,complex<double> scl, double eps){
/****************************************************************************
  Computes sin(m1*scl)
  Works only for Hermitian m1: m1.H() = m1
*****************************************************************************/
  if(m1.n_rows!=m1.n_cols){ cout<<"Error in exp: Can not exponentiate non-square matrix\n"; exit(0); }
  int n = m1.n_rows;
  matrix evec(n,n),eval(n,n);//,inv_evec(n,n);

  m1.eigen(eps,eval,evec,2);
  for(int i=0;i<n;i++){  eval.M[i*n+i] = sin(eval.M[i*n+i].real()*scl); }
//  evec.direct_inverse(eps,inv_evec);
  return (evec*eval*evec.H());
}

matrix cos(matrix& m1,complex<double> scl, double eps){
/****************************************************************************
  Computes cos(m1*scl)
  Works only for Hermitian m1: m1.H() = m1
*****************************************************************************/
  if(m1.n_rows!=m1.n_cols){ cout<<"Error in exp: Can not exponentiate non-square matrix\n"; exit(0); }
  int n = m1.n_rows;
  matrix evec(n,n),eval(n,n);//,inv_evec(n,n);

  m1.eigen(eps,eval,evec,2);
  for(int i=0;i<n;i++){  eval.M[i*n+i] = cos(eval.M[i*n+i].real()*scl); }
//  evec.direct_inverse(eps,inv_evec);
  return (evec*eval*evec.H());
}

matrix pow(matrix& m1,double nn, double eps){
/****************************************************************************
  Computes pow(m1,nn). In particular if nn = 1/2 result is sqrt(m1)
  Works only for Hermitian m1: m1.H() = m1
*****************************************************************************/
  if(m1.n_rows!=m1.n_cols){ cout<<"Error in exp: Can not exponentiate non-square matrix\n"; exit(0); }
  int n = m1.n_rows;
  matrix evec(n,n),eval(n,n);//,inv_evec(n,n);

  m1.eigen(eps,eval,evec,2);
  for(int i=0;i<n;i++){  eval.M[i*n+i] = pow(eval.M[i*n+i].real(),nn); }
//  evec.direct_inverse(eps,inv_evec);
  return (evec*eval*evec.H());
}




void matrix::inverse(double EPS,matrix& INV,int opt){

  if(opt==1){ direct_inverse(EPS,INV); }  // this is much faster way - works fine for ~n = 350 and more
  else if(opt==2){                        // actually this is slower version - works only up ~n = 100
    matrix I(n_rows,n_cols); I.load_identity();
    solve_linsys(*this, I, INV, EPS, 10000, 1.4); // CX = D, so if D = I => X = C^-1  
  }

}



void matrix::direct_inverse(double EPS,matrix& INV){
  int num_of_rows = n_rows;
  int num_of_cols = n_cols;

  double zero = EPS;
  complex<double> *R_time;   R_time=new complex<double>[num_of_rows*num_of_cols];
  complex<double> *L_time;   L_time=new complex<double>[num_of_rows*num_of_cols];

  for(int k=0;k<num_of_rows*num_of_cols;k++){ R_time[k]=M[k]; }

  k=0;
  for(int i=0;i<num_of_cols;i++){
    for(int j=0;j<num_of_cols;j++){
      if(i==j) {L_time[k]=complex<double>(1.0,0.0);}
      else     {L_time[k]=complex<double>(0.0,0.0);}
      k++;
    }// for j
  }// for i


  complex<double> alpha;
  for(int row1=0;row1<num_of_rows-1;row1++){
    if(abs(R_time[row1*num_of_cols+row1])<=zero){
      int row=row1+1;
      while(abs(R_time[row*num_of_cols+row1])<=zero){ row++;}
      complex<double> temp1,temp2;
      for(int col=0;col<num_of_cols;col++){
        temp1=R_time[row1*num_of_cols+col];
        R_time[row1*num_of_cols+col]=R_time[row*num_of_cols+col];
        R_time[row*num_of_cols+col]=temp1;

        temp2=L_time[row1*num_of_cols+col];
        L_time[row1*num_of_cols+col]=L_time[row*num_of_cols+col];
        L_time[row*num_of_cols+col]=temp2;
      }// for col
    }// if

    if(abs(R_time[row1*num_of_cols+row1])>zero){
      for(int row2=row1+1;row2<num_of_rows;row2++){
        if(abs(R_time[row2*num_of_cols+row1])>zero){
          alpha=-R_time[row2*num_of_cols+row1]/R_time[row1*num_of_cols+row1];

          for(int col=0;col<num_of_cols;col++){
            R_time[row2*num_of_cols+col]=R_time[row2*num_of_cols+col]+alpha*R_time[row1*num_of_cols+col];
            L_time[row2*num_of_cols+col]=L_time[row2*num_of_cols+col]+alpha*L_time[row1*num_of_cols+col];
          }// for col
        }// if !=0
        else continue;
      }// for row2
    }// if !=0
  }// for row1

  for(row1=num_of_rows-1;row1>0;row1--){
    alpha=R_time[row1*num_of_cols+row1];
    R_time[row1*num_of_cols+row1]=complex<double>(1.0,0.0);

    for(int col=(num_of_cols-1);col>=0;col--){ 
      L_time[row1*num_of_cols+col]=L_time[row1*num_of_cols+col]/alpha;
    }// for col
    for(int row2=row1-1;row2>=0;row2--){
      alpha=-R_time[row2*num_of_cols+row1];
      for(int col=(num_of_cols-1);col>=0;col--){
        R_time[row2*num_of_cols+col]=R_time[row2*num_of_cols+col]+alpha*R_time[row1*num_of_cols+col];
        L_time[row2*num_of_cols+col]=L_time[row2*num_of_cols+col]+alpha*L_time[row1*num_of_cols+col];
      }// for col
    }// for row2
  }// for row1

  alpha=R_time[0];
  R_time[0]=complex<double>(1.0,0.0);
  for(int col=(num_of_cols-1);col>=0;col--){   L_time[col]=L_time[col]/alpha;  }

  k=0;
  for(int row=0;row<num_of_rows;row++){
    for(int col=0;col<num_of_cols;col++){
      INV.M[row*num_of_cols+col] = L_time[k];
      k++;
    }
  }
  delete [] R_time;
  delete [] L_time;
}


void solve_linsys(matrix& C,matrix& D, matrix& X,double eps,int maxiter,double omega){
/*********************************************
 Here we solve the system of linear equations
      CX = D  --->     AX = D', where  A = C.T()*C   D' = C.T()*D
 using Gauss-Seidel iterative procedure

 Inputs: A, D - matrices
         eps  - precision criterion
         omega - parameter of overrelaxation - must be in range (1,2)
                 to accelerate convergence
 Output: X

 Some preliminary transformations are made in order
 to be able to use Gauss-Seidel method for any matrix A

 More details:
 80.47 An iterative Algorithm for Matrix Inversion
 Which is Always Convergent
 Authors: S. Simons
 Source: The Mathematical Gazette, Vol. 80, No. 489
 (Nov., 1996), pp. 567-569

 url: http://www.jstor.org/stable/pdfplus/3618529.pdf

**********************************************/

// Do the transformations A = C^H * C and b = C^H * d
// If matrices d and c have more then 1 columns we do the
// procedure for each column


    int i,j,k;   // counters
    int n,m,p;   // dimetions
    complex<double> s;    // sums
    double error;// error
    int iter;    // number of iterations

    if(C.n_rows!=D.n_rows)
        {std::cout<<"Error: The number of rows of matrices C and D in equation CX = D must be equal\n"; exit(35); } // n
    if(C.n_cols!=X.n_rows)
        {std::cout<<"Error: The number of cols of matrix C and num of rows in matrix D in equation CX = D must be equal\n"; exit(35); } // m
    if(X.n_cols!=D.n_cols)
        {std::cout<<"Error: The number of cols of matrices X and D in equation CX = D must be equal\n"; exit(35); } // p

    // Set dimentions
    n = C.n_rows;
    m = C.n_cols;
    p = D.n_cols;  // this is just 1 in most of the cases

    matrix A(m,m); A = C.H() * C;       
    eps = eps*eps;
    error = 2.0*eps;
    iter = 0;

    while((error>eps)&&(iter<maxiter)){

    error = 0.0;

    for( k = 0; k < p; k++ ){

        //------- Matrix preparation step -----------

        matrix d(n,1);  for(i = 0;i<n;i++){ d.M[i] = D.M[i*p+k]; }
        matrix x(m,1);  for(i = 0;i<m;i++){ x.M[i] = X.M[i*p+k]; }
        matrix xprev(m,1); xprev = x;
        matrix b(m,1);  b = C.H() * d;

        //------- Gauss-Seidel step -----------------

        for( i = 0; i < m; i++ ){

            s = 0.0;

            for( j = 0; j < i; j++ ){

                s += A.M[i*m + j]*x.M[j];

            }// for j

            for( j = i+1; j < m; j++ ){

                s += A.M[i*m + j]*xprev.M[j];

            }// for j

            x.M[i] = (b.M[i] - s)/A.M[i*m + i];

        }// for i - all elements of vector x


        //-------- Now calculate the error and update X ---------

        for( i = 0; i < m; i++ ){

            error += ((std::conj(x.M[i*p + k] - xprev.M[i*p + k]))*(x.M[i*p + k] - xprev.M[i*p + k])).real();

            X.M[i*p + k] = omega*x.M[i] + (1.0-omega)*X.M[i*p + k]; 

        }// for i



    }// for k - all columns of D

    error = (error/double(m));

    iter++;

    }// loop over convergence


}

void solve_linsys1(matrix& C, matrix& X,double eps,int maxiter,double omega){
/*********************************************
 This is gonna be optimized version of the solve_linsys function
 for the case when D = 0

 omega - is a convergence parameter:
 x^(n+1) = omega * z^(n+1) + (1-omega)*x^n, where:
 
 z^(n+1) - is a normal Gauss-Seidel iterate (that is omega = 1)

 Here we solve the system of linear equations
      CX = D  --->     AX = D', where  A = C.T()*C   D' = C.T()*D
 using Gauss-Seidel iterative procedure

 Inputs: A, D - matrices
         eps  - precision criterion
 Output: X

 Some preliminary transformations are made in order
 to be able to use Gauss-Seidel method for any matrix A

 More details:
 80.47 An iterative Algorithm for Matrix Inversion
 Which is Always Convergent
 Authors: S. Simons
 Source: The Mathematical Gazette, Vol. 80, No. 489
 (Nov., 1996), pp. 567-569

 url: http://www.jstor.org/stable/pdfplus/3618529.pdf

**********************************************/

// Do the transformations A = C^H * C and b = C^H * d
// If matrices d and c have more then 1 columns we do the
// procedure for each column


    int i,j,k;   // counters
    int n,m,p;   // dimetions
    complex<double> s;    // sums
    complex<double> diff;
    double error;// error
    int iter;    // number of iterations

    if(C.n_cols!=X.n_rows)
        {std::cout<<"Error: The number of cols of matrix C and num of rows in matrix D in equation CX = D must be equal\n"; exit(35); } // m
    if(X.n_cols!=1)
        {std::cout<<"Error: The number of cols of matrices X and D in equation CX = D must be equal\n"; exit(35); } // p

    // Set dimentions
    n = C.n_rows;
    m = C.n_cols;
    p = 1;  // this is just 1 in most of the cases

    matrix d(n,1);
    matrix x(m,1);
    matrix xprev(m,1);
   
   
    matrix A(m,m); A = C.H() * C;       
    eps = eps*eps;
    error = 2.0*eps;
    iter = 0;

    int im; // i*m
  

    while((error>eps)&&(iter<maxiter)){

    error = 0.0;

    for( k = 0; k < p; k++ ){

        //------- Matrix preparation step -----------

        for(i = 0;i<m;i++){ xprev.M[i] = x.M[i] = X.M[i*p+k]; }

        //------- Gauss-Seidel step -----------------
        im = 0;
        for( i = 0; i < m; i++ ){

            s = 0.0;

            for( j = 0; j < i; j++ ){

                s += A.M[im + j]*x.M[j];

            }// for j

            for( j = i+1; j < m; j++ ){

                s += A.M[im + j]*xprev.M[j];

            }// for j


            x.M[i] = (-s)/A.M[im + i];


            im += m;

        }// for i - all elements of vector x

        //-------- Now calculate the error and update X ---------

        for( i = 0; i < m; i++ ){

            error += ((std::conj(x.M[i] - xprev.M[i]))*(x.M[i] - xprev.M[i])).real();

            X.M[i] = omega*x.M[i] + (1.0-omega)*X.M[i];  // k takes value of only 0, p = 1, so i*p+k = i

        }// for i



    }// for k - all columns of D

    error = error/double(m);

    iter++;

    }// loop over convergence


}


void dft(matrix& in,matrix& out){
/***************************************
  Discrete Fourier Transform
  e.g. http://en.wikipedia.org/wiki/Fast_Fourier_transform
****************************************/

  int N = in.n_elts; // <in> and <out> are the vectors with n elements: n x 1
  complex<double> f,mul;
  double arg;

  for(int k=0;k<N;k++){

    out.M[k] = 0.0;
    arg = 2.0*M_PI*k/((double)N);

    f = complex<double>(cos(arg),-sin(arg));
    mul = 1.0;

    for(int n=0;n<N;n++){
      out.M[k] += in.M[n]*mul;
      mul *= f;
    }// for j
  }// for i

}

void inv_dft(matrix& in,matrix& out){
/***************************************
  Inverse Discrete Fourier Transform
  e.g. http://en.wikipedia.org/wiki/Discrete_Fourier_transform
****************************************/

  int N = in.n_elts; // <in> and <out> are the vectors with n elements: n x 1
  complex<double> f,mul;
  double arg;

  for(int k=0;k<N;k++){

    out.M[k] = 0.0;
    arg = 2.0*M_PI*k/((double)N);

    f = complex<double>(cos(arg),sin(arg));
    mul = 1.0;

    for(int n=0;n<N;n++){
      out.M[k] += in.M[n]*mul;
      mul *= f;
    }// for j
  }// for i

  arg = 1.0/((double)N);
  
  for(k=0;k<N;k++){ out.M[k] *= arg; }

}




