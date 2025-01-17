cmake_minimum_required(VERSION 3.19.0)
include(FetchContent) # Introduced in CMake 3.11
include(GoogleTest) # Introduced in CMake 3.10

include_directories("${PROJECT_SOURCE_DIR}")

##############################
# Download GoogleTest framework
##############################
if(NOT TARGET gtest)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
endif()

##############################
# TARGET: testing
##############################
# cpp-ast tests disabled since this is a deprecated feature that is scheduled to be removed in favor of going directly
# to MLIR ast from parser & python
set(TEST_FILES
        legacy_ast/ast/ASTComparison.cpp
        #ast/ASTComparison.h
        #AstTestingGenerator.cpp
        #AstTestingGenerator.h
        #JsonOutputTest.cpp
        #ast/AssignmentTest.cpp
        #ast/BinaryExpressionTest.cpp
        #ast/BlockTest.cpp
        #ast/CallTest.cpp
        #ast/ExpressionListTest.cpp
        #ast/ForTest.cpp
        #ast/FunctionParameterTest.cpp
        #ast/FunctionTest.cpp
        #ast/IfTest.cpp
        #ast/LiteralTest.cpp
        #ast/OperatorExpressionTest.cpp
        #ast/ReturnTest.cpp
        #ast/TernaryOperatorTest.cpp
        #ast/UnaryExpressionTest.cpp
        #ast/VariableDeclarationTest.cpp
        #ast/VariableTest.cpp
        end-to-end/BoxBlurTest.cpp
        end-to-end/CardioTest.cpp
        end-to-end/DotProductTest.cpp
        end-to-end/GxKernelTest.cpp
        end-to-end/GyKernelTest.cpp
        end-to-end/HammingDistanceTest.cpp
        end-to-end/L2DistanceTest.cpp
        end-to-end/LaplaceSharpeningTest.cpp
        end-to-end/LinearKernelTest.cpp
        end-to-end/MatrixVectorProductTest.cpp
        end-to-end/PolynomialKernelTest.cpp
        end-to-end/RobertsCrossTest.cpp
        legacy_ast/ast_parser/ParserTest.cpp
        legacy_ast/ast_parser/TokenizerTest.cpp
        #ast_visitor/PrintVisitorTest.cpp
        #ast_visitor/ProgramPrintVisitorTest.cpp
        #ast_visitor/VisitorTest.cpp
        bench/MultiTimer.cpp
        bench/BoxBlur.cpp
        bench/L2Distance.cpp
        bench/HammingDistance.cpp
        bench/RobertsCross.cpp
        bench/GxKernel.cpp
        )


add_executable(testing-all
        ${TEST_FILES})

# this is important to have code coverage in CLion
#if (CMAKE_COMPILER_IS_GNUCXX)
#    set(CMAKE_CXX_FLAGS "--coverage")
#elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
#    set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
#endif ()

target_link_libraries(testing-all PRIVATE gtest cpp_ast gtest_main)
target_include_directories(testing-all PRIVATE ${abc_SOURCE_DIR}/include/ bench)

if(MSVC)
    # Mark gtest as external
    target_compile_options(testing-all PRIVATE /external:I${gtest_SOURCE_DIR}/include)
endif()

# create ctest targets
gtest_discover_tests(testing-all TEST_PREFIX gtest:)

# make sure that ast_lib is built before the testing-all target
add_dependencies(testing-all mlir-headers)
add_dependencies(testing-all cpp_ast)

# Python tests
# add_subdirectory(python_frontend)

# MLIR testing setup
add_subdirectory(IR)
