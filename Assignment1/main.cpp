#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    float angle=(rotation_angle/180)*MY_PI;
    model<<cos(angle),-sin(angle),0,0,
            sin(angle),cos(angle),0,0,
            0,0,1,0,
            0,0,0,1;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function
    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f perspective = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f ortho_translation = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f ortho_scale = Eigen::Matrix4f::Identity();
    //这里注意n和f要取负数变回空间上的坐标
    float n=-zNear,f=-zFar;
    float theta=(eye_fov/180)*MY_PI;
    float t=abs(zNear)*tan(theta/2),b=-t;
    float r=t*aspect_ratio,l=-r;
    perspective<<n,0,0,0,
                0,n,0,0,
                0,0,n+f,-n*f,
                0,0,1,0;
    //先进行透视变换在进行正交变换，正交变换的矩阵先平移到原点再进行缩放
    ortho_translation<<1,0,0,-(r+l)/2.0,
                       0,1,0,-(t+b)/2.0,
                       0,0,1,-(f+n)/2.0,
                       0,0,0,1;
    ortho_scale<<2.0/(r-l),0,0,0,
                0,2.0/(t-b),0,0,
                0,0,2.0/(n-f),0,
                0,0,0,1;
    Eigen::Matrix4f ortho= Eigen::Matrix4f::Identity();
    ortho = ortho_scale * ortho_translation;         
    projection=ortho*perspective;
    return projection;
}
Eigen::Matrix4f get_rotation_matrix(Vector3f axis, float angle) {//任意轴旋转矩阵（罗德里格斯旋转公式，默认轴过原点）
    double fangle = angle / 180 * MY_PI;
    Eigen::Matrix4f I, N, Rod;
    Eigen::Vector4f axi;
    axi << axis.x(), axis.y(), axis.z(), 0;
    I = Matrix4f::Identity();
    //计算sin后面的项
    N << 0, -axis.z(), axis.y(), 0,
         axis.z(), 0, -axis.x(), 0,
         -axis.y(), axis.x(), 0, 0,
         0, 0, 0, 1;
    //罗德里格旋转公式
    Rod = cos(fangle) * I + (1 - cos(fangle)) * axi * axi.transpose() + sin(fangle) * N;
    Rod(3, 3) = 1;//非齐次坐标的公式应用在齐次坐标上时记得运算完成后把矩阵的右下角改为1
    return Rod;
}
int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";
    Eigen::Vector3f raxis(0,0,1);//罗德里格旋转的旋转轴
    double rangle=0;
    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }
    rst::rasterizer r(700, 700);
    Eigen::Vector3f eye_pos = {0, 0, 5};
    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};
    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};
    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
    int key = 0;
    int frame_count = 0;
    if (command_line) {//开命令行标志
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        //初始化帧缓存和渲染缓存
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));
        r.set_rodrigues(get_rotation_matrix(raxis,rangle));
        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imwrite(filename, image);
        return 0;
    }
    bool rflag = false;
    //std::cout << "Please enter the axis and angle:" << std::endl;
    //std::cin >> raxis.x() >> raxis.y() >> raxis.z() >> ra;//定义罗德里格斯旋转轴和角
    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));
        r.set_rodrigues(get_rotation_matrix(raxis, rangle)*r.get_rodrigues());
        raxis<< 0, 0, 1;
        rangle = 0;
        // if (rflag) //如果按下r了，就开始绕给定任意轴旋转
        // {
        //     std::cout << "Please enter the axis and angle:" << std::endl;
        //     std::cin >> raxis.x() >> raxis.y() >> raxis.z() >> rangle;//定义罗德里格斯旋转轴和角
        //     r.set_rodrigues(get_rotation_matrix(raxis, rangle)*r.get_rodrigues());
        // }
        // else
        // {
        //     r.set_rodrigues(r.get_rodrigues());
        // }
        
        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(1000);//由于r的存在要降低帧率，不然等不到输入

        std::cout << "frame count: " << frame_count++ << '\n';
        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
        else if (key == 'r') {//按下r，再次绕给定任意轴旋转
            std::cout << "Please enter the axis and angle:" << std::endl;
            std::cin >> raxis.x() >> raxis.y() >> raxis.z() >> rangle;//定义罗德里格斯旋转轴和角
        }
    }

    return 0;
}
