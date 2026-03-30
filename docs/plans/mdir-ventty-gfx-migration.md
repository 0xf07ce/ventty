# mdir: ANSI → ventty_gfx(SDL3) 마이그레이션

## Context
mdir은 현재 저수준 ANSI 컴포넌트 4개(TerminalControl, Screen, AnsiRenderer, Input)를 개별 조합해서 사용 중. 이를 ventty_gfx의 GfxTerminal(SDL3 기반)로 전환하여 GUI 창 기반 파일 관리자로 변환.

## 핵심 변경점

### 1. Application 멤버 교체
**`mdir/src/app/Application.h`**
```
제거:
  unique_ptr<TerminalControl> _terminal
  unique_ptr<Screen> _screen
  unique_ptr<AnsiRenderer> _ansiRenderer
  unique_ptr<Input> _input

추가:
  unique_ptr<gfx::GfxTerminal> _terminal
  unique_ptr<Screen> _screen  (위젯 드로잉 버퍼로 유지)

handleInput() 시그니처: void handleInput() → void handleInput(KeyEvent const & event)
```

### 2. init() 변경
**`mdir/src/app/Application.cpp`**
```
TerminalControl 초기화 제거 → GfxTerminal::init(cols, rows, title, fontPath, fontSize)
Input 생성 제거 → onKey 콜백 등록
AnsiRenderer 생성 제거
resize 핸들러: TerminalControl::setResizeHandler → terminal->onResize
```

### 3. 메인 루프 변경
```
기존: while(_running) { updateUI(); draw(); handleInput(); }
                                              ↑ blocking Input::read(100)

변경: while(_running && _terminal->isRunning()) {
          while (_terminal->pollEvent());  // 이벤트 펌프 (onKey 콜백 호출)
          updateUI();
          draw();
          _terminal->render();
          sleep_for(16ms);
      }

onKey 콜백에서 handleInput(event) 직접 호출
```

### 4. draw() 변경
```
기존 마지막줄: _ansiRenderer->render(*_screen);
변경:         blitScreenToTerminal(*_screen, *_terminal);
              // Screen 셀 → Terminal 셀 복사 (demo_widget_*.cpp 패턴)
```

`blitScreenToTerminal` — Screen→Terminal 복사 유틸 (인라인, Application 내부 private 메서드로 구현)

### 5. 다이얼로그 루프 변경
```
기존: auto event = _input->read(-1);  // blocking

변경: onKey 콜백을 임시 교체 → dialog에 키 전달
      while (result == None && _running) {
          while (_terminal->pollEvent());
          draw(); dialog.draw(*_screen);
          blit + _terminal->render();
          sleep_for(16ms);
      }
      onKey 콜백 복원
```

### 6. executeFile() 비활성화
SDL3 윈도우에서 fork/exec + 터미널 모드 전환 불가. showDialog로 "GUI 모드에서는 미지원" 표시.

### 7. cleanup() 단순화
```
기존: showCursor + disableAlternateScreen + exitRawMode
변경: _terminal->shutdown()
```

### 8. CMake 변경
**`mdir/src/CMakeLists.txt`**
- `view` 라이브러리: ventty → ventty_gfx 링크 (ventty는 transitively 포함)
- 폰트 에셋 복사: `ventty_gfx/assets/` → 빌드 출력 디렉토리

## 수정 파일 목록
| 파일 | 변경 |
|------|------|
| `mdir/src/app/Application.h` | 멤버 교체, include 변경, handleInput 시그니처 |
| `mdir/src/app/Application.cpp` | init, run, draw, cleanup, handleInput, showDialog, showInputDialog, executeFile 전면 수정 |
| `mdir/src/CMakeLists.txt` | ventty_gfx 링크, 폰트 복사 |

## 검증
```bash
cmake -B cmake-build-agent -S . && cmake --build cmake-build-agent
./cmake-build-agent/mdir/src/mdir
```
- SDL3 창이 뜨고 파일 목록 표시되는지 확인
- 방향키 탐색, Enter 진입, Backspace 상위 이동
- F12 메뉴바, F7 디렉토리 생성 다이얼로그, F8 삭제 다이얼로그
- q 종료
