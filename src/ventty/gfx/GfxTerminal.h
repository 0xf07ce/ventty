#pragma once

#include <ventty/gfx/GfxFont.h>
#include <ventty/gfx/GfxInput.h>
#include <ventty/gfx/GfxRenderer.h>

#include <ventty/terminal/TerminalBase.h>
#include <ventty/core/Window.h>

#include <memory>
#include <string>
#include <vector>

namespace ventty
{
class GfxTerminal : public TerminalBase
{
public:
    /// 생성자
    GfxTerminal();

    /// 소멸자
    ~GfxTerminal() override;

    /// SDL3 윈도우, 렌더러 초기화
    bool init(int cols, int rows, std::string const & title, int scale = 0);

    /// 아틀라스 폰트 로드 (다중 호출로 다중 아틀라스 지원)
    bool loadFont(std::string const & pngPath, std::string const & fntPath);

    /// 내장 폰트(CP437 + 한글) 로드
    bool loadBuiltinFont();

    /// 종료 및 리소스 해제
    void shutdown() override;

    /// 이벤트 폴링
    bool pollEvent() override;

    /// 화면 렌더링
    void render() override;

    /// 전체 화면 강제 다시 그리기
    void forceRedraw() override;

    // -- drawing on root screen --

    /// 기본 스타일로 루트 화면 지우기
    void clear() override;

    /// 지정한 배경색으로 루트 화면 지우기
    void clear(Color bg) override;

    /// 기본 스타일로 문자 출력
    void putChar(int x, int y, char32_t cp) override;

    /// 스타일 적용하여 문자 출력
    void putChar(int x, int y, char32_t cp, Style const & style) override;

    /// 전경/배경/속성으로 문자 출력
    void putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr = Attr::None) override;

    /// 기본 스타일로 텍스트 출력
    void drawText(int x, int y, std::string_view text) override;

    /// 스타일 적용하여 텍스트 출력
    void drawText(int x, int y, std::string_view text, Style const & style) override;

    /// 전경/배경/속성으로 텍스트 출력
    void drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr = Attr::None) override;

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

    /// 셀 너비 (픽셀) 반환
    int cellWidth() const;

    /// 셀 높이 (픽셀) 반환
    int cellHeight() const;

    // -- callbacks --

    /// 키 이벤트 콜백 등록
    void onKey(KeyCallback cb) override;

    /// 마우스 이벤트 콜백 등록
    void onMouse(MouseCallback cb) override;

    /// 크기 변경 이벤트 콜백 등록
    void onResize(ResizeCallback cb) override;

    /// 텍스트 입력(IME 확정) 콜백 등록
    void onTextInput(TextInputCallback cb);

    /// 텍스트 편집(IME 조합) 콜백 등록
    void onTextEditing(TextEditingCallback cb);

    // -- IME --

    /// 텍스트 입력(IME) 시작
    void startTextInput();

    /// 텍스트 입력(IME) 중지
    void stopTextInput();

    // -- direct cell access --

    /// 지정 좌표의 셀 참조 반환
    Cell & cellAt(int x, int y) override;

    /// 지정 좌표의 셀 상수 참조 반환
    Cell const & cellAt(int x, int y) const override;

    /// 실행 파일 기본 경로 반환 (SDL_GetBasePath 래퍼)
    static std::string basePath();

private:
    int _cols = 80; ///< 터미널 열 수
    int _rows = 25; ///< 터미널 행 수

    Style _defaultStyle; ///< 기본 스타일

    GfxFont _font;         ///< 비트맵 아틀라스 폰트
    GfxRenderer _renderer; ///< SDL 렌더러
    GfxInput _input;       ///< 입력 처리기

    bool _fontLoaded = false; ///< 폰트 로드 여부

    std::unique_ptr<Window> _rootScreen;           ///< 루트 화면 윈도우
    std::vector<std::unique_ptr<Window>> _windows; ///< 관리 중인 윈도우 목록

    ResizeCallback _resizeCb; ///< 크기 변경 콜백
};
} // namespace ventty
