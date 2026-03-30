#pragma once

#include <ventty/terminal/TerminalBase.h>
#include <ventty/core/Utf8.h>

#include <memory>
#include <string>
#include <vector>

namespace ventty
{
/// Real terminal backend — termios raw mode + ANSI escape sequences.
class Terminal : public TerminalBase
{
public:
    /// 생성자
    Terminal();

    /// 소멸자 (raw 모드 복원)
    ~Terminal() override;

    /// raw 모드 진입, 대체 화면 전환, 마우스 활성화
    bool init();

    /// 터미널 종료 및 리소스 해제
    void shutdown() override;

    /// 이벤트 폴링
    bool pollEvent() override;

    /// 화면 렌더링
    void render() override;

    /// 전체 화면 강제 다시 그리기
    void forceRedraw() override;

    // -- drawing on the root screen --

    /// 기본 스타일로 루트 화면 지우기
    void clear() override;

    /// 지정한 배경색으로 루트 화면 지우기
    void clear(Color bg) override;

    /// 기본 스타일로 문자 출력
    void putChar(int x, int y, char32_t cp) override;

    /// 스타일 적용하여 문자 출력
    void putChar(int x, int y, char32_t cp, Style const & style) override;

    /// 전경/배경/속성으로 문자 출력
    void putChar(int x, int y, char32_t cp, Color fg, Color bg,
        Attr attr = Attr::None) override;

    /// 기본 스타일로 텍스트 출력
    void drawText(int x, int y, std::string_view text) override;

    /// 스타일 적용하여 텍스트 출력
    void drawText(int x, int y, std::string_view text, Style const & style) override;

    /// 전경/배경/속성으로 텍스트 출력
    void drawText(int x, int y, std::string_view text, Color fg, Color bg,
        Attr attr = Attr::None) override;

    /// 전경/배경으로 영역 채우기
    void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg) override;

    /// 스타일 적용하여 영역 채우기
    void fill(int x, int y, int w, int h, char32_t cp, Style const & style) override;

    /// 기본 스타일 설정
    void setDefaultStyle(Style const & style) override;

    // -- window management --

    /// 윈도우 생성
    Window * createWindow(int x, int y, int w, int h) override;

    /// 윈도우 제거
    void destroyWindow(Window * win) override;

    // -- queries --

    /// 터미널 열 수 반환
    int cols() const override;

    /// 터미널 행 수 반환
    int rows() const override;

    // -- callbacks --

    /// 키 이벤트 콜백 등록
    void onKey(KeyCallback cb) override;

    /// 마우스 이벤트 콜백 등록
    void onMouse(MouseCallback cb) override;

    /// 크기 변경 이벤트 콜백 등록
    void onResize(ResizeCallback cb) override;

    // -- direct cell access --

    /// 지정 좌표의 셀 참조 반환
    Cell & cellAt(int x, int y) override;

    /// 지정 좌표의 셀 상수 참조 반환
    Cell const & cellAt(int x, int y) const override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl; ///< 구현 세부사항 (PIMPL)

    int _cols = 0; ///< 터미널 열 수
    int _rows = 0; ///< 터미널 행 수

    /// 시그널 핸들러의 긴급 복원을 위한 프렌드 선언
    friend void signalHandler(int);

    Style _defaultStyle; ///< 기본 스타일

    std::vector<Cell> _root;   ///< 루트 (배경) 레이어
    std::vector<Cell> _screen; ///< 합성된 프레임
    std::vector<Cell> _prev;   ///< 이전 프레임 (diff 비교용)
    bool _fullRedraw = true;   ///< 전체 다시 그리기 플래그

    std::vector<std::unique_ptr<Window>> _windows; ///< 관리 중인 윈도우 목록

    KeyCallback _keyCb;       ///< 키 이벤트 콜백
    MouseCallback _mouseCb;   ///< 마우스 이벤트 콜백
    ResizeCallback _resizeCb; ///< 크기 변경 콜백

    /// 터미널 크기 조회
    void querySize();

    /// 윈도우들을 _screen에 합성
    void composite();

    /// 변경된 셀에 대한 ANSI 이스케이프 출력
    void emitDiff();

    /// 단일 셀을 ANSI 이스케이프로 출력
    void emitCell(int x, int y, Cell const & cell);

    /// 크기 변경 처리
    void handleResize();
};
} // namespace ventty
