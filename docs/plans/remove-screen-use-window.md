# Screen 제거 — Widget이 Window를 직접 사용하도록 변경

## Context
Screen과 Window가 개념적으로 충돌. Screen은 위젯의 상태 기반(setCursor+setStyle) 드로잉 타겟으로 존재하지만, 상태 없는 명시적 API가 더 낫다. Screen을 제거하고 위젯이 Window의 명시적 API(`putChar(x,y,ch,style)`, `drawText(x,y,text,style)`)를 직접 사용하도록 전환.

## 1. Window에 누락 메서드 추가

**`ventty/src/ventty/screen/Window.h/.cpp`**

현재 Window에 없는 기능 중 위젯이 필요한 것:

| 추가 메서드 | 용도 |
|---|---|
| `drawBox(int x, int y, int w, int h, Style, bool doubleLine = false)` | 임의 위치 박스 (Screen의 drawBox/drawBoxDouble 대체) |
| `drawHLine(int x, int y, int len, char32_t ch, Style)` | 수평선 |
| `drawVLine(int x, int y, int len, char32_t ch, Style)` | 수직선 |
| `clear(Style)` | 스타일 전체로 클리어 (현재 `clear(Color bg)`만 있음) |
| `data() const` / `size() const` | AnsiRenderer용 버퍼 직접 접근 |

기존 `drawBox(Style, bool)` (윈도우 전체 테두리)는 유지.

## 2. Widget 인터페이스 변경

**`ventty/src/ventty/widget/Widget.h`**
```
draw(Screen & screen)  →  draw(Window & window)
```

## 3. 위젯 draw() 구현 변환 패턴

모든 위젯/뷰의 draw()를 Screen 상태 기반 → Window 명시적으로 변환:

```cpp
// Before (Screen 상태 기반)
screen.setStyle(style);
screen.setCursor(x, y);
screen.putChar(U'A');
screen.putString("hello");

// After (Window 명시적)
window.putChar(x, y, U'A', style);
window.drawText(x, y, "hello", style);
```

연속 출력(putChar 반복)은 x 좌표를 직접 증가:
```cpp
// Before
screen.setStyle(s); screen.setCursor(x, y);
for (...) screen.putChar(ch);

// After
int cx = x;
for (...) { window.putChar(cx, y, ch, s); ++cx; }
```

## 4. 변환 대상 파일 목록

**ventty 위젯 (6파일):**
- `widget/Widget.h` — 시그니처 변경
- `widget/Label.cpp` — setCursor+setStyle+putChar/putString → drawText+putChar
- `widget/ListView.cpp` — 동일 패턴
- `widget/Dialog.cpp` — drawBox + setCursor+putString
- `widget/InputDialog.cpp` — 동일
- `widget/Menu.cpp` — 동일

**mdir 뷰 (5파일):**
- `view/FunctionBar.cpp`
- `view/PathBar.cpp`
- `view/FilePanel.cpp`
- `view/StatusBar.cpp`
- `view/InfoBar.cpp`

**mdir app:**
- `app/Application.h` — `unique_ptr<Screen>` → `unique_ptr<Window>`
- `app/Application.cpp` — Screen 생성/사용 → Window, AnsiRenderer에 Window 전달

**AnsiRenderer:**
- `terminal/AnsiRenderer.h/.cpp` — `render(Screen const &)` → `render(Window const &)`
  - `screen.data()` → `window.data()`
  - `screen.width/height` → `window.width/height`

**데모 파일 (4파일):**
- `tests/demo_widget_label.cpp` — Screen 변수 → Window
- `tests/demo_widget_listview.cpp`
- `tests/demo_widget_dialog.cpp`
- `tests/demo_widget_menu.cpp`

**삭제:**
- `ventty/src/ventty/screen/Screen.h` — 제거
- `ventty/src/ventty/screen/Screen.cpp` — 제거
- `ventty/tests/test_screen.cpp` — 제거
- `ventty/src/CMakeLists.txt` — Screen 항목 제거
- `ventty/tests/CMakeLists.txt` — test_screen 제거
- `ventty/src/ventty/ventty.h` — Screen include 제거

## 5. 실행 순서

1. Window에 누락 메서드 추가 + data()/size() 추가
2. AnsiRenderer가 Window를 받도록 변경
3. Widget::draw 시그니처 변경
4. 모든 위젯 draw() 구현 변환 (ventty)
5. 모든 뷰 draw() 구현 변환 (mdir)
6. Application에서 Screen → Window 교체
7. 데모 파일 변환
8. Screen 파일 삭제, CMakeLists/ventty.h 정리
9. test_screen 삭제
10. 빌드 검증

## 6. 검증
```bash
cmake -B cmake-build-agent -S . && cmake --build cmake-build-agent
./cmake-build-agent/mdir/src/mdir
```
- 전체 빌드 성공
- mdir 실행: 파일 목록, 메뉴, 다이얼로그 정상 동작
- demo_widget_* 실행 확인
- `grep -r "Screen" ventty/src/ mdir/src/` — Screen 참조 0건
