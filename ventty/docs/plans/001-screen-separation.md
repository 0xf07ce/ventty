# Screen 분리 및 AnsiRenderer 추출

## 배경

현재 `Screen` 클래스가 두 가지 역할을 동시에 수행한다:
1. **셀 버퍼 + 커서 상태 관리** — `setCursor`, `setStyle`, `putChar`, `drawBox` 등
2. **ANSI 터미널 출력** — `flush()`, `appendAnsiColor()`, `_backBuffer` diff 렌더링

이로 인해 `Screen`을 사용하는 위젯(Label, ListView, Dialog, InputDialog, Menu)이
ANSI 터미널에 종속되어 `ventty_gfx` (SDL3 백엔드)에서 재사용할 수 없다.

## 목표

Screen을 순수 버퍼로 분리하고, ANSI 출력을 별도 렌더러로 추출하여
위젯이 백엔드에 무관하게 동작하도록 한다.

## 변경 후 구조

```
Screen (순수 버퍼 + 커서 + 스타일 + drawBox/fillRect)
  │
  ├── ventty::AnsiRenderer  — Screen → ANSI escape → stdout
  └── ventty::gfx::Renderer — Screen → SDL3 텍스처 → 윈도우 (기존 Window 기반 렌더링 활용)
```

## 상세 변경 사항

### 1. Screen에서 ANSI 관련 코드 제거

**파일**: `ventty/src/ventty/Screen.h`, `Screen.cpp`

제거할 멤버:
- `flush()`
- `appendAnsiColor()`
- `invalidate()` (backBuffer 관련)
- `_backBuffer`
- `_dirty`
- `#include <unistd.h>`

Screen에 남는 것:
- `_buffer`, `_width`, `_height`
- `_cursorX`, `_cursorY`, `_currentStyle`
- `resize()`, `clear()`, `setCursor()`, `setStyle()`
- `putChar()`, `putString()`, `putStringAt()`
- `drawBox()`, `drawBoxDouble()`, `drawHLine()`, `drawVLine()`
- `fillRect()`
- `at()`
- box drawing 상수들

추가할 메서드:
- `Cell const * data() const` — 렌더러가 버퍼에 직접 접근할 수 있도록
- `int size() const` — width * height

### 2. AnsiRenderer 신규 생성

**파일**: `ventty/src/ventty/AnsiRenderer.h`, `AnsiRenderer.cpp`

Screen에서 추출한 ANSI 출력 로직을 담는다.

```cpp
namespace ventty
{

class AnsiRenderer
{
public:
    AnsiRenderer();
    ~AnsiRenderer();

    /// Screen 내용을 diff 기반으로 ANSI 터미널에 출력
    void render(Screen const & screen);

    /// 다음 render()에서 전체 다시 그리기
    void invalidate();

private:
    void appendAnsiColor(std::string & buf, Style const & style, Style const & prev);

    std::vector<Cell> _prev;   // 이전 프레임 (diff용)
    int _prevWidth = 0;
    int _prevHeight = 0;
    bool _fullRedraw = true;
};

} // namespace ventty
```

### 3. ventty::Terminal 수정

**파일**: `ventty/src/ventty/Terminal.h`, `Terminal.cpp`

현재 Terminal은 자체적으로 `_root`, `_screen`, `_prev` 셀 벡터와 `composite()`, `emitDiff()` 등을 갖고 있다.
이미 Screen과 별개로 자체 렌더링 파이프라인이 있으므로, AnsiRenderer 추출과는 **독립적**으로 동작한다.

Terminal에 추가할 것:
- `Screen & screen()` — 위젯이 Terminal의 Screen에 접근하여 그릴 수 있도록

> **참고**: Terminal의 Window 기반 composite + emitDiff 파이프라인은 당장 건드리지 않는다.
> Screen은 위젯 레이어가 사용하고, Terminal의 기존 Window 시스템은 그대로 유지한다.
> 두 레이어를 합성하는 것은 후속 작업으로 남긴다.

### 4. ventty_gfx::Terminal 수정

**파일**: `ventty_gfx/src/ventty_gfx/Terminal.h`, `Terminal.cpp`

추가할 멤버:
- `ventty::Screen _screen` — 위젯이 그릴 수 있는 Screen 인스턴스

추가할 메서드:
- `Screen & screen()` — ventty::Terminal과 동일한 인터페이스

render() 흐름 변경:
1. 기존: Window들을 직접 gfx::Renderer로 렌더링
2. 변경: Screen의 셀 버퍼도 gfx::Renderer를 통해 렌더링
   - `Screen::data()`로 셀 배열을 읽어 각 셀을 `Renderer::renderCell()`로 그린다
   - 기존 Window 렌더링과 합성 순서를 결정한다 (Screen이 배경, Window가 위)

### 5. CMakeLists.txt 수정

**파일**: `ventty/src/CMakeLists.txt`

소스 목록에 `ventty/AnsiRenderer.cpp` 추가.

## 변경하지 않는 것

- 위젯 코드 (`mdir/src/widget/`) — `draw(Screen &)` 시그니처 그대로 유지
- `ventty::Window` — 기존 구조 유지
- `ventty_gfx::Renderer` — 기존 SDL3 렌더링 로직 유지, Screen 렌더링 메서드만 추가

## 검증 방법

1. `cmake -B cmake-build-agent -S . && cmake --build cmake-build-agent` 빌드 성공 확인
2. `ventty_demo_terminal` 실행하여 ANSI 터미널 렌더링이 기존과 동일하게 동작하는지 확인
3. mdir 빌드 및 실행하여 위젯(Dialog, Menu 등)이 정상 동작하는지 확인
