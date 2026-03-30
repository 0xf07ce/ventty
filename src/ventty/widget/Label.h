#ifndef VENTTY_WIDGET_LABEL_H
#define VENTTY_WIDGET_LABEL_H

#include "Widget.h"

#include <string>

namespace ventty
{
/// 텍스트 정렬 방향
enum class Align
{
    Left,   ///< 왼쪽 정렬
    Center, ///< 가운데 정렬
    Right,  ///< 오른쪽 정렬
};

/// 단순 텍스트 표시 위젯
class Label : public Widget
{
public:
    Label() = default;

    /// 텍스트를 지정하여 라벨 생성
    explicit Label(std::string text);

    /// 텍스트 반환
    std::string const & text() const;

    /// 텍스트 설정
    void setText(std::string text);

    /// 스타일 반환
    Style const & style() const;

    /// 스타일 설정
    void setStyle(Style const & s);

    /// 정렬 방향 반환
    Align align() const;

    /// 정렬 방향 설정
    void setAlign(Align a);

    /// 라벨을 윈도우에 그리기
    void draw(Window & window) override;

private:
    std::string _text;          ///< 표시할 텍스트
    Style _style;               ///< 라벨 스타일
    Align _align = Align::Left; ///< 텍스트 정렬 방향
};
} // namespace ventty

#endif // VENTTY_WIDGET_LABEL_H
