/*
 * CIntAccuracy.h
 *
 *  Created on: 04.08.2009
 *      Author: kbirken
 */

#ifndef CINTACCURACY_H_
#define CINTACCURACY_H_

class CIntAccuracy {
public:
	// calc more accurate int from less accurate
	static int toCalc (int i)  { return i*ACCURACY_FACTOR; }

	// and vice versa (with correct rounding)
	static int toPrint (int i)  { return (i+ACCURACY_FACTOR/2)/ACCURACY_FACTOR; }

	// some constants
	static int C_100 ()  { return toCalc(100); }
	static int C_1000 ()  { return toCalc(1000); }

	// some operations
	static int div (int a, int b)  { return (a + (b/2)) / b; }

private:
	static const int ACCURACY_FACTOR = 10;
};

#endif /* CINTACCURACY_H_ */
