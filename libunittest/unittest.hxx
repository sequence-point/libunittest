
/*
 * Instructions
 * ============
 *
 * Tests are defined using the DEFINE_TEST macro.
 *
 * Common setup code can be specified using the ON_TEST_SETUP macro and
 * common teardown code can be specified using the ON_TEST_TEARDOWN
 * macro.
 *
 * Within tests a set of assertions are available. These are:
 *
 *   - TEST_SKIP
 *   - TEST_NOT_IMPLEMENTED
 *   - TEST_TRUE(Expr)
 *   - TEST_FALSE(Expr)
 *   - TEST_EQUAL(LeftExpr, RightExpr)
 *   - TEST_DIFFERENT(LeftExpr, RightExpr)
 *   - TEST_THROWS(Type, Block)
 *   - TEST_THROWS_ANY(Block)
 *   - TEST_DOES_NOT_THROW(Block)
 *
 * ## TEST_SKIP
 *
 * Skips a test. This does not mark the test as failed.
 *
 * ## TEST_NOT_IMPLEMENTED
 *
 * Skips a test. This marks the test as failed.
 *
 * ## TEST_TRUE(Expr)
 *
 * Fails if Expr is not true.
 *
 * ## TEST_FALSE(Expr)
 *
 * Fails if Expr is not false.
 *
 * ## TEST_EQUAL(LeftExpr, RightExpr)
 *
 * Fails if LeftExpr is not equal to RightExpr.
 *
 * ## TEST_DIFFERENT(LeftExpr, RightExpr)
 *
 * Fails if LeftExpr is equal to RightExpr.
 *
 * ## TEST_THROWS(Type, Block)
 *
 * Fails if Block does not throw an exception of type Type.
 *
 * ## TEST_THROWS_ANY(Block)
 *
 * Fails if Block does not throw an exception.
 *
 * ## TEST_DOES_NOT_THROW(Block)
 *
 * Fails if Block does throw an exception.
 *
 * ## Example
 *
 * Below is an example test suite.
 *
 * ```
 * #include <testsuite/testsuite.H>
 *
 * int main() { return TEST::run_all(); }
 *
 * struct divide_by_zero { };
 *
 * int divide(int a, int b)
 * {
 *   if (b == 0)
 *     throw divide_by_zero{ };
 *
 *   return a / b;
 * }
 *
 * ON_TEST_SETUP
 * {
 * }
 *
 * ON_TEST_TEARDOWN
 * {
 * }
 *
 * DEFINE_TEST("Divide by zero")
 * {
 *   TEST_THROWS(divide_by_zero, {
 *     divide(10, 0);
 *   });
 * }
 *
 * DEFINE_TEST("100 / 4 == 25")
 * {
 *   TEST_DOES_NOT_THROW({
 *     TEST_EQUAL(divide(100, 4), 25);
 *   });
 * }
 * ```
 *
 */

#ifndef libunittest_hxx_
#define libunittest_hxx_

#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

#include <libunittest/macros.hxx>
#include <libunittest/utility.hxx>

class TEST_SETUP : private utility::Intrusive_instance_list< TEST_SETUP > {
  friend class utility::Intrusive_instance_list< TEST_SETUP >;

public:
  template< typename Function >
  explicit TEST_SETUP(Function function) : function_{ function }
  {}

  static void
  run_all()
  {
std::cout << "running tests\n";
    for (auto j = first(); j; j = j->next())
      j->run();
  }

private:
  void
  run()
  {
    function_();
  }

private:
  std::function< void() > function_;
};

class TEST_TEARDOWN
  : private utility::Intrusive_instance_list< TEST_TEARDOWN > {
  friend class utility::Intrusive_instance_list< TEST_TEARDOWN >;

public:
  template< typename Function >
  explicit TEST_TEARDOWN(Function function) : function_{ function }
  {}

  static void
  run_all()
  {
    for (auto j = first(); j; j = j->next())
      j->run();
  }

private:
  void
  run()
  {
    function_();
  }

private:
  std::function< void() > function_;
};

