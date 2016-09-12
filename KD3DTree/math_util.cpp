#include "const.h"
#include "math_util.h"

namespace math
{
	///<summary>Calculates square of the distance from the point to the triangle</summary>
	///<remarks>In : A - 1st point of the triangle</remarks>
	///<remarks>In : B - 2nd point of the triangle</remarks>
	///<remarks>In : C - 3rd point of the triangle</remarks>
	///<remarks>In : P - The point for which we are looking for distance to the segment</remarks>
	///<returns>Square of the distance between the point and the triangle</returns>
	double distToTriangle2(const D3& A, const D3& B, const D3& C, const D3& P)
	{
		const D3 E0 = B - A;
		const D3 E1 = C - A;

		const D3 D = A - P;
		const double a = E0 * E0;

		if (a < cEps2)
			return math::distToSeg2(B, C, P);

		const double b = E0 * E1;
		const double c = E1 * E1;

		if (c < cEps2)
			return math::distToSeg2(B, C, P);

		const double d = E0 * D;
		const double e = E1 * D;
		const double f = D * D;

		const double det = a * c - b * b;

		if (det < cEps2)
		{
			if ((B - C).norm2() < cEps2)
				return math::distToSeg2(A, C, P);
			else
				return math::distToSeg2(B, C, P);
		}

		double s = b * e - c * d;
		double t = b * d - a * e;

		double sqrDistance = 0.;

		if (s + t <= det)
		{
			if (s < 0.)
			{
				if (t < 0.)
				{
					if (d < 0.)
					{
						t = 0;
						if (-d >= a)
						{
							s = 1.;
							sqrDistance = a + 2. * d + f;
						}
						else
						{
							s = -d / a;
							sqrDistance = d * s + f;
						}
					}
					else
					{
						s = 0.;
						if (e >= 0.)
						{
							t = 0.;
							sqrDistance = f;
						}
						else
						{
							if (-e >= c)
							{
								t = 1.;
								sqrDistance = c + 2. * e + f;
							}
							else
							{
								t = -e / c;
								sqrDistance = e * t + f;
							}
						}
					}
				}
				else
				{
					s = 0.;
					if (e >= 0.)
					{
						t = 0.;
						sqrDistance = f;
					}
					else
					{
						if (-e >= c)
						{
							t = 1.;
							sqrDistance = c + 2. * e + f;
						}
						else
						{
							t = -e / c;
							sqrDistance = e * t + f;
						}
					}
				}
			}
			else
			{
				if (t < 0.)
				{
					t = 0.;
					if (d >= 0.)
					{
						s = 0.;
						sqrDistance = f;
					}
					else
					{
						if (-d >= a)
						{
							s = 1.;
							sqrDistance = a + 2. * d + f;
						}
						else
						{
							s = -d / a;
							sqrDistance = d * s + f;
						}
					}
				}
				else
				{
					s = s / det;
					t = t / det;
					sqrDistance = s * (a * s + b * t + 2. * d) + t * (b * s + c * t + 2. * e) + f;
				}
			}
		}
		else
		{
			if (s < 0.)
			{
				const double tmp0 = b + d;
				const double tmp1 = c + e;
				if (tmp1 > tmp0)
				{
					const double numer = tmp1 - tmp0;
					const double denom = a - 2. * b + c;
					if (numer >= denom)
					{
						s = 1.;
						t = 0.;
						sqrDistance = a + 2. * d + f;
					}
					else
					{
						s = numer / denom;
						t = 1 - s;
						sqrDistance = s  *(a * s + b * t + 2. * d) + t * (b * s + c * t + 2. * e) + f;
					}
				}
				else
				{
					s = 0.;
					if (tmp1 <= 0.)
					{
						t = 1.;
						sqrDistance = c + 2. * e + f;
					}
					else
					{
						if (e >= 0)
						{
							t = 0.;
							sqrDistance = f;
						}
						else
						{
							t = -e / c;
							sqrDistance = e * t + f;
						}
					}
				}
			}
			else
			{
				if (t < 0.)
				{
					const double tmp0 = b + e;
					const double tmp1 = a + d;
					if (tmp1 > tmp0)
					{
						const double numer = tmp1 - tmp0;
						const double denom = a - 2. * b + c;
						if (numer >= denom)
						{
							t = 1.;
							s = 0.;
							sqrDistance = c + 2. * e + f;
						}
						else
						{
							t = numer / denom;
							s = 1. - t;
							sqrDistance = s * (a * s + b * t + 2. * d) + t * (b * s + c * t + 2. * e) + f;
						}
					}
					else
					{
						t = 0.;
						if (tmp1 <= 0.)
						{
							s = 1.;
							sqrDistance = a + 2. * d + f;
						}
						else
						{
							if (d >= 0.)
							{
								s = 0.;
								sqrDistance = f;
							}
							else
							{
								s = -d / a;
								sqrDistance = d * s + f;
							}
						}
					}
				}
				else
				{
					const double numer = c + e - b - d;
					if (numer <= 0.)
					{
						s = 0.;
						t = 1.;
						sqrDistance = c + 2. * e + f;
					}
					else
					{
						const double denom = a - 2. * b + c;
						if (numer >= denom)
						{
							s = 1.;
							t = 0.;
							sqrDistance = a + 2. * d + f;
						}
						else
						{
							s = numer / denom;
							t = 1 - s;
							sqrDistance = s * (a * s + b * t + 2. * d) + t * (b * s + c * t + 2. * e) + f;
						}
					}
				}
			}
		}

		if (sqrDistance < 0.)
			return 0.;

		return sqrDistance;
	}

	///<summary>Calculates distance from the point to the triangle</summary>
	///<remarks>In : A - 1st point of the triangle</remarks>
	///<remarks>In : B - 2nd point of the triangle</remarks>
	///<remarks>In : C - 3rd point of the triangle</remarks>
	///<remarks>In : P - The point for which we are looking for distance to the segment</remarks>
	///<returns>Distance between the point and the triangle</returns>
	double distToTriangle(const D3& A, const D3& B, const D3& C, const D3& P)
	{
		return sqrt(distToTriangle2(A, B, C, P));
	}
};