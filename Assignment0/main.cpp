#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>

int main(){

    Eigen::Vector3f P(2.0f,1.0f,1.0f),res(0.0f,0.0f,0.0f);
    Eigen::Matrix3f Rotation,Translation;
    Rotation << cos(45.0/180*acos(-1)), -sin(45.0/180*acos(-1)), 0 ,
                sin(45.0/180*acos(-1)), cos(45.0/180*acos(-1)), 0, 
                0, 0, 1.0;
    Translation<<1, 0, 1 ,
                0, 1, 2, 
                0, 0, 1.0;
    res=Rotation*P;
    res=Translation*res;
    res=res/res[2];
    std::cout << "Output P\n";
    std::cout << res<< std::endl;
    // matrix add i + j
    // matrix scalar multiply i * 2.0
    // matrix multiply i * j
    // matrix multiply vector i * v

    return 0;
}