//! The TEST class represents a test.
class TEST : private utility::Intrusive_instance_list< TEST > {
  friend class utility::Intrusive_instance_list< TEST >;

  //! Exception class indicating a skipped test.
  class test_skipped : public std::exception {
  public:
    char const*
    what() const noexcept override
    {
      return "Test skipped";
    }
  };

  //! Exception class indicating an unimplemented test.
  class test_not_implemented : public std::exception {
  public:
    char const*
    what() const noexcept override
    {
      return "Test not implemented";
    }
  };

  //! Exception class indicating a test failure.
  class test_failed : public std::exception {
  public:
    //! Get a short description of the error.
    char const*
    what() const noexcept override
    {
      return "Test failed";
    }

    //! Output diagnostic information.
    virtual void
    diagnose(std::ostream& out) const = 0;
  };

  //! Exception class indicating a test assertion failure.
  class test_assertion_failed : public test_failed {
  public:
    //! Construct a new test_exception object.
    test_assertion_failed(std::string file, std::size_t line)
      : file_{ std::move(file) }, line_{ line }
    {}

    //! Get the source file.
    std::string const&
    file() const noexcept
    {
      return file_;
    }

    //! Get the source file line.
    std::size_t
    line() const noexcept
    {
      return line_;
    }

    //! Output diagnostic information.
    void
    diagnose(std::ostream& out) const override
    {
      out << "    Assertion failed!\n"
          << "    File: " << file() << "\n"
          << "    Line: " << line() << "\n";
    }

  private:
    std::string const file_;
    std::size_t const line_;
  };

public:
  //! Construct a new test.
  template< typename Function >
  TEST(std::string const& name, Function function)
    : name_{ name }, function_{ function }
  {}

  //! Get the name of this test.
  std::string const&
  name() noexcept
  {
    return name_;
  }

  //! Skip a test.
  void
  skip_test()
  {
    throw test_skipped{};
  }

  void
  not_implemented()
  {
    throw test_not_implemented{};
  }

  //! Expect a true value.
  void
  test_true(bool value, std::string file, std::size_t line)
  {
    if (value != true)
      throw test_assertion_failed{ std::move(file), line };
  }

  //! Expect a false value.
  void
  test_false(bool value, std::string file, std::size_t line)
  {
    if (value != false)
      throw test_assertion_failed{ std::move(file), line };
  }

  //! Test two values for equality.
  template< typename T >
  void
  test_equal(T const& left, T const& right, std::string file, std::size_t line)
  {
    if (!(left == right))
      throw test_assertion_failed{ std::move(file), line };
  }

  //! Test two values for equality.
  template< typename T1, typename T2 >
  void
  test_equal(T1 const& left,
             T2 const& right,
             std::string file,
             std::size_t line)
  {
    if (!(left == right))
      throw test_assertion_failed{ std::move(file), line };
  }

  //! Test two values for difference.
  template< typename T >
  void
  test_different(T const& left,
                 T const& right,
                 std::string file,
                 std::size_t line)
  {
    if (!(left != right))
      throw test_assertion_failed{ std::move(file), line };
  }

  //! Test two value for difference.
  template< typename T1, typename T2 >
  void
  test_different(T1 const& left,
                 T2 const& right,
                 std::string file,
                 std::size_t line)
  {
    if (!(left != right))
      throw test_assertion_failed{ std::move(file), line };
  }

  //! Expect a specific exception.
  template< typename Exception, typename Function >
  void
  test_throws(Function function, std::string file, std::size_t line)
  {
    try {
      function();
    }
    catch (Exception const&) {
      return;
    }

    throw test_assertion_failed{ std::move(file), line };
  }

  //! Expect any exception.
  template< typename Function >
  void
  test_throws_any(Function function, std::string file, std::size_t line)
  {
    try {
      function();
    }
    catch (...) {
      return;
    }

    throw test_assertion_failed{ std::move(file), line };
  }

