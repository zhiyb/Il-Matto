#include "ts_calibrate.h"

uint8_t perform_calibration(calibration *cal)
{
	uint8_t j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

// Get sums for matrix
	n = x = y = x2 = y2 = xy = 0;
	for(j=0;j<5;j++) {
		n += 1.0;
		x += (float)cal->x[j];
		y += (float)cal->y[j];
		x2 += (float)((uint32_t)cal->x[j]*(uint32_t)cal->x[j]);
		y2 += (float)((uint32_t)cal->y[j]*(uint32_t)cal->y[j]);
		xy += (float)((uint32_t)cal->x[j]*(uint32_t)cal->y[j]);
	}

// Get determinant of matrix -- check if determinant is too small
	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if(det < 0.1 && det > -0.1) {
		//printf("ts_calibrate: determinant is too small -- %f\n",det);
		return 0;
	}

// Get elements of inverse matrix
	a = (x2*y2 - xy*xy)/det;
	b = (xy*y - x*y2)/det;
	c = (x*xy - y*x2)/det;
	e = (n*y2 - y*y)/det;
	f = (x*y - n*xy)/det;
	i = (n*x2 - x*x)/det;

// Get sums for x calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal->xfb[j];
		zx += (float)((uint32_t)cal->xfb[j]*(uint32_t)cal->x[j]);
		zy += (float)((uint32_t)cal->xfb[j]*(uint32_t)cal->y[j]);
	}

// Now multiply out to get the calibration for framebuffer x coord
	cal->a[0] = (int32_t)((a*z + b*zx + c*zy)*(scaling));
	cal->a[1] = (int32_t)((b*z + e*zx + f*zy)*(scaling));
	cal->a[2] = (int32_t)((c*z + f*zx + i*zy)*(scaling));

#if 0
	printf("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));
#endif

// Get sums for y calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal->yfb[j];
		zx += (float)((uint32_t)cal->yfb[j]*(uint32_t)cal->x[j]);
		zy += (float)((uint32_t)cal->yfb[j]*(uint32_t)cal->y[j]);
	}

// Now multiply out to get the calibration for framebuffer y coord
	cal->a[3] = (int32_t)((a*z + b*zx + c*zy)*(scaling));
	cal->a[4] = (int32_t)((b*z + e*zx + f*zy)*(scaling));
	cal->a[5] = (int32_t)((c*z + f*zx + i*zy)*(scaling));

#if 0
	printf("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));
#endif

// If we got here, we're OK, so assign scaling to a[6] and return
	cal->a[6] = (int32_t)scaling;
	return 1;
/*	
// This code was here originally to just insert default values
	for(j=0;j<7;j++) {
		c->a[j]=0;
	}
	c->a[1] = c->a[5] = c->a[6] = 1;
	return 1;
*/

}

#if 0
int main(void)
{
	if (perform_calibration (&cal)) {
		printf ("Calibration constants: ");
		for (i = 0; i < 7; i++) printf("%d ", cal.a [i]);
		printf("\n");
		if ((calfile = getenv("TSLIB_CALIBFILE")) != NULL) {
			cal_fd = open (calfile, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
		} else {
			cal_fd = open ("/etc/pointercal", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
		}
		sprintf (cal_buffer,"%d %d %d %d %d %d %d",
			 cal.a[1], cal.a[2], cal.a[0],
			 cal.a[4], cal.a[5], cal.a[3], cal.a[6]);
		write (cal_fd, cal_buffer, strlen (cal_buffer) + 1);
		close (cal_fd);
                i = 0;
	}
}
#endif
