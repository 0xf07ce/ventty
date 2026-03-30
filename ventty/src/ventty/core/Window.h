#pragma once

#include <ventty/core/Cell.hpp>
#include <ventty/core/Utf8.h>

#include <string>
#include <string_view>
#include <vector>

namespace ventty
{
/// Virtual window — a rectangular region of cells that can be
/// positioned anywhere on the terminal screen.
class Window
{
public:
    /// 위치와 크기를 지정하여 윈도우 생성
    Window(int x, int y, int w, int h);

    // -- drawing --

    /// 기본 스타일로 전체 화면 지우기
    void clear();
    /// 지정한 배경색으로 전체 화면 지우기
    void clear(Color bg);
    /// 지정한 스타일로 전체 화면 지우기
    void clear(Style const & style);
    /// 지정한 위치에 스타일 적용하여 문자 출력
    void putChar(int x, int y, char32_t cp, Style const & style);
    /// 지정한 위치에 전경/배경/속성으로 문자 출력
    void putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr = Attr::None);
    /// 지정한 위치에 스타일 적용하여 텍스트 출력
    void drawText(int x, int y, std::string_view text, Style const & style);
    /// 지정한 위치에 전경/배경/속성으로 텍스트 출력
    void drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr = Attr::None);
    /// 지정한 영역을 문자와 스타일로 채우기
    void fill(int x, int y, int w, int h, char32_t cp, Style const & style);
    /// 지정한 영역을 문자와 전경/배경으로 채우기
    void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg);

    /// Print text with automatic line wrapping + scrollback
    void print(std::string_view text, Style const & style);
    void print(std::string_view text, Color fg, Color bg);

    /// 윈도우 테두리에 박스 그리기 (단선 또는 이중선)
    void drawBox(Style const & style, bool doubleLine = false);
    /// 윈도우 테두리에 전경/배경 색으로 박스 그리기
    void drawBox(Color fg, Color bg, bool doubleLine = false);

    /// 임의 위치에 박스 그리기 (단선 또는 이중선)
    void drawBox(int x, int y, int w, int h, Style const & style, bool doubleLine = false);

    /// 수평 선 그리기
    void drawHLine(int x, int y, int length, char32_t ch, Style const & style);
    /// 수직 선 그리기
    void drawVLine(int x, int y, int length, char32_t ch, Style const & style);

    // -- scrollback --

    /// 스크롤백 버퍼 활성화 (최대 줄 수 지정)
    void enableScroll(int scrollbackLines);
    /// 지정한 줄 수만큼 스크롤 (양수 = 위로)
    void scroll(int lines);
    /// 현재 스크롤 오프셋 반환
    int scrollOffset() const;
    /// 스크롤백 버퍼에 저장된 줄 수 반환
    int scrollbackSize() const;

    // -- position / visibility --

    /// X 좌표(열) 반환
    int x() const;
    /// Y 좌표(행) 반환
    int y() const;
    /// 윈도우 너비 반환
    int width() const;
    /// 윈도우 높이 반환
    int height() const;

    /// 윈도우 위치 설정
    void setPosition(int x, int y);

    /// 윈도우 크기 변경
    void resize(int w, int h);

    /// Z 순서 반환 (높을수록 앞에 표시)
    int zOrder() const;
    /// Z 순서 설정
    void setZOrder(int z);

    /// 표시 여부 반환
    bool visible() const;
    /// 표시 여부 설정
    void setVisible(bool v);

    /// 변경 사항이 있는지 확인
    bool isDirty() const;
    /// 변경됨으로 표시
    void markDirty();
    /// 변경 플래그 초기화
    void clearDirty();

    // -- cell access --

    /// 지정 좌표의 셀 참조 반환
    Cell & cellAt(int x, int y);
    /// 지정 좌표의 셀 상수 참조 반환
    Cell const & cellAt(int x, int y) const;

    /// 렌더러용 버퍼 직접 접근 포인터
    Cell const * data() const;
    /// 버퍼 크기 (셀 수) 반환
    int bufferSize() const;

    // -- cursor for print() --

    /// print() 커서의 X 좌표 반환
    int cursorX() const;
    /// print() 커서의 Y 좌표 반환
    int cursorY() const;
    /// print() 커서 위치 설정
    void setCursor(int x, int y);

private:
    int _x, _y;               ///< 윈도우 위치
    int _width, _height;       ///< 윈도우 크기
    int _zOrder = 0;           ///< Z 순서
    bool _visible = true;      ///< 표시 여부
    bool _dirty = true;        ///< 변경 플래그

    std::vector<Cell> _buffer; ///< 셀 버퍼

    // scrollback
    std::vector<std::vector<Cell>> _scrollback; ///< 스크롤백 버퍼
    int _scrollOffset = 0;     ///< 현재 스크롤 오프셋
    int _maxScrollback = 0;    ///< 최대 스크롤백 줄 수

    // cursor for print()
    int _cursorX = 0;          ///< print() 커서 X 좌표
    int _cursorY = 0;          ///< print() 커서 Y 좌표

    /// 버퍼를 한 줄 위로 스크롤
    void scrollBufferUp();
};
} // namespace ventty
