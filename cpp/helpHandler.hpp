/* MIT License
 *
 * Copyright (c) 2022 TechnicFully
 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef HELP_HANDLER_HPP
#define HELP_HANDLER_HPP

#include <regex>
#include <limits>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

#define HELP_HANDLER_VERSION_RELEASE 0
#define HELP_HANDLER_VERSION_MAJOR 0.1
#define HELP_HANDLER_VERSION_MINOR 0
#define HELP_HANDLER_VERSION_STR "0.1.0"

//C++'s std::endl doesn't change its newline feed to the appropriate host OS's, so we use our own
#ifdef _WIN32
#define NEWLINE \
    "\r\n" \
    << \
    std::flush;
#else
#define NEWLINE \
    "\n" \
    << \
    std::flush;
#endif

int DISABLE_NO_ARGS_HELP     = 0x00000010;
int DISABLE_EXTRA_STRINGS    = 0x00000001;
int DISABLE_MATCH_HYPHENS    = 0x00000100;
int ENABLE_UNKNOWN_ARGS_HELP = 0x00001000;
int ENABLE_HYPHENS_ONLY      = 0x00002000; //Value being higher than match_hyphens is intentional, to override in case both are set



namespace helpHandler {
    enum class versionT {
        stringT,
        integerT,
        doubleT,
    };

    static struct most_recent_t {
        unsigned int name = 0;
        versionT ver = versionT::stringT;
    } most_recent_t;

    static struct info_t {
        std::string name        = "";
        std::string versionStr  = "No version is available";
        unsigned int versionInt = 0;
        double versionDouble    = 0;
    } info_t;

    static struct options_t {
        bool noArgHelp      = true;
        bool extraStrings   = true;
        bool matchHyphens   = true;
        bool hyphensOnly    = false;
        bool unknownArgHelp = false;
    } options_t;


    /*****************/
    /**** PRIVATE ****/
    /*****************/
    static std::string trim(const std::string& s) {
        size_t first = s.find_first_not_of(' ');
        if (std::string::npos == first) {
            return s; }
            
        size_t last = s.find_last_not_of(' ');
        return s.substr(first, (last - first + 1));
    }


    /****************/
    /**** PUBLIC ****/
    /****************/
    int handle(int argc, char** argv, std::string help) {
        if (help.empty()) {
            help = "No usage help is available"; }
        if (argc == 1 && options_t.noArgHelp == true) {
            std::cout << help << NEWLINE;
            return EXIT_SUCCESS; }


        /****************/
        /* Error checks */
        /****************/
        if (!argv) {
            throw std::invalid_argument("Argument value (argv) is NULL"); }

        if (argc > std::numeric_limits<int>::max()) {
            throw std::invalid_argument("Argument count (argc) is larger than the limit of int type");
        } else if (argc < 1) {
            if (argc < std::numeric_limits<int>::min()) {
                throw std::invalid_argument("Argument count (argc) is smaller than the limit of int type");
            } else {
                throw std::invalid_argument("Argument count (argc) is 0 or less (should always be at least 1)..."); }
        }


        /*******/
        /* Run */
        /*******/
        unsigned matches = 0;
        bool matchedHelp = false;
        bool matchedVer  = false;

        //Construct regex
        std::string helpExpression = "";
        std::string versionExpression = "";
        std::string hyphensExpression = "";
    

        if (options_t.hyphensOnly == true) {
            std::cout << "Hyphens only";
            hyphensExpression = "-{1,}";
        } else if (options_t.matchHyphens == true) {
            std::cout << "All hyphens";
            hyphensExpression = "-{0,}";
        }


        if (options_t.matchHyphens == true || options_t.hyphensOnly == true) {
                helpExpression      += hyphensExpression;
                versionExpression   += hyphensExpression;
        }


        helpExpression      += "h{1,}e{1,}l{1,}p{1,}(.*)";
        versionExpression   += "v{1,}e{1,}r{1,}s{1,}i{1,}o{1,}n{1,}(.*)";


        if (options_t.extraStrings == true) { 
            helpExpression      += "|";
            versionExpression   += "|";

            if (options_t.matchHyphens == true || options_t.hyphensOnly == true) {
                helpExpression      += hyphensExpression;
                versionExpression   += hyphensExpression;
            }

            helpExpression      += "h{1,}$"; 
            versionExpression   += "v{1,}$";
        }

        //Match for arguments
        std::vector<std::string> arguments(argv+1, argv+argc); //Start from argv+1 to skip binary name
        for (auto arg: arguments) {
                std::regex helpRegex(helpExpression, std::regex_constants::icase);
                if (std::regex_match(arg, helpRegex) == true) {
                    matchedHelp = true;
                    matches++; }

                std::regex versionRegex(versionExpression, std::regex_constants::icase);
                if (std::regex_match(arg, versionRegex) == true) {
                    matchedVer = true;
                    matches++; }
        }

        //Output appropriate results
        if (matches > 0) {
            if (matchedVer == true) {
                switch (most_recent_t.ver) {
                    case versionT::stringT: std::cout << trim(info_t.versionStr); break;
                    case versionT::integerT: std::cout << info_t.versionInt; break; 
                    case versionT::doubleT: std::cout << info_t.versionDouble; break;
                }

            }

            if (matchedHelp == true) {
                if (matchedVer == true) { std::cout << NEWLINE; }
                if (info_t.name.empty() == false) { 
                    std::cout << trim(info_t.name) << " "; }
                std::cout << help;
            }

            std::cout << NEWLINE;
            return matches;
        }

        //End
        if (options_t.unknownArgHelp == true && argc > 1) {
            argc > 2 ? std::cout << "Unknown arguments given" : std::cout << "Unknown argument given";
            std::cout << NEWLINE;
        }

        return 0;
    }


    int handleFile(int argc, char** argv, const std::string& fileName) {
        std::ifstream f;
        std::string s;

        f.open(fileName, std::ios::in);
        if (!f.is_open()) {
            throw std::ios_base::failure("Could not open file"); }
        if (f.peek() == std::ifstream::traits_type::eof()) {
            throw std::runtime_error("Given help file is empty"); }

        std::string line;
        while(getline(f, line)) {
            s += line; }

        f.close();

        return helpHandler::handle(argc, argv, s);
    } 


    void version(double version) noexcept {
        info_t.versionDouble = version;
        most_recent_t.ver = versionT::doubleT;
    } void version(unsigned int version) noexcept {
        info_t.versionInt = version;
        most_recent_t.ver = versionT::integerT;
    } void version(int version) noexcept {
        info_t.versionInt = version;
        most_recent_t.ver = versionT::integerT;
    } void version(std::string version) { //Parent
        if (version.empty()) {
            throw std::invalid_argument("Version string was given, but is empty"); }
        
        info_t.versionStr = trim(version);
        most_recent_t.ver = versionT::stringT;
    }


    int handle(int argc, char** argv, std::string helpDialogue, std::string version) {
        helpHandler::version(version);
        return helpHandler::handle(argc, argv, helpDialogue);
    }
        helpHandler::version(version);
        return helpHandler::handle(argc, argv, helpDialogue);
    }


    int handleFile(int argc, char** argv, const std::string& fileName, std::string version) {
        helpHandler::version(version);
        return helpHandler::handleFile(argc, argv, fileName);
    } int handleFile(int argc, char** argv, const std::string& fileName, double version) {
        helpHandler::version(version);
        return helpHandler::handleFile(argc, argv, fileName);
    } int handleFile(int argc, char** argv, const std::string& fileName, unsigned int version) {
        helpHandler::version(version);
        return helpHandler::handleFile(argc, argv, fileName);
    } int handleFile(int argc, char** argv, const std::string& fileName, int version) {
        helpHandler::version(version);
        return helpHandler::handleFile(argc, argv, fileName);
    }


    void name(const std::string& appName) { //Parent
        if (appName.empty()) {
            throw std::invalid_argument("App name was given, but is empty"); }

        info_t.name = trim(appName);
    }


    void info(const std::string& appName, std::string version) {
        helpHandler::name(appName);
        helpHandler::version(version);
    } void info(const std::string& appName, double version) {
        helpHandler::name(appName);
        helpHandler::version(version);
    } void info(const std::string& appName, unsigned int version) {
        helpHandler::name(appName);
        helpHandler::version(version);
    } void info(const std::string& appName, int version) {
        helpHandler::name(appName);
        helpHandler::version(version);
    }


    int config(int option_flags) {
        if (option_flags & DISABLE_NO_ARGS_HELP)        { options_t.noArgHelp       = false; }
        if (option_flags & DISABLE_EXTRA_STRINGS)       { options_t.extraStrings    = false; }
        if (option_flags & DISABLE_MATCH_HYPHENS)       { options_t.matchHyphens    = false; }
        if (option_flags & ENABLE_HYPHENS_ONLY)         { options_t.hyphensOnly     = true; }
        if (option_flags & ENABLE_UNKNOWN_ARGS_HELP)    { options_t.unknownArgHelp  = true;  }

        if (!(option_flags & DISABLE_NO_ARGS_HELP) &&
            !(option_flags & DISABLE_EXTRA_STRINGS) &&
            !(option_flags & DISABLE_MATCH_HYPHENS) &&
            !(option_flags & ENABLE_UNKNOWN_ARGS_HELP) && 
            !(option_flags & ENABLE_HYPHENS_ONLY)) {
                throw std::invalid_argument("invalid config flag passed");
        }

        return EXIT_SUCCESS;
    }
}

#undef NEWLINE
#endif /* HELP_HANDLER_HPP */
