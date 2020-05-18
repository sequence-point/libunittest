#include <libunittest/unittest.hxx>

#include <cstdlib>
#include <string>

int
main(int argc, char* argv[])
{
  char* verbosity_str = ::getenv("TEST_VERBOSITY");
  int verbosity{ 0 };

  if (verbosity_str) {
    verbosity = std::stoi(verbosity_str);
  }

  return TEST::run_all(verbosity);
}