  //! Expect code not to throw an exception.
  template< typename Function >
  void
  test_does_not_throw(Function function, std::string file, std::size_t line)
  {
    try {
      function();
    }
    catch (...) {
      throw test_assertion_failed{ std::move(file), line };
    }
  }

  template< typename Function >
  void
  ensure_reached(std::string point,
                 Function function,
                 std::string file,
                 std::size_t line)
  {
    try {
      function();
    }
    catch (utility::Point_exception const& ex) {
      if (ex.id == point)
        return;
    }

    throw test_assertion_failed{ std::move(file), line };
  }

  void
  point_reached(std::string point)
  {
    throw utility::Point_exception{ std::move(point) };
  }

  //! Run all tests.
  static int
  run_all(int verbosity = 0)
  {
    auto test = first();

    if (!test) {
      std::cout << "no tests\n";
      return 1;
    }

    std::stringstream log_output;
    std::size_t counter{};
    std::size_t pass_count{};
    std::size_t skip_count{};
    std::size_t fail_count{};

    while (test) {
      auto result = test->run(log_output, ++counter, verbosity);

      switch (result) {
        case Test_result::pass: ++pass_count; break;
        case Test_result::skip: ++skip_count; break;
        case Test_result::fail: ++fail_count; break;
      }

      test = test->next();
    }

    if (verbosity > 2)
      log_output << '\n'
                 << pass_count << " tests passed out of " << counter << "; "
                 << skip_count << " tests were skipped.\n";
    else if (verbosity == 1)
      log_output << (fail_count ? "failed\n" : "ok\n");

    if (verbosity > 0) {
      log_output.seekg(0);
      std::cerr << log_output.rdbuf();
    }

    return fail_count;
  }

  //! Get the current test.
  static TEST*
  current()
  {
    return utility::Current< TEST >::get();
  }

private:
  enum class Test_result { pass, skip, fail };

  Test_result
  run(std::ostream& output, std::size_t counter, int verbosity)
  {
    if (verbosity > 1)
      print_banner(output, counter);

    try {
      utility::Current< TEST > current_test{ this };

      TEST_SETUP::run_all();

      try {
        function_();
      }
      catch (...) {
        TEST_TEARDOWN::run_all();
        throw;
      }
      TEST_TEARDOWN::run_all();

      if (verbosity > 1)
        output << "Passed\n";
      return Test_result::pass;
    }
    catch (test_skipped const&) {
      if (verbosity > 1)
        output << "Skipped\n";
      return Test_result::skip;
    }
    catch (test_not_implemented const&) {
      if (verbosity > 1)
        output << "Not implemented\n";
      return Test_result::skip;
    }
    catch (int ex) {
      if (verbosity > 1)
        output << "Failed: " << ex << "\n\n";
    }
    catch (test_failed const& ex) {
      if (verbosity > 1)
        output << "Failed\n";
      if (verbosity > 3) {
        output << '\n';
        ex.diagnose(output);
        output << '\n';
      }
    }
    catch (std::exception const& ex) {
      if (verbosity > 1)
        output << "Failed\n";
      if (verbosity > 3) {
        output << '\n'
               << "    Test threw an exception:\n"
               << "    " << ex.what() << "\n"
               << '\n';
      }
    }
    catch (...) {
      if (verbosity > 1)
        output << "Failed\n";
      if (verbosity > 3) {
        output << '\n' << "    No more information available\n" << '\n';
      }
    }

    return Test_result::fail;
  }

  void
  print_banner(std::ostream& output, std::size_t counter)
  {
    std::size_t width{ 1 }, c{ count() };

    while ((c /= 10) > 0)
      ++width;

    std::size_t banner_width{ 51 - width };

    std::string name_substr{ name().substr(0, banner_width - 1) };
    name_substr += ' ';

    output << " ** " << std::setw(width) << std::right << std::setfill(' ')
           << counter << '/' << count() << ' ' << std::setw(banner_width)
           << std::left << std::setfill('.') << name_substr << ':' << ' ';
  }

private:
  std::string const name_;

  std::function< void() > function_;
};

#endif
