/*This file is part of the FEBio Studio source code and is licensed under the MIT license
listed below.

See Copyright-FEBio-Studio.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#pragma once

namespace HEX20
{
	// shape functions
	inline void shape(double* H, double r, double s, double t)
	{
		H[8] = 0.25*(1 - r*r)*(1 - s)*(1 - t);
		H[9] = 0.25*(1 - s*s)*(1 + r)*(1 - t);
		H[10] = 0.25*(1 - r*r)*(1 + s)*(1 - t);
		H[11] = 0.25*(1 - s*s)*(1 - r)*(1 - t);
		H[12] = 0.25*(1 - r*r)*(1 - s)*(1 + t);
		H[13] = 0.25*(1 - s*s)*(1 + r)*(1 + t);
		H[14] = 0.25*(1 - r*r)*(1 + s)*(1 + t);
		H[15] = 0.25*(1 - s*s)*(1 - r)*(1 + t);
		H[16] = 0.25*(1 - t*t)*(1 - r)*(1 - s);
		H[17] = 0.25*(1 - t*t)*(1 + r)*(1 - s);
		H[18] = 0.25*(1 - t*t)*(1 + r)*(1 + s);
		H[19] = 0.25*(1 - t*t)*(1 - r)*(1 + s);

		H[0] = 0.125*(1 - r)*(1 - s)*(1 - t) - 0.5*(H[8] + H[11] + H[16]);
		H[1] = 0.125*(1 + r)*(1 - s)*(1 - t) - 0.5*(H[8] + H[9] + H[17]);
		H[2] = 0.125*(1 + r)*(1 + s)*(1 - t) - 0.5*(H[9] + H[10] + H[18]);
		H[3] = 0.125*(1 - r)*(1 + s)*(1 - t) - 0.5*(H[10] + H[11] + H[19]);
		H[4] = 0.125*(1 - r)*(1 - s)*(1 + t) - 0.5*(H[12] + H[15] + H[16]);
		H[5] = 0.125*(1 + r)*(1 - s)*(1 + t) - 0.5*(H[12] + H[13] + H[17]);
		H[6] = 0.125*(1 + r)*(1 + s)*(1 + t) - 0.5*(H[13] + H[14] + H[18]);
		H[7] = 0.125*(1 - r)*(1 + s)*(1 + t) - 0.5*(H[14] + H[15] + H[19]);
	}

	// shape function derivatives
	inline void shape_deriv(double* Hr, double* Hs, double* Ht, double r, double s, double t)
	{
		Hr[ 8] = -0.5*r*(1 - s)*(1 - t);
		Hr[ 9] =  0.25*(1 - s*s)*(1 - t);
		Hr[10] = -0.5*r*(1 + s)*(1 - t);
		Hr[11] = -0.25*(1 - s*s)*(1 - t);
		Hr[12] = -0.5*r*(1 - s)*(1 + t);
		Hr[13] =  0.25*(1 - s*s)*(1 + t);
		Hr[14] = -0.5*r*(1 + s)*(1 + t);
		Hr[15] = -0.25*(1 - s*s)*(1 + t);
		Hr[16] = -0.25*(1 - t*t)*(1 - s);
		Hr[17] =  0.25*(1 - t*t)*(1 - s);
		Hr[18] =  0.25*(1 - t*t)*(1 + s);
		Hr[19] = -0.25*(1 - t*t)*(1 + s);

		Hr[0] = -0.125*(1 - s)*(1 - t) - 0.5*(Hr[ 8] + Hr[11] + Hr[16]);
		Hr[1] =  0.125*(1 - s)*(1 - t) - 0.5*(Hr[ 8] + Hr[ 9] + Hr[17]);
		Hr[2] =  0.125*(1 + s)*(1 - t) - 0.5*(Hr[ 9] + Hr[10] + Hr[18]);
		Hr[3] = -0.125*(1 + s)*(1 - t) - 0.5*(Hr[10] + Hr[11] + Hr[19]);
		Hr[4] = -0.125*(1 - s)*(1 + t) - 0.5*(Hr[12] + Hr[15] + Hr[16]);
		Hr[5] =  0.125*(1 - s)*(1 + t) - 0.5*(Hr[12] + Hr[13] + Hr[17]);
		Hr[6] =  0.125*(1 + s)*(1 + t) - 0.5*(Hr[13] + Hr[14] + Hr[18]);
		Hr[7] = -0.125*(1 + s)*(1 + t) - 0.5*(Hr[14] + Hr[15] + Hr[19]);
				
		Hs[ 8] = -0.25*(1 - r*r)*(1 - t);
		Hs[ 9] = -0.5*s*(1 + r)*(1 - t);
		Hs[10] = 0.25*(1 - r*r)*(1 - t);
		Hs[11] = -0.5*s*(1 - r)*(1 - t);
		Hs[12] = -0.25*(1 - r*r)*(1 + t);
		Hs[13] = -0.5*s*(1 + r)*(1 + t);
		Hs[14] = 0.25*(1 - r*r)*(1 + t);
		Hs[15] = -0.5*s*(1 - r)*(1 + t);
		Hs[16] = -0.25*(1 - t*t)*(1 - r);
		Hs[17] = -0.25*(1 - t*t)*(1 + r);
		Hs[18] =  0.25*(1 - t*t)*(1 + r);
		Hs[19] =  0.25*(1 - t*t)*(1 - r);

		Hs[0] = -0.125*(1 - r)*(1 - t) - 0.5*(Hs[ 8] + Hs[11] + Hs[16]);
		Hs[1] = -0.125*(1 + r)*(1 - t) - 0.5*(Hs[ 8] + Hs[ 9] + Hs[17]);
		Hs[2] =  0.125*(1 + r)*(1 - t) - 0.5*(Hs[ 9] + Hs[10] + Hs[18]);
		Hs[3] =  0.125*(1 - r)*(1 - t) - 0.5*(Hs[10] + Hs[11] + Hs[19]);
		Hs[4] = -0.125*(1 - r)*(1 + t) - 0.5*(Hs[12] + Hs[15] + Hs[16]);
		Hs[5] = -0.125*(1 + r)*(1 + t) - 0.5*(Hs[12] + Hs[13] + Hs[17]);
		Hs[6] =  0.125*(1 + r)*(1 + t) - 0.5*(Hs[13] + Hs[14] + Hs[18]);
		Hs[7] =  0.125*(1 - r)*(1 + t) - 0.5*(Hs[14] + Hs[15] + Hs[19]);

		Ht[ 8] = -0.25*(1 - r*r)*(1 - s);
		Ht[ 9] = -0.25*(1 - s*s)*(1 + r);
		Ht[10] = -0.25*(1 - r*r)*(1 + s);
		Ht[11] = -0.25*(1 - s*s)*(1 - r);
		Ht[12] =  0.25*(1 - r*r)*(1 - s);
		Ht[13] =  0.25*(1 - s*s)*(1 + r);
		Ht[14] =  0.25*(1 - r*r)*(1 + s);
		Ht[15] =  0.25*(1 - s*s)*(1 - r);
		Ht[16] = -0.5*t*(1 - r)*(1 - s);
		Ht[17] = -0.5*t*(1 + r)*(1 - s);
		Ht[18] = -0.5*t*(1 + r)*(1 + s);
		Ht[19] = -0.5*t*(1 - r)*(1 + s);
				
		Ht[0] = -0.125*(1 - r)*(1 - s) - 0.5*(Ht[ 8] + Ht[11] + Ht[16]);
		Ht[1] = -0.125*(1 + r)*(1 - s) - 0.5*(Ht[ 8] + Ht[ 9] + Ht[17]);
		Ht[2] = -0.125*(1 + r)*(1 + s) - 0.5*(Ht[ 9] + Ht[10] + Ht[18]);
		Ht[3] = -0.125*(1 - r)*(1 + s) - 0.5*(Ht[10] + Ht[11] + Ht[19]);
		Ht[4] =  0.125*(1 - r)*(1 - s) - 0.5*(Ht[12] + Ht[15] + Ht[16]);
		Ht[5] =  0.125*(1 + r)*(1 - s) - 0.5*(Ht[12] + Ht[13] + Ht[17]);
		Ht[6] =  0.125*(1 + r)*(1 + s) - 0.5*(Ht[13] + Ht[14] + Ht[18]);
		Ht[7] =  0.125*(1 - r)*(1 + s) - 0.5*(Ht[14] + Ht[15] + Ht[19]);
	}

	// iso-parametric coordinates of nodes (for n = -1 return center coordinates)
	inline void iso_coord(int n, double q[3])
	{
		switch (n)
		{
        case -1: q[0] = 0; q[1] = 0; q[2] = 0; break;
		case  0: q[0] = -1; q[1] = -1; q[2] = -1; break;
		case  1: q[0] =  1; q[1] = -1; q[2] = -1; break;
		case  2: q[0] =  1; q[1] =  1; q[2] = -1; break;
		case  3: q[0] = -1; q[1] =  1; q[2] = -1; break;
		case  4: q[0] = -1; q[1] = -1; q[2] =  1; break;
		case  5: q[0] =  1; q[1] = -1; q[2] =  1; break;
		case  6: q[0] =  1; q[1] =  1; q[2] =  1; break;
		case  7: q[0] = -1; q[1] =  1; q[2] =  1; break;
		case  8: q[0] =  0; q[1] = -1; q[2] = -1; break;
		case  9: q[0] =  1; q[1] =  0; q[2] = -1; break;
		case 10: q[0] =  0; q[1] =  1; q[2] = -1; break;
		case 11: q[0] = -1; q[1] =  0; q[2] = -1; break;
		case 12: q[0] =  0; q[1] = -1; q[2] =  1; break;
		case 13: q[0] =  1; q[1] =  0; q[2] =  1; break;
		case 14: q[0] =  0; q[1] =  1; q[2] =  1; break;
		case 15: q[0] = -1; q[1] =  0; q[2] =  1; break;
		case 16: q[0] = -1; q[1] = -1; q[2] =  0; break;
		case 17: q[0] =  1; q[1] = -1; q[2] =  0; break;
		case 18: q[0] =  1; q[1] =  1; q[2] =  0; break;
		case 19: q[0] = -1; q[1] =  1; q[2] =  0; break;
		}
	}
}
