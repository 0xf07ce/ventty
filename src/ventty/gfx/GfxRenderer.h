#pragma once

#include <ventty/gfx/GfxFont.h>

#include <string>

struct SDL_Window;
struct SDL_Renderer;

namespace ventty
{
struct Cell;
class Window;

class GfxRenderer
{
public:
    /// 기본 생성자
    GfxRenderer() = default;

    /// 소멸자 (SDL 리소스 해제)
    ~GfxRenderer();

    GfxRenderer(GfxRenderer const &) = delete;
    GfxRenderer & operator=(GfxRenderer const &) = delete;

    /// SDL 윈도우 및 렌더러 초기화
    bool init(int cols, int rows, int cellW, int cellH, std::string const & title, int scale = 0);

    /// SDL 리소스 해제
    void shutdown();

    /// 프레임 렌더링 시작
    void beginFrame();

    /// 프레임 렌더링 완료 및 표시
    void endFrame();

    /// 단일 셀을 픽셀 좌표에 렌더링
    void renderCell(ventty::Cell const & cell, int px, int py, GfxFont & font, int cellW, int cellH);

    /// 윈도우의 모든 셀 렌더링
    void renderWindow(ventty::Window & win, GfxFont & font);

    /// SDL 윈도우 포인터 반환
    SDL_Window * sdlWindow() const;

    /// SDL 렌더러 포인터 반환
    SDL_Renderer * sdlRenderer() const;

    /// 스케일 배율 반환
    int scale() const;

    /// 논리적 크기 업데이트
    void updateLogicalSize(int fbW, int fbH);

private:
    SDL_Window * _sdlWindow = nullptr;  ///< SDL 윈도우
    SDL_Renderer * _renderer = nullptr; ///< SDL 렌더러
    int _scale = 1;                     ///< 스케일 배율
    int _fbWidth = 0;                   ///< 프레임 버퍼 너비
    int _fbHeight = 0;                  ///< 프레임 버퍼 높이
    int _cellW = 8;                     ///< 셀 너비 (픽셀)
    int _cellH = 16;                    ///< 셀 높이 (픽셀)
};
} // namespace ventty
