#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <stdio.h>
using namespace std;

#include "vector.h"
#include "ray.h"
#include "maths.h"

class Camera
{

private:
	Vector eye, at, up; 
	float fovy, vnear, vfar, plane_dist, focal_ratio, aperture;
	float w, h;
	int res_x, res_y;
	Vector u, v, n;

public:
	Vector GetEye() { return eye; }
	int GetResX()  { return res_x; }
    int GetResY()  { return res_y; }
	float GetFov() { return fovy; }
	float GetPlaneDist() { return plane_dist; }
	float GetFar() {return vfar; }
	float GetAperture() { return aperture; }

    Camera( Vector from, Vector At, Vector Up, float angle, float hither, float yon, int ResX, int ResY, float Aperture_ratio, float Focal_ratio) {
	    eye = from;
	    at = At;
	    up = Up;
	    fovy = angle;
	    vnear = hither;
	    vfar = yon;
	    res_x = ResX;
	    res_y = ResY;
		focal_ratio = Focal_ratio;

        // set the camera frame uvn
        n = ( eye - at );
        plane_dist = n.length();
	    n = n / plane_dist;

	    u = up % n;
	    u = u / u.length();

	    v = n % u;

        //Dimensions of the vis window
	    h = 2 * plane_dist * tan( (PI * angle / 180) / 2.0f );
        w = ( (float) res_x / res_y ) * h;  

		aperture = Aperture_ratio * (w / res_x); //Lens aperture = aperture_ratio * pixel_size

		printf("\nwidth=%f height=%f fov=%f, viewplane distance=%f, pixel size=%.3f\n", w,h, fovy,plane_dist, w/res_x);
		if (Aperture_ratio != 0) printf("\nDepth-Of-Field effect enabled with a lens aperture = %.1f\n", Aperture_ratio);
    }

	void SetEye(Vector from) {
		eye = from;
		// set the camera frame uvn
		n = (eye - at);
		plane_dist = n.length();
		n = n / plane_dist;
		u = up % n;
		u = u / u.length();
		v = n % u;
	}

	Ray PrimaryRay(const Vector& pixel_sample) //  Rays cast from the Eye to a pixel sample which is in Viewport coordinates
	{
		float x_scalar = this->w * (pixel_sample.x / GetResX() - 0.5);
		float y_scalar = this->h * (pixel_sample.y / GetResY() - 0.5);
		float z_scalar = GetPlaneDist();

		Vector ray_dir = Vector(this->u * x_scalar +
								this->v * y_scalar -
								this->n * z_scalar).normalize(); // Direction of the ray is normalised; also -n because df is in the -zs, always.

		return Ray(eye, ray_dir);
	}

	Ray PrimaryRay(const Vector& lens_sample, const Vector& pixel_sample) // DOF: Rays cast from a thin lens sample to a pixel sample
	{
		float x_scalar = this->w * (pixel_sample.x / GetResX() - 0.5);
		float y_scalar = this->h * (pixel_sample.y / GetResY() - 0.5);
		float z_scalar = GetPlaneDist();

		Vector centre_ray = Vector(x_scalar, y_scalar, -z_scalar); // goes from eye to pixel_sample, in camera coords

		Vector actual_p = Vector(centre_ray.x * focal_ratio,
								centre_ray.y * focal_ratio,
								centre_ray.z * focal_ratio); // in camera coords.

		Vector ray_dir = (this->u * (actual_p.x - lens_sample.x) 
						+ this->v * (actual_p.y - lens_sample.y) 
						+ this->n * actual_p.z).normalize(); // in world coordinates

		Vector eye_offset = eye + this-> u * lens_sample.x + this-> v * lens_sample.y; // also in world coordinates

		return Ray(eye_offset, ray_dir);
	}
};

#endif