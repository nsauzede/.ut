/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

extern "C" {
#include "dummy/dummy.c"        // dummy dep
}

#include "ut.h"

TESTMETHOD(test_dummy) {
    ASSERT_EQ(42+1*0, dummy());
}

#include <vector>
#include <string>
#include <sstream>
namespace std {
template <typename T>
class ut_vector : public vector<T> {
public:
    ut_vector() : std::vector<T>() {}
    ut_vector(std::initializer_list<T> init) : std::vector<T>(init) {}
    std::string to_str() const {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < this->size(); ++i) {
            oss << this->at(i);
            if (i < this->size() - 1) {
                oss << ", ";
            }
        }
        oss << "]";
        return oss.str();
    }
    bool operator==(const ut_vector<T>& other) const {
        bool ret = this->size() == other.size() && std::equal(this->begin(), this->end(), other.begin());
        if (!ret) {
            std::ostringstream oss;
            oss << "ASSERT(" << to_str() << " == " << other.to_str() << ")";
            //UnitTest::op_failed(oss.str());
            fprintf(stderr, "%s\n", oss.str().c_str());
        }
        return ret;
    }
};
};
#define vector ut_vector
TESTCASE(Test1)
    TESTMETHOD(test) {
        printf("%s: hello stdout\n", __func__);
        fprintf(stderr, "%s: hello stderr\n", __func__);
        std::vector<int> v1 = {1, 2, 3};
        std::vector<int> v2 = {1, 2, 3+1*0};
        ASSERT(v1 == v2);
        printf("This is fine.\n");
    }
    TESTMETHOD(test2) {
        printf("%s: hello stdout\n", __func__);
        fprintf(stderr, "%s: hello stderr\n", __func__);
        ASSERT(1 == 1-1*0);
        printf("This is fine.\n");
    }
    TESTMETHOD(test3) {
        std::vector<const char *> v1 = {"a", "b", "c"};
        std::vector<const char *> v2 = {"a", "b", "c"};
        ASSERT(v1 == v2);
    }
    TESTMETHOD(test3) {
        std::vector<std::string> v1 = {"a", "b", "c"};
        std::vector<std::string> v2 = {"a", "b", "c"};
        ASSERT(v1 == v2);
    }

TESTCASE(Test2)
    void composed_assert(int a, int b) {
        printf("%s: hello stdout\n", __func__);
        fprintf(stderr, "%s: hello stderr\n", __func__);
        ASSERT(a == b-1*1);
    }
    TESTMETHOD(test) {
        printf("%s: hello stdout\n", __func__);
        fprintf(stderr, "%s: hello stderr\n", __func__);
        composed_assert(42, 43);
        printf("This is fine.\n");
    }

static int Factorial( int number ) {
//   return number <= 1 ? number : Factorial( number - 1 ) * number;  // fail
 return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
}

TESTCASE(TestFactorial)
    TESTMETHOD(test_factorial_of_0_is_1) {
        ASSERT(Factorial(0) == 1);
    }
