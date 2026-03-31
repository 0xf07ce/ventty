#ifndef VENTTY_WIDGET_INPUTDIALOG_H
#define VENTTY_WIDGET_INPUTDIALOG_H

#include "Dialog.h"

#include <string>

namespace ventty
{
/// 텍스트 입력 대화상자 위젯
class InputDialog : public Widget
{
public:
    /// 생성자
    InputDialog();

    /// 제목 설정
    void setTitle(std::string title);

    /// 제목 반환
    std::string const & title() const;

    /// 프롬프트 텍스트 설정
    void setPrompt(std::string prompt);

    /// 프롬프트 텍스트 반환
    std::string const & prompt() const;

    /// 입력값 설정
    void setValue(std::string value);

    /// 입력값 반환
    std::string const & value() const;

    /// 대화상자 결과 반환
    DialogResult result() const;

    /// 결과값 설정
    void setResult(DialogResult r);

    /// 입력 대화상자를 윈도우에 그리기
    void draw(Window & window) override;

    /// 키 이벤트 처리
    bool handleKey(KeyEvent const & event) override;

    /// IME 조합 중인 텍스트 설정
    void setComposingText(std::string text);

    /// 내용에 맞게 크기 자동 조정
    void autoSize(int screenWidth, int screenHeight);

private:
    /// 커서 위치에 문자 삽입
    void insertChar(char32_t ch);

    /// 커서 위치의 문자 삭제
    void deleteChar();

    /// 커서 앞의 문자 삭제
    void backspace();

    /// 커서를 왼쪽으로 이동
    void moveCursorLeft();

    /// 커서를 오른쪽으로 이동
    void moveCursorRight();

    std::string _title;                        ///< 대화상자 제목
    std::string _prompt;                       ///< 프롬프트 텍스트
    std::string _value;                        ///< 현재 입력값
    std::string _composingText;                ///< IME 조합 중인 텍스트
    int _cursorPos = 0;                        ///< 커서 위치
    int _scrollOffset = 0;                     ///< 입력 필드 스크롤 오프셋
    DialogResult _result = DialogResult::None; ///< 대화상자 결과
};
} // namespace ventty

#endif // VENTTY_WIDGET_INPUTDIALOG_H
