#include <iostream>
#include <iostream>
#include <filesystem>
#include "PoissonImage.h"

int main(int argc, char* argv[])
{
    try {
        std::string dir = "C:/users/Emiel/Develop/Thesis/PoissonImageEditing/";

        cv::Mat src;
        cv::Mat dst;
        std::string outputPath;
        cv::Mat mask;

        if (argc < 4) {
            std::cout << "Compulsory arguments are missing ..." << std::endl;
            std::cout << "Usage: PoissonImageEditor <source-path> <destination-path> <output-path> \n"
                << "[ <mask-path> [ <x-offset> <y-offset> [ <-r|-a|-m> [ <-b|-f|-c> <-b|-f|-c> ] ] ] ]"
                << std::endl
                ;
            src = cv::imread(dir + "showcases/case0/src.jpg");
            dst = cv::imread(dir + "showcases/case0/dst.jpg");
            outputPath = dir + "showcases/case0/result.jpg";
            mask = cv::imread(dir + "showcases/case0/mask.jpg", cv::IMREAD_GRAYSCALE);

        }
        else {


            src = cv::imread(argv[1]);
            cv::Mat dst = cv::imread(argv[2]);
            std::string outputPath = argv[3];

            if (argc >= 5) mask = cv::imread(argv[4], cv::IMREAD_GRAYSCALE);
            else mask = 255 * cv::Mat::ones(src.rows, src.cols, CV_8UC1);
        }
        std::cout << "Poisson Image Editing ..." << std::endl;

        cv::Point offset(0, 0);
        if (argc >= 7) {
            offset = cv::Point(std::atoi(argv[5]), std::atoi(argv[6]));
        }

        PoissonImage::GradientScheme scheme = PoissonImage::Maximum;
        if (argc >= 8) {
            std::string a = argv[7];
            if (a == "-r") scheme = PoissonImage::Replace;
            else if (a == "-a") scheme = PoissonImage::Average;
            else if (a == "-m") scheme = PoissonImage::Maximum;
        }

        PoissonImage::DiffOp gradientOp = PoissonImage::Backward, divOp = PoissonImage::Forward;
        if (argc >= 10) {
            std::string a = argv[8], b = argv[9];
            if (a == "-f") gradientOp = PoissonImage::Forward;
            else if (a == "-b") gradientOp = PoissonImage::Backward;
            else if (a == "-c") gradientOp = PoissonImage::Centered;

            if (b == "-f") divOp = PoissonImage::Forward;
            else if (b == "-b") divOp = PoissonImage::Backward;
            else if (b == "-c") divOp = PoissonImage::Centered;
        }

        cv::Mat output;
        PoissonImage::PerfMetric perf;
        if (PoissonImage::seamlessClone(src, dst, mask, offset, output, scheme, gradientOp, divOp, &perf)) {
            cv::imwrite(outputPath, output);

            std::cout << "Initialization: " << perf.m_tInit << "s" << std::endl;
            std::cout << "Calculate Gradient: " << perf.m_tGradient << "s" << std::endl;
            std::cout << "Poisson Solving: " << perf.m_tSolver << "s" << std::endl;
        }
        else {
            std::cerr << "Failed to run Poisson image editing." << std::endl;
        }
    }
    catch (cv::Exception& e)
    {
        std::cerr << e.msg << std::endl; // output exception message
    }

    return 0;
    
}
