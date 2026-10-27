#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

int main()
{
    bool supported = false;
    std::string lang;
    std::cout << "Hi , and Welcome to DevChecker" << std::endl;
    std::cout << "Version 0.10 Alpha Release" << std::endl;
    std::cout << "Supported Languages : C++ , Python , Rust" << std::endl;
    std::cout << "Type your programming language that you want to check if the compiler works or no :" << std::endl;
    std::cin >> lang;
    for (size_t i = 0; i < lang.length(); i++)
    {
        lang[i] = std::tolower(static_cast<unsigned char>(lang[i]));
    }
    

    if (lang == "c++" || lang == "cpp")
    {
        supported = true;
        int result = std::system("where g++");

        if (result == 0)
        {
            std::cout << "GCC Found!" << std::endl;
        }
        else if (std::system("where clang++") == 0)
        {
            std::cout << "Clang Found!" << std::endl;
        }
        else if (std::system("where cl") == 0)
        {
            std::cout << "MSVC Found!" << std::endl;
        }
        else
        {
            std::cout << "No supported C++ compiler found! or your compiler isn't in the path" << std::endl;
        }
    }
    if (lang == "python" || lang == "py")
    {
        supported = true;
        int result = std::system("where python");
        if (result==0)
        {
            std::cout << "Python Found !" << std::endl;
        }
        else
        {
            result = std::system("where py");
            if (result == 0)
            {
                std::cout << "Python Found !" << std::endl;
            }
            else
            {
                result = std::system("where python3");
                if (result == 0)
                {
                    std::cout << "Python Found !" << std::endl;
                }
                else
                {
                    std::cout << "No supported Python compiler found! or your compiler isn't in the path" << std::endl;
                }
                
            }
            
        }
        
    }
    if (lang == "rust" || lang == "rs")
    {
        supported = true;
        int result = std::system("where rustc");

        if (result == 0)
        {
            std::cout << "Rust Compiler Found!" << std::endl;
        }
        else
        {
            std::cout << "Rust Compiler Not Found! It may not be installed or added to PATH." << std::endl;
        }
    }
    if (!supported)
    {
        std::cout << "sorry but the selected language isn't supported" << std::endl;
    }
    
    
}