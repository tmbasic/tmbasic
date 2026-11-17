#include "../common.h"
#include "gtest/gtest.h"
#include "shared/SystemCalls.h"

class BaseClass {
   public:
    virtual ~BaseClass() = default;

    // Rule of five implementation
    BaseClass() = default;
    BaseClass(const BaseClass&) = default;
    BaseClass& operator=(const BaseClass&) = default;
    BaseClass(BaseClass&&) = default;
    BaseClass& operator=(BaseClass&&) = default;
};

class TestClass : public BaseClass {
   public:
    int memberVariable{};
};

enum class TestEnum { kValue };

TEST(NameofTest, BaseClassName) {
#ifdef _WIN32
    ASSERT_EQ("class BaseClass", NAMEOF_TYPE(BaseClass));
#else
    ASSERT_EQ("BaseClass", NAMEOF_TYPE(BaseClass));
#endif
}

TEST(NameofTest, TestClassName) {
#ifdef _WIN32
    ASSERT_EQ("class TestClass", NAMEOF_TYPE(TestClass));
#else
    ASSERT_EQ("TestClass", NAMEOF_TYPE(TestClass));
#endif
}

TEST(NameofTest, MemberVariableName) {
    TestClass const x;
    ASSERT_EQ("memberVariable", NAMEOF(x.memberVariable));
}

TEST(NameofTest, EnumLiteralName) {
    ASSERT_EQ("kValue", NAMEOF_ENUM(TestEnum::kValue));
}

TEST(NameofTest, EnumVariableName) {
    auto x = TestEnum::kValue;
    ASSERT_EQ("kValue", NAMEOF_ENUM(x));
}

TEST(NameofTest, AllSystemCallsHaveNames) {
    for (int i = 0; i < static_cast<int>(shared::SystemCall::kMaxSystemCall); ++i) {
        auto call = static_cast<shared::SystemCall>(i);
        ASSERT_FALSE(NAMEOF_ENUM(call).empty()) << "System call " << i << " has no name";
    }
}

TEST(NameofTest, SystemCallNamesAboveDefaultRange) {
    using shared::SystemCall;
    ASSERT_EQ("kValueToValueMapSet", NAMEOF_ENUM(SystemCall::kValueToValueMapSet));
    ASSERT_EQ("kValueToValueMapUnion", NAMEOF_ENUM(SystemCall::kValueToValueMapUnion));
    ASSERT_EQ("kWriteFileBytes", NAMEOF_ENUM(SystemCall::kWriteFileBytes));
    ASSERT_EQ("kWriteFileLines", NAMEOF_ENUM(SystemCall::kWriteFileLines));
    ASSERT_EQ("kWriteFileText", NAMEOF_ENUM(SystemCall::kWriteFileText));
}

TEST(NameofTest, InvalidSystemCallName) {
    ASSERT_EQ("unknown!", NAMEOF_ENUM_OR(static_cast<shared::SystemCall>(-1), "unknown!"));
    ASSERT_EQ("unknown!", NAMEOF_ENUM_OR(shared::SystemCall::kMaxSystemCall, "unknown!"));
}
