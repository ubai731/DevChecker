#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <vector>
#include <system_error>

namespace fs = std::filesystem;

// Checks whether a command is available through Windows PATH.
bool commandExists(const std::string& command)
{
    const std::string check = "where " + command + " >nul 2>&1";
    return std::system(check.c_str()) == 0;
}

// Writes a test source file and checks both writing and closing errors.
bool writeFile(const fs::path& path, const std::string& contents)
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
        return false;

    file << contents;
    file.flush();

    const bool writeOkay = static_cast<bool>(file);
    file.close();

    return writeOkay && !file.fail();
}

std::string quote(const fs::path& path)
{
    return "\"" + path.string() + "\"";
}

bool runCommand(const std::string& command)
{
    return std::system(command.c_str()) == 0;
}

fs::path getTempDirectory()
{
    const char* temp = std::getenv("TEMP");
    if (temp && *temp)
        return fs::path(temp);

    const char* tmp = std::getenv("TMP");
    if (tmp && *tmp)
        return fs::path(tmp);

    return {};
}

void removeIfExists(const fs::path& path)
{
    std::error_code ec;
    fs::remove(path, ec); // Cleanup failure should not crash the checker.
}

void reportRun(const std::string& language, const std::string& command)
{
    if (runCommand(command))
        std::cout << language << " test passed!\n";
    else
        std::cout << language << " program execution failed.\n";
}

bool prepareTemp(fs::path& dir)
{
    dir = getTempDirectory();
    if (dir.empty())
    {
        std::cout << "ERROR: TEMP/TMP environment variable was not found.\n";
        return false;
    }

    std::error_code ec;
    if (!fs::exists(dir, ec) || ec)
    {
        std::cout << "ERROR: The temporary directory does not exist.\n";
        return false;
    }
    return true;
}

std::string lowerCase(std::string value)
{
    for (char& ch : value)
        ch = static_cast<char>(
            std::tolower(static_cast<unsigned char>(ch)));
    return value;
}

