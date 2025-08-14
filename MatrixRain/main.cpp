#include "matrixrain.h"
#include <boost/program_options.hpp>

using namespace boost::program_options;

void on_age(int age)
{
    std::cout << "On age: " << age << '\n';
}

int main(int argc, const char *argv[]) {
    try
    {
        options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("host", value<std::string>()->default_value("127.0.0.1"),
                        "Set hostname for a matrixserver")
                ("fade", value<float>()->default_value(0.9),
                 "Set custom fade factor");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help"))
            std::cout << desc << '\n';
        else if (vm.count("age"))
            std::cout << "host: " << vm["host"].as<std::string>() << '\n';

        PixelFlow App1(vm["host"].as<std::string>(), vm["fade"].as<float>());
        App1.start();
        while(1) sleep(2);
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    return 0;
}
