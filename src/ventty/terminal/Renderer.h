#pragma once

#include <ventty/core/Cell.hpp>
#include <ventty/core/Window.h>

#include <string>
#include <vector>

namespace ventty
{
/// Renders a Screen to the terminal using diff-based ANSI escape sequences.
class Renderer
{
public:
    /// 기본 생성자
    Renderer() = default;

    /// 소멸자
    ~Renderer() = default;

    /// 화면 내용을 stdout으로 렌더링 (diff 기반)
    void render(Window const & window);

    /// 다음 render()에서 전체 다시 그리기 강제
    void invalidate();

private:
    /// ANSI 색상 이스케이프 시퀀스를 버퍼에 추가
    void appendAnsiColor(std::string & buf, Style const & style, Style const & prevStyle);

    std::vector<Cell> _prev; ///< 이전 프레임 버퍼
    int _prevWidth = 0;      ///< 이전 프레임 너비
    int _prevHeight = 0;     ///< 이전 프레임 높이
    bool _fullRedraw = true; ///< 전체 다시 그리기 플래그
};
} // namespace ventty
