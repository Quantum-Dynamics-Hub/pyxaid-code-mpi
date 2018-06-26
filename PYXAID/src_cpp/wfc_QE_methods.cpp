/***********************************************************
 * Copyright (C) 2013 Alexey V. Akimov
 * This file is distributed under the terms of the
 * GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * http://www.gnu.org/copyleft/gpl.txt
***********************************************************/

#include "wfc.h"
#include "aux.h"
#include "io.h"

void wfc::QE_read_binary_wfc(std::string filename,int _nkpts,int _nbands,int _tot_npw){

  ifstream::pos_type size;
  char * memblock;

  ifstream file;
  file.open(filename.c_str(), ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    // First - read all file
    size = file.tellg();
    memblock = new char [size];
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();

    // Read first record
    int pos = 0;  // position in file (in bytes)
    double tmp;
    complex<double> c;

    nkpts = _nkpts;
    nbands = _nbands;
    int ntot_pw = size / sizeof(complex<double> );
    cout<<"ntot_pw = "<<ntot_pw<<endl;

//    for(int i=0;i<10;i++){
//      get_value< complex<double> >(c,memblock,pos);
//      cout<<c<<endl;
//    }

    kpts = std::vector<K_point>(nkpts,K_point());
    for(int ikpt=0;ikpt<nkpts;ikpt++){
      kpts[ikpt].nbands = nbands;
      kpts[ikpt].mo = std::vector<MO>(nbands,MO());

      for(int iband=0;iband<kpts[ikpt].nbands;iband++){
      //=============== iband-th sub-sub-record ================
        kpts[ikpt].mo[iband].npw = 4268 / nbands ; // check what tha 4268 number means!!!
        kpts[ikpt].mo[iband].coeff = std::vector< complex<double> >(kpts[ikpt].mo[iband].npw,std::complex<double>(0.0,0.0));
        for(int i=0;i<kpts[ikpt].mo[iband].npw;i++){
          get_value< std::complex<double> >(c,memblock,pos); kpts[ikpt].mo[iband].coeff[i] = c;
        }
//        pos += ((int)rdum - kpts[ikpt].mo[iband].npw*sizeof( std::complex<float>));
      }// for bands

    }// for ikpt

    // Free memory
    delete[] memblock;

  }// if file is open
  else{ cout << "Unable to open file"<<filename<<"\n"; }

}



void wfc::QE_read_acsii_wfc(std::string filename){

  int verbose = 1;
  // Read all lines
  vector<std::string> A;
  int filesize = read_file(filename,1,A);

  // Find k-point sections
  vector<int> beg,end;
  nkpts = 0;
  int start = 0;
  int status = 1;
  while(status!=0){
    int b,e;
    status = find_section(A,"<Kpoint.","</Kpoint.",start,filesize,b,e);
    if(status==1){ nkpts++; start = e; beg.push_back(b); end.push_back(e); }
  }
  if(verbose){  cout<<"Number of K-points = "<<nkpts<<endl; }
 
  
  // Now find the positions of the bands for all k-points
  vector< vector<int> > kbeg( nkpts,vector<int>() );
  vector< vector<int> > kend( nkpts,vector<int>() );


  for(int k=0;k<nkpts;k++){
    start = beg[k]; status = 1;int knbnd = 0;

    while(status!=0){
      int b,e;
      status = find_section(A,"<Wfc.","</Wfc.",start,end[k],b,e);
      if(status==1) { start = e+1; knbnd++; kbeg[k].push_back(b); kend[k].push_back(e);}
    }
    if(verbose){ cout<<"Number of bands for k-point "<<k<<" is = "<<knbnd<<endl; }
  }// for k

  if(is_allocated==0){
    // Construct MOs:
    nbands = kbeg[0].size();                   // Number of bands(MOs) in each k-point
    npw =  kend[0][0] - kbeg[0][0] - 1;        // Number of plane waves in each band
    kpts = vector<K_point>(nkpts,K_point(nbands,npw));
    is_allocated = 3;
  }

  // Now finally get the coefficients 
  for(k=0;k<nkpts;k++){
    for(int band=0;band<nbands;band++){
      for(int pw=0;pw<npw;pw++){

        int i = kbeg[k][band]+1+pw;

        vector<std::string> At;
        split_line2(A[i],At,',');
        double re = atof(At[0].c_str());
        double im = atof(At[1].c_str());

        kpts[k].mo[band].coeff[pw] = complex<double>(re,im);

      }//for npw
    }//for band
  }// for k
 

  // Free the memory
  A.clear();

}

void wfc::QE_read_acsii_grid(std::string filename){

  int verbose = 1;
  // Read all lines
  vector<std::string> A;
  int filesize = read_file(filename,1,A);

  // Find k-point sections
  int beg,end;
  int start = 0;
  int status = 0;
  status = find_section(A,"<grid","</grid>",start,filesize,beg,end);
  int sz = end-beg-1;
  if(verbose){  cout<<"Size of the grid = "<<sz<<endl; }


//  for(int i=0;i<sz;i++){ grid.push_back(gp); }
  grid = vector<vector<int> >(sz,vector<int>(3,0));
  grid.reserve(int(sz+10));

  // Read all grid points now
  for(int i=0;i<sz;i++){
    vector<std::string> At;
    split_line(A[i+beg+1],At);

    grid[i][0] = atoi(At[0].c_str());
    grid[i][1] = atoi(At[1].c_str());
    grid[i][2] = atoi(At[2].c_str());

  }// for i

  // Free memory
  A.clear();
  
}

