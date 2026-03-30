#pragma once

#include <ventty/core/Cell.hpp>
#include <ventty/core/Color.h>

#include <functional>
#include <string_view>

namespace ventty
{
class Window;

/// Key event from terminal input
struct KeyEvent
{
    /// 키 종류
    enum class Key : int
    {
        None = 0,  ///< 없음
        Up,        ///< 위쪽 화살표
        Down,      ///< 아래쪽 화살표
        Left,      ///< 왼쪽 화살표
        Right,     ///< 오른쪽 화살표
        Home,      ///< Home 키
        End,       ///< End 키
        PageUp,    ///< Page Up 키
        PageDown,  ///< Page Down 키
        Insert,    ///< Insert 키
        Delete,    ///< Delete 키
        Backspace, ///< Backspace 키
        Tab,       ///< Tab 키
        Enter,     ///< Enter 키
        Escape,    ///< Escape 키
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        Char,      ///< 일반 문자 — `ch` 참조
    };

    Key key = Key::None; ///< 눌린 키 종류
    char32_t ch = 0;     ///< 유니코드 코드포인트 (key == Key::Char일 때)
    bool shift = false;  ///< Shift 키 눌림 여부
    bool ctrl = false;   ///< Ctrl 키 눌림 여부
    bool alt = false;    ///< Alt 키 눌림 여부
};

/// Mouse button event
struct MouseEvent
{
    /// 마우스 버튼 종류
    enum class Button : uint8_t
    {
        None = 0,   ///< 없음
        Left,       ///< 왼쪽 버튼
        Middle,     ///< 가운데 버튼
        Right,      ///< 오른쪽 버튼
        ScrollUp,   ///< 스크롤 위로
        ScrollDown, ///< 스크롤 아래로
    };

    /// 마우스 동작 종류
    enum class Action : uint8_t
    {
        Press,   ///< 누르기
        Release, ///< 놓기
        Move,    ///< 이동
    };

    Button button = Button::None;  ///< 눌린 마우스 버튼
    Action action = Action::Press; ///< 마우스 동작
    int x = 0;                     ///< 열 좌표 (0 기반)
    int y = 0;                     ///< 행 좌표 (0 기반)
};

/// Terminal resize event
struct ResizeEvent
{
    int cols = 0; ///< 변경된 열 수
    int rows = 0; ///< 변경된 행 수
};

/// 키 이벤트 콜백 타입
using KeyCallback = std::function<void(KeyEvent const &)>;

/// 마우스 이벤트 콜백 타입
using MouseCallback = std::function<void(MouseEvent const &)>;

/// 크기 변경 이벤트 콜백 타입
using ResizeCallback = std::function<void(ResizeEvent const &)>;

/// Abstract terminal interface — backend-agnostic.
/// Concrete implementations: AnsiTerminal (ANSI/VT), gfx::GfxTerminal (SDL3).
class TerminalBase
{
public:
    virtual ~TerminalBase() = default;

    TerminalBase(TerminalBase const &) = delete;
    TerminalBase & operator=(TerminalBase const &) = delete;

    // -- lifecycle --

    /// 터미널 종료 및 리소스 해제
    virtual void shutdown() = 0;

    /// 이벤트 폴링 (이벤트가 있으면 true 반환)
    virtual bool pollEvent() = 0;

    /// 화면 렌더링
    virtual void render() = 0;

    /// 전체 화면 강제 다시 그리기
    virtual void forceRedraw() = 0;

    // -- drawing on root screen --

    /// 기본 스타일로 루트 화면 지우기
    virtual void clear() = 0;

    /// 지정한 배경색으로 루트 화면 지우기
    virtual void clear(Color bg) = 0;

    /// 기본 스타일로 문자 출력
    virtual void putChar(int x, int y, char32_t cp) = 0;

    /// 스타일 적용하여 문자 출력
    virtual void putChar(int x, int y, char32_t cp, Style const & style) = 0;

    /// 전경/배경/속성으로 문자 출력
    virtual void putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr = Attr::None) = 0;

    /// 기본 스타일로 텍스트 출력
    virtual void drawText(int x, int y, std::string_view text) = 0;

    /// 스타일 적용하여 텍스트 출력
    virtual void drawText(int x, int y, std::string_view text, Style const & style) = 0;

    /// 전경/배경/속성으로 텍스트 출력
    virtual void drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr = Attr::None) = 0;

    /// 전경/배경으로 영역 채우기
    virtual void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg) = 0;

    /// 스타일 적용하여 영역 채우기
    virtual void fill(int x, int y, int w, int h, char32_t cp, Style const & style) = 0;

    /// 기본 스타일 설정
    virtual void setDefaultStyle(Style const & style) = 0;

    // -- window management --

    /// 윈도우 생성 (위치와 크기 지정)
    virtual Window * createWindow(int x, int y, int w, int h) = 0;

    /// 윈도우 제거
    virtual void destroyWindow(Window * win) = 0;

    // -- queries --

    /// 터미널 열 수 반환
    virtual int cols() const = 0;

    /// 터미널 행 수 반환
    virtual int rows() const = 0;

    /// 실행 중인지 확인
    bool isRunning() const { return _running; }

    /// 종료 요청
    void quit() { _running = false; }

    // -- callbacks --

    /// 키 이벤트 콜백 등록
    virtual void onKey(KeyCallback cb) = 0;

    /// 마우스 이벤트 콜백 등록
    virtual void onMouse(MouseCallback cb) = 0;

    /// 크기 변경 이벤트 콜백 등록
    virtual void onResize(ResizeCallback cb) = 0;

    // -- direct cell access --

    /// 지정 좌표의 셀 참조 반환
    virtual Cell & cellAt(int x, int y) = 0;

    /// 지정 좌표의 셀 상수 참조 반환
    virtual Cell const & cellAt(int x, int y) const = 0;

protected:
    TerminalBase() = default;
    bool _running = false; ///< 실행 상태 플래그
};
} // namespace ventty
