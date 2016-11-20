// gcc UnitTest.c -o CherenkovInOutTest -lm

#include <stdio.h>
#include <string.h>

#include "DetectorSphere.h"

int number_of_tests;
int number_of_failed_tests;

int is_equal(int line, double a, double b, char* comment) {

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


int main() {
    number_of_tests = 0;
    number_of_failed_tests = 0;
    printf("CherenkovInOut UnitTests: Start\n");

    // Init a DetectorSphere
    {
        struct DetectorSphere sphere;
        DetectorSphere_init(&sphere, 1.0, 2.0, 3.0, 55.0);

        is_equal(__LINE__, sphere.x, 1.0, "init x position of DetectorSphere");
        is_equal(__LINE__, sphere.y, 2.0, "init y position of DetectorSphere");
        is_equal(__LINE__, sphere.z, 3.0, "init z position of DetectorSphere");
        is_equal(__LINE__, sphere.radius, 55.0, "init radius of DetectorSphere");
    }

    printf("\nCherenkovInOut UnitTests: Finished\n");
    return number_of_failed_tests;
}