void checkCpp(const fs::path& dir)
{
    std::string compiler;
    std::string name;

    // Prefer GCC, then Clang. MSVC is checked separately because it needs
    // the Visual Studio Developer Command Prompt environment.
    if (commandExists("g++")) {
        compiler = "g++";
        name = "GCC";
    } else if (commandExists("clang++")) {
        compiler = "clang++";
        name = "Clang";
    } else if (commandExists("cl")) {
        compiler = "cl";
        name = "MSVC";
    }

    if (compiler.empty()) {
        std::cout << "ERROR: No supported C++ compiler was found.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.cpp";
    const fs::path exe = dir / "DevCheckerCppTest.exe";
    const fs::path obj = dir / "DevCheckerCppTest.obj";
    const std::string sourceCode =
        "#include <iostream>\n"
        "int main() { std::cout << \"C++ works!\" << std::endl; return 0; }\n";

    if (!writeFile(source, sourceCode)) {
        std::cout << "ERROR: Failed to write the C++ test file.\n";
        return;
    }

    removeIfExists(exe);
    std::string compile;
    if (name == "MSVC") {
        compile = "cl /nologo /EHsc " + quote(source) +
                  " /Fe:" + quote(exe) +
                  " /Fo:" + quote(obj) +
                  " /link /OUT:" + quote(exe);
    } else {
        compile = compiler + " " + quote(source) + " -o " + quote(exe);
    }

    std::cout << name << " detected. Testing compilation...\n";
    if (!runCommand(compile)) {
        std::cout << "ERROR: C++ compilation failed.\n";
        return;
    }

    std::cout << "C++ compilation successful.\n";
    reportRun("C++", quote(exe));
    removeIfExists(source);
    removeIfExists(exe);
    removeIfExists(obj);
}

void checkC(const fs::path& dir)
{
    std::string compiler;
    if (commandExists("gcc"))
        compiler = "gcc";
    else if (commandExists("clang"))
        compiler = "clang";

    if (compiler.empty()) {
        std::cout << "ERROR: No C compiler (GCC or Clang) was found.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.c";
    const fs::path exe = dir / "DevCheckerCTest.exe";
    if (!writeFile(source,
        "#include <stdio.h>\n"
        "int main(void) { puts(\"C works!\"); return 0; }\n")) {
        std::cout << "ERROR: Failed to write the C test file.\n";
        return;
    }

    removeIfExists(exe);
    if (!runCommand(compiler + " " + quote(source) + " -o " + quote(exe))) {
        std::cout << "ERROR: C compilation failed.\n";
    } else {
        std::cout << "C compilation successful.\n";
        reportRun("C", quote(exe));
    }
    removeIfExists(source);
    removeIfExists(exe);
}

void checkPython(const fs::path& dir)
{
    std::string interpreter;
    if (commandExists("python"))
        interpreter = "python";
    else if (commandExists("py"))
        interpreter = "py";
    else if (commandExists("python3"))
        interpreter = "python3";

    if (interpreter.empty()) {
        std::cout << "ERROR: Python was not found.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.py";
    if (!writeFile(source, "print('Python works!')\n")) {
        std::cout << "ERROR: Failed to write the Python test file.\n";
        return;
    }

    reportRun("Python", interpreter + " " + quote(source));
    removeIfExists(source);
}

void checkRust(const fs::path& dir)
{
    if (!commandExists("rustc")) {
        std::cout << "ERROR: Rust compiler rustc was not found.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.rs";
    const fs::path exe = dir / "DevCheckerRustTest.exe";
    if (!writeFile(source, "fn main() { println!(\"Rust works!\"); }\n")) {
        std::cout << "ERROR: Failed to write the Rust test file.\n";
        return;
    }

    removeIfExists(exe);
    if (!runCommand("rustc " + quote(source) + " -o " + quote(exe))) {
        std::cout << "ERROR: Rust compilation failed.\n";
    } else {
        std::cout << "Rust compilation successful.\n";
        reportRun("Rust", quote(exe));
    }
    removeIfExists(source);
    removeIfExists(exe);
}

void checkJava(const fs::path& dir)
{
    if (!commandExists("javac") || !commandExists("java")) {
        std::cout << "ERROR: Java JDK (javac) and runtime (java) are required.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.java";
    const fs::path classFile = dir / "DevChecker.class";
    if (!writeFile(source,
        "public class DevChecker {\n"
        "  public static void main(String[] args) {\n"
        "    System.out.println(\"Java works!\");\n"
        "  }\n"
        "}\n")) {
        std::cout << "ERROR: Failed to write the Java test file.\n";
        return;
    }

    removeIfExists(classFile);
    if (!runCommand("javac -d " + quote(dir) + " " + quote(source))) {
        std::cout << "ERROR: Java compilation failed.\n";
    } else {
        std::cout << "Java compilation successful.\n";
        reportRun("Java", "java -cp " + quote(dir) + " DevChecker");
    }
    removeIfExists(source);
    removeIfExists(classFile);
}

void checkKotlin(const fs::path& dir)
{
    if (!commandExists("kotlinc") || !commandExists("java")) {
        std::cout << "ERROR: Kotlin compiler (kotlinc) and Java are required.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.kt";
    const fs::path jar = dir / "DevCheckerKotlinTest.jar";
    if (!writeFile(source, "fun main() { println(\"Kotlin works!\") }\n")) {
        std::cout << "ERROR: Failed to write the Kotlin test file.\n";
        return;
    }

    removeIfExists(jar);
    if (!runCommand("kotlinc " + quote(source) +
                    " -include-runtime -d " + quote(jar))) {
        std::cout << "ERROR: Kotlin compilation failed.\n";
    } else {
        std::cout << "Kotlin compilation successful.\n";
        reportRun("Kotlin", "java -jar " + quote(jar));
    }
    removeIfExists(source);
    removeIfExists(jar);
}

void checkCSharp(const fs::path& dir)
{
    if (!commandExists("dotnet")) {
        std::cout << "ERROR: .NET SDK (dotnet) was not found.\n";
        return;
    }

    const fs::path project = dir / "DevCheckerCSharp";
    std::error_code ec;
    fs::create_directories(project, ec);
    if (ec) {
        std::cout << "ERROR: Could not create the temporary C# project folder.\n";
        return;
    }

    const fs::path csproj = project / "DevCheckerCSharp.csproj";
    const fs::path source = project / "Program.cs";
    const std::string projectXml =
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <OutputType>Exe</OutputType>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "  </PropertyGroup>\n"
        "</Project>\n";

    if (!writeFile(csproj, projectXml) ||
        !writeFile(source, "Console.WriteLine(\"C# works!\");\n")) {
        std::cout << "ERROR: Failed to write the C# test project.\n";
        return;
    }

    // dotnet run compiles and then executes the test project.
    reportRun("C#", "dotnet run --project " + quote(csproj) + " --nologo");
    fs::remove_all(project, ec);
}

void checkGo(const fs::path& dir)
{
    if (!commandExists("go")) {
        std::cout << "ERROR: Go toolchain was not found.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.go";
    const fs::path exe = dir / "DevCheckerGoTest.exe";
    if (!writeFile(source,
        "package main\n"
        "import \"fmt\"\n"
        "func main() { fmt.Println(\"Go works!\") }\n")) {
        std::cout << "ERROR: Failed to write the Go test file.\n";
        return;
    }

    removeIfExists(exe);
    if (!runCommand("go build -o " + quote(exe) + " " + quote(source))) {
        std::cout << "ERROR: Go compilation failed.\n";
    } else {
        std::cout << "Go compilation successful.\n";
        reportRun("Go", quote(exe));
    }
    removeIfExists(source);
    removeIfExists(exe);
}

void checkJavaScript(const fs::path& dir)
{
    if (!commandExists("node")) {
        std::cout << "ERROR: Node.js was not found.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.js";
    if (!writeFile(source, "console.log('JavaScript works!');\n")) {
        std::cout << "ERROR: Failed to write the JavaScript test file.\n";
        return;
    }

    reportRun("JavaScript", "node " + quote(source));
    removeIfExists(source);
}

void checkPHP(const fs::path& dir)
{
    if (!commandExists("php")) {
        std::cout << "ERROR: PHP was not found.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.php";
    if (!writeFile(source, "<?php echo \"PHP works!\\n\"; ?>\n")) {
        std::cout << "ERROR: Failed to write the PHP test file.\n";
        return;
    }

    reportRun("PHP", "php " + quote(source));
    removeIfExists(source);
}

void checkTypeScript(const fs::path& dir)
{
    if (!commandExists("tsc") || !commandExists("node")) {
        std::cout << "ERROR: TypeScript compiler (tsc) and Node.js are required.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.ts";
    const fs::path output = dir / "DevChecker.js";
    if (!writeFile(source, "console.log('TypeScript works!');\n")) {
        std::cout << "ERROR: Failed to write the TypeScript test file.\n";
        return;
    }

    removeIfExists(output);
    if (!runCommand("tsc --target ES2020 --module none --skipLibCheck --outDir " +
                    quote(dir) + " " + quote(source))) {
        std::cout << "ERROR: TypeScript compilation failed.\n";
    } else {
        std::cout << "TypeScript compilation successful.\n";
        reportRun("TypeScript", "node " + quote(output));
    }
    removeIfExists(source);
    removeIfExists(output);
}

void checkSwift(const fs::path& dir)
{
    if (!commandExists("swiftc")) {
        std::cout << "ERROR: Swift compiler (swiftc) was not found.\n";
        return;
    }

    const fs::path source = dir / "DevChecker.swift";
    const fs::path exe = dir / "DevCheckerSwiftTest.exe";
    if (!writeFile(source, "print(\"Swift works!\")\n")) {
        std::cout << "ERROR: Failed to write the Swift test file.\n";
        return;
    }

    removeIfExists(exe);
    if (!runCommand("swiftc " + quote(source) + " -o " + quote(exe))) {
        std::cout << "ERROR: Swift compilation failed.\n";
    } else {
        std::cout << "Swift compilation successful.\n";
        reportRun("Swift", quote(exe));
    }
    removeIfExists(source);
    removeIfExists(exe);
}

int main()
{
    std::cout << "Hi, and welcome to DevChecker!\n";
    std::cout << "Version 1.5.0 Stable Release Release\n\n";

    while (true)
    {
        std::cout << "\nSupported languages: C++, C, Python, Rust, Java, "
                     "Kotlin, C#, Go, JavaScript, PHP, TypeScript, Swift\n";
        std::cout << "Enter a language to test (or type exit to quit): ";

        std::string lang;
        if (!(std::cin >> lang)) {
            std::cout << "\nInput ended. Goodbye!\n";
            break;
        }

        lang = lowerCase(lang);
        if (lang == "exit" || lang == "quit" || lang == "q" || lang == "x")
        {
            std::cout << "Goodbye!\n";
            break;
        }

        fs::path tempDir;
        if (!prepareTemp(tempDir))
            continue;

        if (lang == "c++" || lang == "cpp" || lang == "cplusplus")
            checkCpp(tempDir);
        else if (lang == "c")
            checkC(tempDir);
        else if (lang == "python" || lang == "py" || lang == "python3")
            checkPython(tempDir);
        else if (lang == "rust" || lang == "rs")
            checkRust(tempDir);
        else if (lang == "java" || lang == "jv")
            checkJava(tempDir);
        else if (lang == "kotlin" || lang == "kt")
            checkKotlin(tempDir);
        else if (lang == "c#" || lang == "csharp" || lang == "cs")
            checkCSharp(tempDir);
        else if (lang == "go" || lang == "golang")
            checkGo(tempDir);
        else if (lang == "javascript" || lang == "js" || lang == "node")
            checkJavaScript(tempDir);
        else if (lang == "php")
            checkPHP(tempDir);
        else if (lang == "typescript" || lang == "ts")
            checkTypeScript(tempDir);
        else if (lang == "swift")
            checkSwift(tempDir);
        else
            std::cout << "ERROR: Unsupported language. Check the supported list above.\n";
    }

    return 0;
}
