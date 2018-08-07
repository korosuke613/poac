#ifndef POAC_UTIL_COMMAND_HPP
#define POAC_UTIL_COMMAND_HPP

#include <string>
#include <iostream>
#include <array>

#include <stdio.h> // popen, pclose

#include <boost/optional.hpp>


namespace poac::util {
    class command {
    public:
        std::string data() const { return cmd; };

        command(const std::string& c) {
            cmd = c;
        }

        command env(const std::string& name, const std::string& val) {
            return cmd.insert(0, name + "=" + val + " ");
        }
        command std_err() {
            return cmd + " 2>&1";
        }

        boost::optional<std::string> run() {
            std::array<char, 128> buffer;
            std::string result;

            if (FILE* pipe = popen(cmd.c_str(), "r")) {
                while (std::fgets(buffer.data(), 128, pipe) != nullptr)
                    result += buffer.data();
                if (pclose(pipe) != 0)
                    return boost::none;
            }
            else {
                return boost::none;
            }
            return result;
        }

        friend std::ostream& operator<<(std::ostream& stream, const command& c) {
            stream << c.cmd;
            return stream;
        }

        bool operator==(const command& rhs) const {
            return this->cmd == rhs.cmd;
        }
        bool operator==(const std::string& rhs) const {
            return this->cmd == rhs;
        }

        command operator&&(const command& rhs) const {
            return command(this->cmd + " && " + rhs.cmd);
        }
        command operator&&(const std::string& rhs) const {
            return command(this->cmd + " && " + rhs);
        }

        command operator&=(const command& rhs) {
            return this->cmd += (" && " + rhs.cmd);
        }
        command operator&=(const std::string& rhs) {
            return this->cmd += (" && " + rhs);
        }

        command operator||(const command& rhs) const {
            return command(this->cmd + " || " + rhs.cmd);
        }
        command operator||(const std::string& rhs) const {
            return command(this->cmd + " || " + rhs);
        }

        command operator|=(const command& rhs) {
            return this->cmd += (" || " + rhs.cmd);
        }
        command operator|=(const std::string& rhs) {
            return this->cmd += (" || " + rhs);
        }

    private:
        std::string cmd;
    };
} // end namespace
#endif // !POAC_UTIL_COMMAND_HPP