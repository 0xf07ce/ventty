#ifndef VENTTY_WIDGET_DIALOG_H
#define VENTTY_WIDGET_DIALOG_H

#include "Widget.h"

#include <functional>
#include <string>
#include <vector>

namespace ventty
{
/// 대화상자 결과
enum class DialogResult
{
    None,   ///< 결과 없음
    Ok,     ///< 확인
    Cancel, ///< 취소
    Yes,    ///< 예
    No,     ///< 아니오
};

/// 메시지 대화상자 위젯
class Dialog : public Widget
{
public:
    /// 생성자
    Dialog();

    /// 제목 설정
    void setTitle(std::string title);

    /// 제목 반환
    std::string const & title() const;

    /// 메시지 설정
    void setMessage(std::string message);

    /// 메시지 반환
    std::string const & message() const;

    /// 버튼 추가 (라벨과 결과값)
    void addButton(std::string label, DialogResult result);

    /// 모든 버튼 제거
    void clearButtons();

    /// 대화상자 결과 반환
    DialogResult result() const;

    /// 결과값 설정
    void setResult(DialogResult r);

    /// 선택된 버튼 인덱스 반환
    int selectedButton() const;

    /// 선택 버튼 인덱스 설정
    void setSelectedButton(int idx);

    /// 대화상자를 윈도우에 그리기
    void draw(Window & window) override;

    /// 키 이벤트 처리
    bool handleKey(KeyEvent const & event) override;

    /// 내용에 맞게 크기 자동 조정
    void autoSize(int screenWidth, int screenHeight);

private:
    std::string _title;                                         ///< 대화상자 제목
    std::string _message;                                       ///< 대화상자 메시지
    std::vector<std::pair<std::string, DialogResult>> _buttons; ///< 버튼 목록 (라벨, 결과값 쌍)
    int _selectedButton = 0;                                    ///< 선택된 버튼 인덱스
    DialogResult _result = DialogResult::None;                  ///< 대화상자 결과
};
} // namespace ventty

#endif // VENTTY_WIDGET_DIALOG_H
