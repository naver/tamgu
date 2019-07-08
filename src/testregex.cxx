#ifdef REGEXCPP
#include <regex>
using std::regex;
using std::sregex_token_iterator;
#else
#include <boost/regex.hpp>
using boost::regex;
using boost::sregex_token_iterator;
#endif

int main(int argc, char *argv[]) {
    regex rg("test");
    const sregex_token_iterator end;
}
