#ifndef VENTTY_WIDGET_LISTVIEW_H
#define VENTTY_WIDGET_LISTVIEW_H

#include "Widget.h"

#include <functional>
#include <string>
#include <vector>

namespace ventty
{
/// 리스트뷰의 항목
struct ListItem
{
    std::string text;          ///< 항목 텍스트
    Style style;               ///< 항목 스타일
    void * userData = nullptr; ///< 사용자 데이터 포인터
};

/// 스크롤 가능한 항목 목록 위젯
class ListView : public Widget
{
public:
    ListView() = default;

    // Items

    /// 모든 항목 제거
    void clear();

    /// 항목 추가
    void addItem(ListItem item);

    /// 항목 목록 일괄 설정
    void setItems(std::vector<ListItem> items);

    /// 항목 목록 반환
    std::vector<ListItem> const & items() const;

    /// 항목 수 반환
    size_t itemCount() const;

    // Selection

    /// 선택된 항목 인덱스 반환
    int selectedIndex() const;

    /// 선택 인덱스 설정
    void setSelectedIndex(int index);

    /// 선택된 항목 포인터 반환 (없으면 nullptr)
    ListItem const * selectedItem() const;

    // Scrolling

    /// 스크롤 오프셋 반환
    int scrollOffset() const;

    /// 스크롤 오프셋 설정
    void setScrollOffset(int offset);

    /// 선택된 항목이 보이도록 스크롤
    void scrollToSelected();

    // Style

    /// 일반 항목 스타일 반환
    Style const & normalStyle() const;

    /// 일반 항목 스타일 설정
    void setNormalStyle(Style const & s);

    /// 선택 항목 스타일 반환
    Style const & selectedStyle() const;

    /// 선택 항목 스타일 설정
    void setSelectedStyle(Style const & s);

    // Callbacks

    /// 선택 변경 콜백 타입
    using SelectionCallback = std::function<void(int index)>;

    /// 항목 활성화(Enter) 콜백 타입
    using ActivateCallback = std::function<void(int index)>;

    /// 선택 변경 콜백 설정
    void setOnSelectionChanged(SelectionCallback cb);

    /// 항목 활성화 콜백 설정
    void setOnActivate(ActivateCallback cb);

    // Widget interface

    /// 리스트뷰를 윈도우에 그리기
    void draw(Window & window) override;

    /// 키 이벤트 처리
    bool handleKey(KeyEvent const & event) override;

protected:
    /// 크기 변경 시 호출
    void onResize() override;

private:
    /// 지정 인덱스가 보이도록 스크롤 조정
    void ensureVisible(int index);

    /// 화면에 표시 가능한 항목 수 반환
    int visibleItemCount() const;

    std::vector<ListItem> _items;                             ///< 항목 목록
    int _selectedIndex = 0;                                   ///< 현재 선택 인덱스
    int _scrollOffset = 0;                                    ///< 스크롤 오프셋
    Style _normalStyle { Colors::LIGHT_GRAY, Colors::BLACK }; ///< 일반 항목 스타일
    Style _selectedStyle { Colors::WHITE, Colors::RED };      ///< 선택 항목 스타일
    SelectionCallback _onSelectionChanged;                    ///< 선택 변경 콜백
    ActivateCallback _onActivate;                             ///< 항목 활성화 콜백
};
} // namespace ventty

#endif // VENTTY_WIDGET_LISTVIEW_H
