#pragma once

namespace ventty
{
/// 너비와 높이를 나타내는 크기
struct Size
{
    int width = 0;  ///< 너비
    int height = 0; ///< 높이
};

/// 위치와 크기를 가진 직사각형 영역
struct Rect
{
    int x = 0;      ///< X 좌표 (열)
    int y = 0;      ///< Y 좌표 (행)
    int width = 0;  ///< 너비
    int height = 0; ///< 높이

    /// 지정한 좌표가 영역 내에 포함되는지 확인
    bool contains(int px, int py) const;
};
} // namespace ventty
