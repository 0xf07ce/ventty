#pragma once

#include <ventty/terminal/TerminalBase.h>

#include <functional>
#include <string>
#include <vector>

union SDL_Event;
struct SDL_Window;

namespace ventty
{
/// IME committed text event
/// IME 확정 텍스트 이벤트
struct TextInputEvent
{
    std::string text; ///< 확정된 텍스트
};

/// IME composing text event
/// IME 조합 중 텍스트 이벤트
struct TextEditingEvent
{
    std::string text; ///< 조합 중인 텍스트
    int cursor = 0;   ///< 조합 커서 위치
    int length = 0;   ///< 조합 텍스트 길이
};

/// 텍스트 입력 콜백 타입
using TextInputCallback = std::function<void(TextInputEvent const &)>;

/// 텍스트 편집 콜백 타입
using TextEditingCallback = std::function<void(TextEditingEvent const &)>;

/// SDL 이벤트를 ventty 이벤트로 변환하는 입력 처리기
class GfxInput
{
public:
    /// 셀 크기 설정 (픽셀)
    void setCellSize(int cellW, int cellH);

    /// 스케일 배율 설정
    void setScale(int scale);

    /// SDL 이벤트 처리 (처리했으면 true 반환)
    bool processEvent(SDL_Event const & event);

    /// 프레임 종료 시 호출
    void endFrame();

    // Callbacks (ventty-compatible types)

    /// 키 이벤트 콜백 등록
    void onKey(ventty::KeyCallback cb);

    /// 마우스 이벤트 콜백 등록
    void onMouse(ventty::MouseCallback cb);

    /// 텍스트 입력 콜백 등록
    void onTextInput(TextInputCallback cb);

    /// 텍스트 편집 콜백 등록
    void onTextEditing(TextEditingCallback cb);

    // IME

    /// SDL 윈도우 설정 (IME용)
    void setWindow(SDL_Window * window);

    /// 텍스트 입력(IME) 시작
    void startTextInput();

    /// 텍스트 입력(IME) 중지
    void stopTextInput();

    /// 텍스트 입력 활성 여부 확인
    bool isTextInputActive() const;

    /// 마우스 셀 X 좌표 반환
    int mouseX() const;

    /// 마우스 셀 Y 좌표 반환
    int mouseY() const;

private:
    /// 픽셀 좌표를 셀 좌표로 변환
    std::pair<int, int> pixelToCell(float px, float py) const;

    int _cellWidth = 8;   ///< 셀 너비 (픽셀)
    int _cellHeight = 16; ///< 셀 높이 (픽셀)
    int _scale = 1;       ///< 스케일 배율
    int _mouseCellX = 0;  ///< 마우스 셀 X 좌표
    int _mouseCellY = 0;  ///< 마우스 셀 Y 좌표

    SDL_Window * _window = nullptr; ///< SDL 윈도우
    bool _textInputActive = false;  ///< 텍스트 입력 활성 여부

    std::vector<ventty::KeyCallback> _keyCallbacks;         ///< 키 이벤트 콜백 목록
    std::vector<ventty::MouseCallback> _mouseCallbacks;     ///< 마우스 이벤트 콜백 목록
    std::vector<TextInputCallback> _textInputCallbacks;     ///< 텍스트 입력 콜백 목록
    std::vector<TextEditingCallback> _textEditingCallbacks; ///< 텍스트 편집 콜백 목록
};
} // namespace ventty
