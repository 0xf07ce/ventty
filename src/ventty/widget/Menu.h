#ifndef VENTTY_WIDGET_MENU_H
#define VENTTY_WIDGET_MENU_H

#include "Widget.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ventty
{
/// 메뉴 항목
struct MenuItem
{
    std::string label;                       ///< 항목 라벨
    std::string shortcut;                    ///< 단축키 표시 텍스트
    KeyEvent::Key key = KeyEvent::Key::None; ///< 연결된 키
    bool separator = false;                  ///< 구분선 여부
    bool enabled = true;                     ///< 활성화 여부
    std::function<void()> action;            ///< 선택 시 실행할 동작

    /// 구분선 항목 생성
    static MenuItem makeSeparator();
};

/// 팝업 메뉴 위젯
class Menu : public Widget
{
public:
    /// 생성자
    Menu();

    /// 메뉴 항목 추가
    void addItem(MenuItem item);

    /// 구분선 추가
    void addSeparator();

    /// 모든 항목 제거
    void clear();

    /// 항목 목록 반환
    std::vector<MenuItem> const & items() const;

    /// 선택된 항목 인덱스 반환
    int selectedIndex() const;

    /// 선택 인덱스 설정
    void setSelectedIndex(int idx);

    /// 지정 위치에 메뉴 표시
    void show(int x, int y);

    /// 메뉴 숨기기
    void hide() override;

    /// 메뉴가 열려 있는지 확인
    bool isOpen() const;

    /// 선택된 항목 실행
    bool execute();

    /// 메뉴를 윈도우에 그리기
    void draw(Window & window) override;

    /// 키 이벤트 처리
    bool handleKey(KeyEvent const & event) override;

    /// 내용에 맞게 크기 자동 조정
    void autoSize();

private:
    /// 선택을 위로 이동
    void moveSelectionUp();

    /// 선택을 아래로 이동
    void moveSelectionDown();

    /// 다음 선택 가능한 항목 인덱스 반환
    int nextSelectableItem(int from, int direction);

    std::vector<MenuItem> _items; ///< 메뉴 항목 목록
    int _selectedIndex = 0;       ///< 선택된 항목 인덱스
    bool _open = false;           ///< 메뉴 열림 여부
};

/// 메뉴바 항목
struct MenuBarEntry
{
    std::string label;          ///< 메뉴바에 표시할 라벨
    char hotkey = 0;            ///< Alt+키 단축키 (0이면 없음)
    std::shared_ptr<Menu> menu; ///< 연결된 메뉴
};

/// 화면 상단 메뉴바 위젯
class MenuBar : public Widget
{
public:
    /// 생성자
    MenuBar();

    /// 메뉴 추가 (라벨과 메뉴 지정)
    void addMenu(std::string label, std::shared_ptr<Menu> menu);

    /// 모든 항목 제거
    void clear();

    /// 메뉴바 항목 목록 반환
    std::vector<MenuBarEntry> const & entries() const;

    /// 선택된 항목 인덱스 반환
    int selectedIndex() const;

    /// 선택 인덱스 설정
    void setSelectedIndex(int idx);

    /// 메뉴바 활성화
    void activate();

    /// 메뉴바 비활성화
    void deactivate();

    /// 활성화 여부 확인
    bool isActive() const;

    /// 현재 선택된 메뉴 반환
    Menu * currentMenu();

    /// 메뉴바를 윈도우에 그리기
    void draw(Window & window) override;

    /// 키 이벤트 처리
    bool handleKey(KeyEvent const & event) override;

private:
    std::vector<MenuBarEntry> _entries; ///< 메뉴바 항목 목록
    int _selectedIndex = 0;             ///< 선택된 항목 인덱스
    bool _active = false;               ///< 활성화 여부
};
} // namespace ventty

#endif // VENTTY_WIDGET_MENU_H
