/* ============================================================================
 Name        : BreastCancerRecord.h
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Breast Cancer Record Structure
 ============================================================================
*/

#ifndef BREAST_CANCER_RECORD_H
#define BREAST_CANCER_RECORD_H

#define NUMBER_OF_ATTRIBUTES	9
#define MALIGNANT		4
#define BENIGN			2

#include <iostream>
#include <string>
#include <list>

#define LARGEST_SIZE_T 18446744073709

using namespace std;

class BreastCancerRecord
{
	private :
		long SCM;	// id number
		int A1;		//	Clump Thickness
		int A2;		//	Uniformity of Cell Size
		int A3;		// Uniformity of Cell Shape
		int A4;		// Marginal Adhesion
		int A5;		// Single Epithelial Cell Size
		int A6;		// Bare Nuclei
		int A7;		// Bland Chromatin
		int A8;		// Normal Nucleoli
		int A9;		// Mitoses
		int A10;	// Class - Benign or Malignant
	public :
		long getSCM();	void setSCM(long);
		int getA1();	void setA1(int);
		int getA2();	void setA2(int);
		int getA3();	void setA3(int);
		int getA4();	void setA4(int);
		int getA5();	void setA5(int);
		int getA6();	void setA6(int);
		int getA7();	void setA7(int);
		int getA8();	void setA8(int);
		int getA9();	void setA9(int);
		int getA10();	void setA10(int);
};

#endif