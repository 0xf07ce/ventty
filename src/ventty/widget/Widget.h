#ifndef VENTTY_WIDGET_WIDGET_H
#define VENTTY_WIDGET_WIDGET_H

#include <ventty/core/Color.h>
#include <ventty/core/Rect.h>
#include <ventty/core/Window.h>
#include <ventty/terminal/TerminalBase.h>

namespace ventty
{
/// 위젯 기본 클래스
class Widget
{
public:
    Widget() = default;

    virtual ~Widget() = default;

    // Geometry

    /// 위젯 영역 반환
    Rect const & rect() const;

    /// Rect로 위젯 영역 설정
    void setRect(Rect const & r);

    /// 좌표와 크기로 위젯 영역 설정
    void setRect(int x, int y, int w, int h);

    /// X 좌표 반환
    int x() const;

    /// Y 좌표 반환
    int y() const;

    /// 너비 반환
    int width() const;

    /// 높이 반환
    int height() const;

    // Visibility

    /// 표시 여부 반환
    bool isVisible() const;

    /// 표시 여부 설정
    void setVisible(bool v);

    /// 위젯 표시
    void show();

    /// 위젯 숨기기
    virtual void hide();

    // Focus

    /// 포커스 여부 반환
    bool isFocused() const;

    /// 포커스 설정
    void setFocused(bool f);

    // Rendering

    /// 위젯을 윈도우에 그리기 (순수 가상 함수)
    virtual void draw(Window & window) = 0;

    // Input handling

    /// 키 이벤트 처리 (처리했으면 true 반환)
    virtual bool handleKey(KeyEvent const & event);

protected:
    /// 크기 변경 시 호출되는 콜백
    virtual void onResize();

    /// 포커스 변경 시 호출되는 콜백
    virtual void onFocusChanged();

    Rect _rect;            ///< 위젯 영역
    bool _visible = true;  ///< 표시 여부
    bool _focused = false; ///< 포커스 여부
};
} // namespace ventty

#endif // VENTTY_WIDGET_WIDGET_H
