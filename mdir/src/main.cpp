#include "app/Application.h"

#include <clocale>
#include <iostream>

int main()
{
    // Enable UTF-8 locale
    std::setlocale(LC_ALL, ""); // NOLINT(*-mt-unsafe)

    try
    {
        mdir::app::Application app;
        return app.run();
    }
    catch (std::exception const & e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
