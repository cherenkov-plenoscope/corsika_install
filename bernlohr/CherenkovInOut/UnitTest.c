// gcc UnitTest.c -o CherenkovInOutTest -lm

#include <stdio.h>
#include <string.h>

#include "DetectorSphere.h"

int number_of_tests;
int number_of_failed_tests;

int expect_equal(int line, double a, double b, char* comment) {
    number_of_tests = number_of_tests + 1;
    printf(".");
    if(a == b) {
        return 0;
    }else{

        number_of_failed_tests = number_of_failed_tests + 1;
        printf("F");

        char info[1024] = "\nError in line ";
        char line_str[1024];
        sprintf(line_str, "%d", line);
        strcat(info, line_str);
        strcat(info, ": ");
        strcat(info, comment);
        strcat(info, "\n");
        fputs(info, stdout);
        return 1;
    }
}


int expect_true(int line, int what, char* comment) {
    number_of_tests = number_of_tests + 1;
    printf(".");
    if(what) {
        return 0;
    }else{
        number_of_failed_tests = number_of_failed_tests + 1;
        printf("F");
        char info[1024] = "\nError in line ";
        char line_str[1024];
        sprintf(line_str, "%d", line);
        strcat(info, line_str);
        strcat(info, ": ");
        strcat(info, comment);
        strcat(info, "\n");
        fputs(info, stdout);
        return 1;
    }   
}


int main() {
    number_of_tests = 0;
    number_of_failed_tests = 0;
    printf("CherenkovInOut UnitTests: Start\n");

    // Init a DetectorSphere
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 1.0, 2.0, 3.0, 55.0);

        expect_equal(__LINE__, sphere.x, 1.0, "init x position of DetectorSphere");
        expect_equal(__LINE__, sphere.y, 2.0, "init y position of DetectorSphere");
        expect_equal(__LINE__, sphere.z, 3.0, "init z position of DetectorSphere");
        expect_equal(__LINE__, sphere.radius, 55.0, "init radius of DetectorSphere");
    }

    // Frontal hit on DetectorSphere
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 0.0, 0.0, 0.0, 1.0);

        struct PhotonBunch bunch;
        bunch.x = 0.0;
        bunch.y = 0.0;
        bunch.cx = 0.0;
        bunch.cy = 0.0;

        int hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, frontal hit");
    }

    // Frontal but too far away on DetectorSphere
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 0.0, 0.0, 0.0, 1.0);

        struct PhotonBunch bunch;
        bunch.x = 1.1;
        bunch.y = 0.0;
        bunch.cx = 0.0;
        bunch.cy = 0.0;

        int hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, frontal but too far away");
    }

    // zero radius sphere frontal
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 0.0, 0.0, 0.0, 0.0);

        struct PhotonBunch bunch;
        bunch.x = 0.0;
        bunch.y = 0.0;
        bunch.cx = 0.0;
        bunch.cy = 0.0;

        int hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, zero radius, but exact hit");
    }

    // zero radius with offset
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 0.0, 0.0, 0.0, 0.0);

        struct PhotonBunch bunch;
        bunch.x = 1e-6;
        bunch.y = 0.0;
        bunch.cx = 0.0;
        bunch.cy = 0.0;

        int hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, zero radius and too far away");
    }


    // frontal, close to edge
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 0.0, 0.0, 0.0, 1.0);

        struct PhotonBunch bunch;
        bunch.x = 0.0;
        bunch.y = 0.0;
        bunch.cx = 0.0;
        bunch.cy = 0.0;
        int hit;

        bunch.x = 1.01;
        bunch.y = 0.0;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, x, slightly off");

        bunch.x = 0.99;
        bunch.y = 0.0;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, x, slightly on");

        bunch.x = 0.0;
        bunch.y = 1.01;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, y, slightly off");

        bunch.x = 0.0;
        bunch.y = 0.99;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, y, slightly on");

        bunch.x =-1.01;
        bunch.y = 0.0;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, -x, slightly off");

        bunch.x =-0.99;
        bunch.y = 0.0;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, -x, slightly on");

        bunch.x = 0.0;
        bunch.y =-1.01;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, -y, slightly off");

        bunch.x = 0.0;
        bunch.y =-0.99;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, -y, slightly on");
    }

    // inclined photon bunch 45 deg
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 0.0, 0.0, 1.0, sqrt(0.5)+0.01);

        struct PhotonBunch bunch;
        bunch.x = 0.0;
        bunch.y = 0.0;
        bunch.cx = 0.0;
        bunch.cy = 0.0;
        int hit;

        bunch.cx = 0.70710678118654757;
        bunch.cy = 0.0;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, cx 45 deg");

        bunch.cx = 0.0;
        bunch.cy = 0.70710678118654757;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, cy 45 deg");

        bunch.cx =-0.70710678118654757;
        bunch.cy = 0.0;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, cx -45 deg");

        bunch.cx = 0.0;
        bunch.cy =-0.70710678118654757;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, hit, "DetectorSphere, cy -45 deg");

        sphere.radius = sqrt(0.5)-0.01;

        bunch.cx = 0.70710678118654757;
        bunch.cy = 0.0;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, cx 45 deg, but too far away");

        bunch.cx = 0.0;
        bunch.cy = 0.70710678118654757;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, cy 45 deg, but too far away");

        bunch.cx =-0.70710678118654757;
        bunch.cy = 0.0;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, cx -45 deg, but too far away");

        bunch.cx = 0.0;
        bunch.cy =-0.70710678118654757;
        hit = DetectorSphere_is_hit_by_photon(&sphere, &bunch);
        expect_true(__LINE__, !hit, "DetectorSphere, cy -45 deg, but too far away");
    }

    // transform_to_detector_frame
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 0.0, 0.0, 0.0, 0.0);

        struct PhotonBunch bunch;
        bunch.x = 1.0;
        bunch.y = 2.0;
        bunch.cx = 0.1;
        bunch.cy = 0.2;

        DetectorSphere_transform_to_detector_frame(&sphere, &bunch);
        expect_equal(__LINE__, bunch.x, 1.0, "DetectorSphere_transform, expect no offset in x");
        expect_equal(__LINE__, bunch.y, 2.0, "DetectorSphere_transform, expect no offset in y");
        expect_equal(__LINE__, bunch.cx, 0.1, "DetectorSphere_transform, expect no offset in cx");
        expect_equal(__LINE__, bunch.cy, 0.2, "DetectorSphere_transform, expect no offset in cy");
    }





    printf("\nCherenkovInOut UnitTests: Finished\n");
    return number_of_failed_tests;
}