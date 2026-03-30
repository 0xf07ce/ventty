#pragma once

#include <ventty/terminal/TerminalBase.h>

#include <optional>
#include <string>

namespace ventty
{
/// Standalone input reader for blocking/timeout key reading.
/// Useful for widget-based applications that need synchronous input.
class Input
{
public:
    /// 생성자
    Input();

    /// 소멸자
    ~Input();

    /// 키 이벤트 읽기 (타임아웃 시 Key::None 반환, -1은 무한 대기)
    KeyEvent read(int timeoutMs = -1);

    /// 입력 가능 여부 확인
    bool poll(int timeoutMs = 0);

private:
    /// 바이트 한 개 읽기 (타임아웃 지원)
    int readByte(int timeoutMs);

    /// ESC 시퀀스 파싱
    KeyEvent parseEscapeSequence();

    /// CSI 시퀀스 파싱
    KeyEvent parseCsiSequence();

    /// SS3 시퀀스 파싱
    KeyEvent parseSs3Sequence();

    static constexpr int ESC_TIMEOUT_MS = 50; ///< ESC 키 타임아웃 (밀리초)
};
} // namespace ventty