void wfc::aux_line2vec(string line,vector<double>& a){
// Speciall auxiliary function to conver line with 3 numbers to
// 3 distinct numbers in the array a
  vector<string> tmp; 
  split_line(line,tmp);
  if(a.size()>0) { a.clear(); }
  a = vector<double>(3,0.0);
  a[0] = atof(tmp[0].c_str());
  a[1] = atof(tmp[1].c_str());
  a[2] = atof(tmp[2].c_str());

}

void wfc::QE_read_acsii_index(std::string filename){

  int verbose = 1;
  // Read all lines
  vector<std::string> A;
  int filesize = read_file(filename,1,A);
 
  // Get some information from the file
  int beg,end; beg = end = 0;
  int status = 0;
  size_t pos; string str;

  //-------------------- Set parameters to default values ------------
  nspin = -1;
  gamma_only = -1;
  natoms = -1; 
 
  //--------------------- Dimensions --------------------------------
  status = find_section(A,"<Dimensions>","</Dimensions>",0,filesize,beg,end);

  if(status){
    for(int i=beg;i<end;i++){
      pos = A[i].find("<Kpoints"); if(pos!=string::npos){ str = extract_s(A[i],"nktot"); nkpts = atoi(str.c_str()); }
      pos = A[i].find("<Kpoints"); if(pos!=string::npos){ str = extract_s(A[i],"nspin"); nspin = atoi(str.c_str()); }
      pos = A[i].find("<Wfc_grid");if(pos!=string::npos){ str = extract_s(A[i],"npwx"); npw = atoi(str.c_str()); }
      pos = A[i].find("<Bands"); if(pos!=string::npos){ str =  extract_s(A[i],"nbnd"); nbands = atoi(str.c_str()); }
      pos = A[i].find("<Gamma_tricks");if(pos!=string::npos){ str =  extract_s(A[i],"gamma_only"); if(str=="F"){ gamma_only=0; }else{ gamma_only=1;} }
      pos = A[i].find("<Atoms"); if(pos!=string::npos){ str =  extract_s(A[i],"natoms"); natoms = atoi(str.c_str()); }
    }// for i
  }// if status

  //------------------------ Cell ----------------------------------
  beg = end = 0;
  status = find_section(A,"<Cell","</Cell>",0,filesize,beg,end);

  if(status){
    for(int i=beg;i<end;i++){
      pos = A[i].find("<Cell"); if(pos!=string::npos){ str =  extract_s(A[i],"units"); cell_units = str; }
      pos = A[i].find("<Data"); if(pos!=string::npos){ str =  extract_s(A[i],"alat"); alat = atof(str.c_str()); }
      pos = A[i].find("<Data"); if(pos!=string::npos){ str =  extract_s(A[i],"omega"); omega = atof(str.c_str()); }
      pos = A[i].find("<Data"); if(pos!=string::npos){ str =  extract_s(A[i],"tpiba"); tpiba = atof(str.c_str()); }

      pos = A[i].find("<a1"); if(pos!=string::npos){ str =  extract_s(A[i],"xyz"); aux_line2vec(str,a1);  }
      pos = A[i].find("<a2"); if(pos!=string::npos){ str =  extract_s(A[i],"xyz"); aux_line2vec(str,a2); }
      pos = A[i].find("<a3"); if(pos!=string::npos){ str =  extract_s(A[i],"xyz"); aux_line2vec(str,a3); }
      pos = A[i].find("<b1"); if(pos!=string::npos){ str =  extract_s(A[i],"xyz"); aux_line2vec(str,b1); }
      pos = A[i].find("<b2"); if(pos!=string::npos){ str =  extract_s(A[i],"xyz"); aux_line2vec(str,b2); }
      pos = A[i].find("<b3"); if(pos!=string::npos){ str =  extract_s(A[i],"xyz"); aux_line2vec(str,b3); }

    }// for i
  }// if status

  //---------------------- Eigenvalues ----------------------------
  beg = end = 0;
  status = find_section(A,"<Eigenvalues","</Eigenvalues>",0,filesize,beg,end);

  if(status){
    for(int i=beg;i<end;i++){
      pos = A[i].find("<Eigenvalues"); if(pos!=string::npos){ str =  extract_s(A[i],"efermi"); efermi = atof(str.c_str()); }
      pos = A[i].find("<Eigenvalues"); if(pos!=string::npos){ energy_units =  extract_s(A[i],"units");  }
    }// for i

  }//status


  //======================= Almost done ============================
  // Before reading the eigenvalues we will allocate the memory
  // and set the corresponding flag so do not do this when reading wavefunctions
  // and grids

  if(nkpts>0 && nbands>0 && npw>0){

    if(is_allocated==0){ // Allocate all levels
      kpts = vector<K_point>(nkpts,K_point(nbands,npw));
      is_allocated = 3;
    }

  }// if >0 >0 >0


  // Now ready to read in the eigenvalues
  if(is_allocated==3){
    for(int k=0;k<nkpts;k++){
      int beg1,end1;
      // Use latest beg and end enclosing Eigenvalues section
      status = find_section(A,"<e."+int2str(k+1),"</e."+int2str(k+1),beg,end,beg1,end1);
 
      if(status){
        for(int i=0;i<nbands;i++){
          kpts[k].mo[i].energy = atof(A[beg1+1+i].c_str());
        }// for i
      }// if status
    }// for k
  }// is_allocated==3

}

