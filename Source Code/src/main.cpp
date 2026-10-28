#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <filesystem>
#include <fstream>

int main()
{
    std::string lang;
    std::cout << "Hi , and Welcome to DevChecker" << std::endl;
    std::cout << "Version 1.0.0 Pre-Alpha Release" << std::endl;
    while (true)
    {
        bool supported = false;
        std::cout << "Supported Compilers to Check : C++ , C , Python , Rust , Java , Kotlin , C#" << std::endl;
        std::cout << "Type your programming language that you want to check if the compiler works or no :" << std::endl;
        std::cin >> lang;
        for (size_t i = 0; i < lang.length(); i++)
        {
            lang[i] = std::tolower(static_cast<unsigned char>(lang[i]));
        }
        

        
        if (lang == "c++" || lang == "cpp")
        {
            supported = true;

            std::string cppCompiler;
            std::string compilerName;

            if (std::system("where g++ >nul 2>&1") == 0)
            {
                cppCompiler = "g++";
                compilerName = "GCC";
            }
            else if (std::system("where clang++ >nul 2>&1") == 0)
            {
                cppCompiler = "clang++";
                compilerName = "Clang";
            }
            else if (std::system("where cl >nul 2>&1") == 0)
            {
                cppCompiler = "cl";
                compilerName = "MSVC";
            }

            if (cppCompiler.empty())
            {
                std::cout << "No supported C++ compiler found!" << std::endl;
            }
            else
            {
                std::cout << compilerName << " Found!" << std::endl;

                const char* tempEnv = std::getenv("TEMP");

                if (tempEnv == nullptr)
                {
                    std::cout << "TEMP folder not found!" << std::endl;
                }
                else
                {
                    std::filesystem::path tempPath = tempEnv;
                    auto testFile = tempPath / "DevChecker.cpp";
                    auto testExe = tempPath / "DevCheckerTest.exe";

                    std::ofstream file(testFile);

                    if (!file)
                    {
                        std::cout << "Failed to create test file!" << std::endl;
                    }
                    else
                    {
                        file << "#include <iostream>\n";
                        file << "int main() {\n";
                        file << "    std::cout << \"DevChecker works!\" << std::endl;\n";
                        file << "    return 0;\n";
                        file << "}\n";
                        file.close();

                        std::string compileCommand;

                        if (compilerName == "MSVC")
                        {
                            compileCommand =
                                "cl /nologo /EHsc \"" + testFile.string() +
                                "\" /Fe:\"" + testExe.string() + "\" /Fo:\"" +
                                (tempPath / "DevCheckerTest.obj").string() +
                                "\" /link /OUT:\"" + testExe.string() + "\"";
                        }
                        else
                        {
                            compileCommand =
                                cppCompiler + " \"" + testFile.string() +
                                "\" -o \"" + testExe.string() + "\"";
                        }

                        int compileResult = std::system(compileCommand.c_str());

                        if (compileResult != 0)
                        {
                            std::cout << "Compilation Failed!" << std::endl;
                        }
                        else
                        {
                            std::cout << "Compilation Successful!" << std::endl;

                            std::string runCommand =
                                "\"" + testExe.string() + "\"";

                            int runResult = std::system(runCommand.c_str());

                            if (runResult == 0)
                            {
                                std::cout << "C++ Compilation and Run Successful!"
                                        << std::endl;
                            }
                            else
                            {
                                std::cout << "C++ Program Execution Failed!"
                                        << std::endl;
                            }
                        }
                    }
                }
            }
        }
        
        // PYTHON
        if (lang == "python" || lang == "py")
        {
            supported = true;

            std::string python;
            if (std::system("where python >nul 2>&1") == 0)
                python = "python";
            else if (std::system("where py >nul 2>&1") == 0)
                python = "py";
            else if (std::system("where python3 >nul 2>&1") == 0)
                python = "python3";

            if (python.empty())
                std::cout << "Python Not Found!" << std::endl;
            else
            {
                const char* env = std::getenv("TEMP");
                if (!env)
                    std::cout << "TEMP folder not found!" << std::endl;
                else
                {
                    auto path = std::filesystem::path(env) / "DevChecker.py";
                    std::ofstream file(path);
                    file << "print('Python works!')\n";
                    file.close();

                    std::string command =
                        python + " \"" + path.string() + "\"";
                    int result = file ? std::system(command.c_str()) : -1;

                    std::cout << (result == 0
                        ? "Python Test Passed!" : "Python Test Failed!")
                        << std::endl;
                }
            }
        }

        // RUST
        if (lang == "rust" || lang == "rs")
        {
            supported = true;

            if (std::system("where rustc >nul 2>&1") != 0)
                std::cout << "Rust Compiler Not Found!" << std::endl;
            else
            {
                const char* env = std::getenv("TEMP");
                if (!env)
                    std::cout << "TEMP folder not found!" << std::endl;
                else
                {
                    auto dir = std::filesystem::path(env);
                    auto source = dir / "DevChecker.rs";
                    auto exe = dir / "DevCheckerRust.exe";

                    std::ofstream file(source);
                    file << "fn main() { println!(\"Rust works!\"); }\n";
                    file.close();

                    std::string compile =
                        "rustc \"" + source.string() + "\" -o \"" +
                        exe.string() + "\"";

                    int result = file ? std::system(compile.c_str()) : -1;

                    if (result == 0)
                    {
                        std::string run = "\"" + exe.string() + "\"";
                        int runResult = std::system(run.c_str());
                        std::cout << (runResult == 0
                            ? "Rust Test Passed!" : "Rust Run Failed!")
                            << std::endl;
                    }
                    else
                        std::cout << "Rust Compilation Failed!" << std::endl;
                }
            }
        }

        // C
        if (lang == "c")
        {
            supported = true;

            std::string compiler;
            if (std::system("where gcc >nul 2>&1") == 0)
                compiler = "gcc";
            else if (std::system("where clang >nul 2>&1") == 0)
                compiler = "clang";

            if (compiler.empty())
                std::cout << "C Compiler Not Found!" << std::endl;
            else
            {
                const char* env = std::getenv("TEMP");
                if (!env)
                    std::cout << "TEMP folder not found!" << std::endl;
                else
                {
                    auto dir = std::filesystem::path(env);
                    auto source = dir / "DevChecker.c";
                    auto exe = dir / "DevCheckerC.exe";

                    std::ofstream file(source);
                    file << "#include <stdio.h>\n";
                    file << "int main(void) { puts(\"C works!\"); return 0; }\n";
                    file.close();

                    std::string compile =
                        compiler + " \"" + source.string() + "\" -o \"" +
                        exe.string() + "\"";

                    int result = file ? std::system(compile.c_str()) : -1;

                    if (result == 0)
                    {
                        std::string run = "\"" + exe.string() + "\"";
                        int runResult = std::system(run.c_str());
                        std::cout << (runResult == 0
                            ? "C Test Passed!" : "C Run Failed!")
                            << std::endl;
                    }
                    else
                        std::cout << "C Compilation Failed!" << std::endl;
                }
            }
        }

        // JAVA
        if (lang == "java" || lang == "jv")
        {
            supported = true;

            if (std::system("where javac >nul 2>&1") != 0 ||
                std::system("where java >nul 2>&1") != 0)
                std::cout << "Java JDK/Runtime Not Found!" << std::endl;
            else
            {
                const char* env = std::getenv("TEMP");
                if (!env)
                    std::cout << "TEMP folder not found!" << std::endl;
                else
                {
                    auto dir = std::filesystem::path(env);
                    auto source = dir / "DevChecker.java";

                    std::ofstream file(source);
                    file << "public class DevChecker {\n";
                    file << " public static void main(String[] args) {\n";
                    file << "  System.out.println(\"Java works!\");\n";
                    file << " }\n}\n";
                    file.close();

                    std::string compile =
                        "javac \"" + source.string() + "\"";

                    int result = file ? std::system(compile.c_str()) : -1;

                    if (result == 0)
                    {
                        std::string run =
                            "java -cp \"" + dir.string() + "\" DevChecker";
                        int runResult = std::system(run.c_str());
                        std::cout << (runResult == 0
                            ? "Java Test Passed!" : "Java Run Failed!")
                            << std::endl;
                    }
                    else
                        std::cout << "Java Compilation Failed!" << std::endl;
                }
            }
        }

        // KOTLIN
        if (lang == "kotlin" || lang == "kt")
        {
            supported = true;

            if (std::system("where kotlinc >nul 2>&1") != 0 ||
                std::system("where java >nul 2>&1") != 0)
                std::cout << "Kotlin Compiler or Java Not Found!" << std::endl;
            else
            {
                const char* env = std::getenv("TEMP");
                if (!env)
                    std::cout << "TEMP folder not found!" << std::endl;
                else
                {
                    auto dir = std::filesystem::path(env);
                    auto source = dir / "DevChecker.kt";
                    auto jar = dir / "DevCheckerKotlin.jar";

                    std::ofstream file(source);
                    file << "fun main() { println(\"Kotlin works!\") }\n";
                    file.close();

                    std::string compile =
                        "kotlinc \"" + source.string() +
                        "\" -include-runtime -d \"" + jar.string() + "\"";

                    int result = file ? std::system(compile.c_str()) : -1;

                    if (result == 0)
                    {
                        std::string run = "java -jar \"" + jar.string() + "\"";
                        int runResult = std::system(run.c_str());
                        std::cout << (runResult == 0
                            ? "Kotlin Test Passed!" : "Kotlin Run Failed!")
                            << std::endl;
                    }
                    else
                        std::cout << "Kotlin Compilation Failed!" << std::endl;
                }
            }
        }

        // C#
        if (lang == "c#" || lang == "csharp")
        {
            supported = true;

            if (std::system("where dotnet >nul 2>&1") != 0)
                std::cout << ".NET SDK Not Found!" << std::endl;
            else
            {
                const char* env = std::getenv("TEMP");
                if (!env)
                    std::cout << "TEMP folder not found!" << std::endl;
                else
                {
                    auto project =
                        std::filesystem::path(env) / "DevCheckerCSharp";
                    std::filesystem::create_directories(project);

                    auto csproj = project / "DevCheckerCSharp.csproj";
                    auto source = project / "Program.cs";

                    std::ofstream config(csproj);
                    config << "<Project Sdk=\"Microsoft.NET.Sdk\">\n";
                    config << "<PropertyGroup><OutputType>Exe</OutputType>";
                    config << "<TargetFramework>net8.0</TargetFramework>";
                    config << "</PropertyGroup></Project>\n";
                    config.close();

                    std::ofstream file(source);
                    file << "Console.WriteLine(\"CSharp works!\");\n";
                    file.close();

                    std::string command =
                        "dotnet run --project \"" + csproj.string() + "\"";

                    int result = (config && file)
                        ? std::system(command.c_str()) : -1;

                    std::cout << (result == 0
                        ? "CSharp Test Passed!" : "CSharp Test Failed!")
                        << std::endl;
                }
            }
        }
        if (!supported)
        {
            std::cout << "Sorry but You Select To Test Unsupported Compiler" << std::endl;
        }
        
    }
}