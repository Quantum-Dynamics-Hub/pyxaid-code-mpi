/***********************************************************
 * Copyright (C) 2013 Alexey V. Akimov
 * This file is distributed under the terms of the
 * GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * http://www.gnu.org/copyleft/gpl.txt
***********************************************************/

#ifndef AUX_H
#define AUX_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

// Auxiliary functions for various general purposes

// Operations on the the vectors of integers ("states")
void show_vector(vector<int>& A);
int is_in_vector(int a, vector<int>& A);
int num_in_vector(int a, vector<int>& A, vector<int>& indx);
int find_int(int a,vector<int>& A);
int is_repeating(vector<int>& A,int& reap);


// Operations on strings ("lines")
void split_line(std::string line, vector<std::string>& arr);
void split_line2(std::string line,vector<std::string>& arr,char delim);
std::string int2str(int inp);
int find_section(vector<std::string>& A,std::string marker_beg,std::string marker_end,int min_line,int max_line,int& beg,int& end);
std::string extract_s(std::string line, std::string marker);    
std::string int2string(int);

// Operations on arrays - reformings, etc.
void extract_1D(vector<double>& in, vector<double>& out, vector<int>& templ,int shift);
void extract_2D(vector< vector<double> >& in, vector< vector<double> >& out, int minx,int maxx, int miny, int maxy );
void extract_2D(vector< vector<double> >& in, vector< vector<double> >& out, vector<int>& templ,int shift);


#endif // AUX_H
