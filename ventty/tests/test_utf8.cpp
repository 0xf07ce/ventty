#include <ventty/Utf8.h>

#include <cassert>
#include <cstdio>

using namespace ventty;

static void testDecodeAscii()
{
    std::string_view s = "ABC";
    size_t pos = 0;
    assert(utf8::decode(s, pos) == U'A');
    assert(pos == 1);
    assert(utf8::decode(s, pos) == U'B');
    assert(utf8::decode(s, pos) == U'C');
    assert(pos == 3);
}

static void testDecodeHangul()
{
    std::string_view s = "가나다";
    auto cps = utf8::toCodepoints(s);
    assert(cps.size() == 3);
    assert(cps[0] == U'가');
    assert(cps[1] == U'나');
    assert(cps[2] == U'다');
}

static void testEncode()
{
    std::string out;
    utf8::encode(U'A', out);
    assert(out == "A");

    out.clear();
    utf8::encode(U'가', out);
    assert(out.size() == 3);
    assert(static_cast<uint8_t>(out[0]) == 0xEA);
    assert(static_cast<uint8_t>(out[1]) == 0xB0);
    assert(static_cast<uint8_t>(out[2]) == 0x80);
}

static void testRoundtrip()
{
    std::string original = "Hello 안녕 世界 🌍";
    auto cps = utf8::toCodepoints(original);
    auto reconstructed = utf8::fromCodepoints(cps);
    assert(original == reconstructed);
}

static void testFullwidth()
{
    assert(utf8::isFullwidth(U'가'));
    assert(utf8::isFullwidth(U'漢'));
    assert(utf8::isFullwidth(U'Ａ'));
    assert(!utf8::isFullwidth(U'A'));
    assert(!utf8::isFullwidth(U'é'));
    assert(!utf8::isFullwidth(U' '));
}

static void testDisplayWidth()
{
    assert(utf8::displayWidth(U'A') == 1);
    assert(utf8::displayWidth(U'가') == 2);
    assert(utf8::displayWidth(U'漢') == 2);
}

static void testStringWidth()
{
    assert(utf8::stringWidth("ABC") == 3);
    assert(utf8::stringWidth("가나다") == 6);
    assert(utf8::stringWidth("A가B") == 4);
    assert(utf8::stringWidth("") == 0);
}

static void testInvalidUtf8()
{
    std::string_view bad = "\xff\xfe";
    size_t pos = 0;
    auto cp = utf8::decode(bad, pos);
    assert(cp == U'\uFFFD');
}

int main()
{
    testDecodeAscii();
    testDecodeHangul();
    testEncode();
    testRoundtrip();
    testFullwidth();
    testDisplayWidth();
    testStringWidth();
    testInvalidUtf8();

    std::printf("test_utf8: all tests passed\n");
    return 0;
}
