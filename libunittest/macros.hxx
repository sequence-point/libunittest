
#ifndef libunittest__macros_hxx_
#define libunittest__macros_hxx_

// I really, really, really (get it?) *H**E* having to use
// these pesky macros. They're a f*****g abomination.

#define ON_TEST_SETUP_3(line)                                                  \
  void test_setup_func_##line();                                               \
  TEST_SETUP test_setup_##line(test_setup_func_##line);                        \
  void test_setup_func_##line()
#define ON_TEST_SETUP_2(line) ON_TEST_SETUP_3(line)
#define ON_TEST_SETUP ON_TEST_SETUP_2(__LINE__)

#define ON_TEST_TEARDOWN_3(line)                                               \
  void test_teardown_func_##line();                                            \
  TEST_TEARDOWN test_teardown_##line(test_teardown_func_##line);               \
  void test_teardown_func_##line()
#define ON_TEST_TEARDOWN_2(line) ON_TEST_TEARDOWN_3(line)
#define ON_TEST_TEARDOWN ON_TEST_TEARDOWN_2(__LINE__)

#define DEFINE_TEST_3(name, line)                                              \
  void test_func_##line();                                                     \
  TEST test_##line(name, test_func_##line);                                    \
  void test_func_##line()
#define DEFINE_TEST_2(name, line) DEFINE_TEST_3(name, line)
#define DEFINE_TEST(name) DEFINE_TEST_2(name, __LINE__)

#define TEST_SKIP TEST::current()->skip_test()

#define TEST_NOT_IMPLEMENTED TEST::current()->not_implemented()

#define TEST_TRUE(Expr) TEST::current()->test_true(Expr, __FILE__, __LINE__)

#define TEST_FALSE(Expr) TEST::current()->test_false(Expr, __FILE__, __LINE__)

#define TEST_EQUAL(LeftExpr, RightExpr)                                        \
  TEST::current()->test_equal(LeftExpr, RightExpr, __FILE__, __LINE__)

#define TEST_DIFFERENT(LeftExpr, RightExpr)                                    \
  TEST::current()->test_different(LeftExpr, RightExpr, __FILE__, __LINE__)

#define TEST_THROWS(Exception, Block)                                          \
  TEST::current()->test_throws< Exception >([&]() { Block; }, __FILE__,        \
                                            __LINE__)

#define TEST_THROWS_ANY(Block)                                                 \
  TEST::current()->test_throws_any([&]() { Block; }, __FILE__, __LINE__)

#define TEST_DOES_NOT_THROW(Block)                                             \
  TEST::current()->test_does_not_throw([&]() { Block; }, __FILE__, __LINE__)

#define TEST_ENSURE_REACHED(Point, Block)                                      \
  TEST::current()->ensure_reached(                                             \
    Point, [&]() { Block; }, __FILE__, __LINE__)

#define TEST_POINT_REACHED(Point) TEST::current()->point_reached(Point)

#endif
