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

namespace PYRA5 
{
	// shape functions
	inline void shape(double* H, double r, double s, double t)
	{
		H[0] = 0.125*(1.0 - r)*(1.0 - s)*(1.0 - t);
		H[1] = 0.125*(1.0 + r)*(1.0 - s)*(1.0 - t);
		H[2] = 0.125*(1.0 + r)*(1.0 + s)*(1.0 - t);
		H[3] = 0.125*(1.0 - r)*(1.0 + s)*(1.0 - t);
		H[4] = 0.5*(1.0 + t);
	}

	// shape function derivatives
	inline void shape_deriv(double* Hr, double* Hs, double* Ht, double r, double s, double t)
	{
		Hr[0] = -0.125*(1.0 - s)*(1.0 - t);
		Hr[1] =  0.125*(1.0 - s)*(1.0 - t);
		Hr[2] =  0.125*(1.0 + s)*(1.0 - t);
		Hr[3] = -0.125*(1.0 + s)*(1.0 - t);
		Hr[4] =  0.0;

		Hs[0] = -0.125*(1.0 - r)*(1.0 - t);
		Hs[1] = -0.125*(1.0 + r)*(1.0 - t);
		Hs[2] =  0.125*(1.0 + r)*(1.0 - t);
		Hs[3] =  0.125*(1.0 - r)*(1.0 - t);
		Hs[4] =  0.0;

		Ht[0] = -0.125*(1.0 - r)*(1.0 - s);
		Ht[1] = -0.125*(1.0 + r)*(1.0 - s);
		Ht[2] = -0.125*(1.0 + r)*(1.0 + s);
		Ht[3] = -0.125*(1.0 - r)*(1.0 + s);
		Ht[4] =  0.5;
	}

	// iso-parametric coordinates of nodes (for n = -1 return center coordinates)
	inline void iso_coord(int n, double q[3])
	{
		switch (n)
		{
		case -1: q[0] = 0.0; q[1] = 0.0; q[2] = 0.0; break;
		case 0: q[0] = -1.0; q[1] = -1.0; q[2] = -1.0; break;
		case 1: q[0] = 1.0; q[1] = -1.0; q[2] = -1.0; break;
		case 2: q[0] = 1.0; q[1] = 1.0; q[2] = -1.0; break;
		case 3: q[0] = -1.0; q[1] = 1.0; q[2] = -1.0; break;
		case 4: q[0] = 0.0; q[1] = 0.0; q[2] = 1.0; break;
		}
	}
}
