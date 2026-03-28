# KGM 개발 로드맵

> KGM: K-era Graphical Mdir (또는 Korean-style Graphical Manager)

## 프로젝트 개요

MDIR의 외형과 사용성을 현대 Linux/Unix 환경에서 재현하는 터미널 기반 파일 관리자.

### 핵심 설계 원칙

1. **외부 의존성 최소화**: ncurses 대신 자체 vtty 레이어 구현
2. **MDIR 호환 UI**: 원본 MDIR의 외형을 최대한 재현
3. **현대적 기능**: UTF-8, True Color, 마우스 지원
4. **모듈화**: vtty → widget → application 계층 분리

---

## 버전 로드맵

### v0.1 - 기본 프레임워크 (MVP)

**목표**: 파일 목록 출력, 디렉토리 이동, 파일 실행

#### 마일스톤 0.1.0: vtty 코어

- [ ] Terminal raw mode 진입/복원
- [ ] ANSI escape sequence 출력
- [ ] 키보드 입력 파싱 (일반키, 기능키, 조합키)
- [ ] 화면 버퍼 관리 (더블 버퍼링)
- [ ] True Color 지원 (24-bit)
- [ ] UTF-8 문자 출력 (wide character 처리)

#### 마일스톤 0.1.1: 기본 위젯

- [ ] Box (테두리 그리기)
- [ ] Label (텍스트 출력)
- [ ] ListView (스크롤 가능한 목록)

#### 마일스톤 0.1.2: 파일 시스템

- [ ] 디렉토리 읽기
- [ ] 파일 정보 (크기, 날짜, 권한)
- [ ] 정렬 (이름, 크기, 날짜)

#### 마일스톤 0.1.3: 메인 화면

- [ ] 기능키 바
- [ ] 경로/볼륨 바
- [ ] 파일 목록 (색상 구분)
- [ ] 상태 바
- [ ] 키보드 네비게이션

#### 마일스톤 0.1.4: 파일 실행

- [ ] 실행 파일 탐지 및 실행
- [ ] 터미널 복원 후 실행, 재진입

---

### v0.2 - 메뉴 및 다이얼로그

**목표**: 메뉴 시스템, MCD, 기본 파일 조작

- [ ] 드롭다운 메뉴 바
- [ ] 메뉴 항목 및 단축키
- [ ] MCD (디렉토리 트리 다이얼로그)
- [ ] 확인/취소 다이얼로그
- [ ] 파일 삭제 (F8)
- [ ] 디렉토리 생성 (F7)

---

### v0.3 - 파일 조작

**목표**: 복사, 이동, 이름변경

- [ ] 파일 선택 (Space, Ins)
- [ ] 다중 선택
- [ ] 복사 (F5)
- [ ] 이동/이름변경 (F6)
- [ ] 진행률 표시

---

### v0.4 - 듀얼 패널

**목표**: 선택적 듀얼 패널 모드

- [ ] 패널 분할 (Tab 전환)
- [ ] 패널 간 복사/이동
- [ ] 패널 동기화 옵션

---

### v0.5 - 파일 뷰어/편집기

**목표**: 내장 뷰어

- [ ] 텍스트 파일 뷰어 (F3)
- [ ] 헥스 뷰어
- [ ] 외부 편집기 연동 (F4)

---

### v1.0 - 정식 릴리스

**목표**: 안정화 및 완성도

- [ ] 설정 파일 (~/.kgmrc)
- [ ] 색상 테마 커스터마이징
- [ ] 북마크/즐겨찾기
- [ ] 검색 기능
- [ ] 마우스 지원

---

## 아키텍처

```
┌─────────────────────────────────────────────────────────────┐
│                      Application                             │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐            │
│  │ MainView│ │  Menu   │ │   MCD   │ │ Dialog  │            │
│  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘            │
│       │           │           │           │                  │
│  ┌────┴───────────┴───────────┴───────────┴────┐            │
│  │              Widget Layer                    │            │
│  │  ┌─────┐ ┌───────┐ ┌────────┐ ┌──────────┐  │            │
│  │  │ Box │ │ Label │ │ListView│ │ InputBox │  │            │
│  │  └─────┘ └───────┘ └────────┘ └──────────┘  │            │
│  └──────────────────────┬───────────────────────┘            │
│                         │                                    │
│  ┌──────────────────────┴───────────────────────┐            │
│  │                vtty Layer                     │            │
│  │  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ │            │
│  │  │Terminal│ │ Screen │ │ Input  │ │ Color  │ │            │
│  │  │ Mode   │ │ Buffer │ │ Parser │ │ Manager│ │            │
│  │  └────────┘ └────────┘ └────────┘ └────────┘ │            │
│  └──────────────────────┬───────────────────────┘            │
│                         │                                    │
└─────────────────────────┼────────────────────────────────────┘
                          │
                   ┌──────┴──────┐
                   │   termios   │
                   │ ANSI/VT100  │
                   └─────────────┘
```

---

## 디렉토리 구조

```
src/
├── main.cpp                 # 진입점
├── vtty/                    # 가상 터미널 레이어
│   ├── Terminal.h/cpp       # termios 제어
│   ├── Screen.h/cpp         # 화면 버퍼
│   ├── Input.h/cpp          # 키 입력 파싱
│   ├── Color.h/cpp          # 색상 관리
│   └── Utf8.h/cpp           # UTF-8 처리
├── widget/                  # UI 위젯
│   ├── Widget.h/cpp         # 기본 위젯 클래스
│   ├── Box.h/cpp            # 테두리
│   ├── Label.h/cpp          # 텍스트
│   ├── ListView.h/cpp       # 목록
│   ├── Menu.h/cpp           # 메뉴
│   └── Dialog.h/cpp         # 다이얼로그
├── fs/                      # 파일 시스템
│   ├── FileSystem.h/cpp     # 파일 조작
│   ├── FileInfo.h/cpp       # 파일 정보
│   └── DirTree.h/cpp        # 디렉토리 트리
├── view/                    # 화면 구성
│   ├── MainView.h/cpp       # 메인 화면
│   ├── FilePanel.h/cpp      # 파일 패널
│   ├── StatusBar.h/cpp      # 상태바
│   └── FunctionBar.h/cpp    # 기능키 바
└── app/                     # 애플리케이션
    ├── Application.h/cpp    # 앱 코어
    ├── Config.h/cpp         # 설정
    └── Theme.h/cpp          # 테마
```

---

## 기술 명세

### vtty 레이어

#### Terminal Mode

```cpp
class Terminal
{
public:
    void enterRawMode();     // termios 설정
    void exitRawMode();      // 원래 설정 복원
    void enableMouse();      // 마우스 이벤트 활성화
    void disableMouse();

private:
    struct termios _originalTermios;
};
```

#### Screen Buffer

```cpp
struct Cell
{
    char32_t ch;             // Unicode 코드포인트
    Color fg;                // 전경색 (24-bit)
    Color bg;                // 배경색 (24-bit)
    uint8_t attr;            // Bold, Underline 등
};

class Screen
{
public:
    void resize(int width, int height);
    void clear();
    void setCursor(int x, int y);
    void putChar(char32_t ch);
    void putString(std::string_view str);
    void setColor(Color fg, Color bg);
    void flush();            // 터미널에 출력

private:
    std::vector<Cell> _buffer;
    std::vector<Cell> _backBuffer;
    int _width, _height;
    int _cursorX, _cursorY;
};
```

#### Input Parser

```cpp
enum class Key
{
    None,
    Char,        // 일반 문자
    Enter, Tab, Escape, Backspace,
    Up, Down, Left, Right,
    Home, End, PageUp, PageDown,
    Insert, Delete,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
};

struct KeyEvent
{
    Key key;
    char32_t ch;     // Key::Char인 경우
    bool ctrl;
    bool alt;
    bool shift;
};

class Input
{
public:
    KeyEvent read();         // blocking read
    bool poll(int timeoutMs);

private:
    KeyEvent parseEscapeSequence();
};
```

### 색상 시스템

```cpp
struct Color
{
    uint8_t r, g, b;

    static Color const BLACK;
    static Color const RED;
    static Color const GREEN;
    static Color const YELLOW;
    static Color const BLUE;
    static Color const MAGENTA;
    static Color const CYAN;
    static Color const WHITE;

    // DOS 16색 팔레트
    static Color fromDosIndex(int index);
};
```

### ANSI Escape Sequences

```cpp
// 커서 이동
"\033[{row};{col}H"          // 절대 위치

// 색상 (True Color)
"\033[38;2;{r};{g};{b}m"     // 전경색
"\033[48;2;{r};{g};{b}m"     // 배경색

// 속성
"\033[0m"                     // 리셋
"\033[1m"                     // Bold
"\033[4m"                     // Underline
"\033[7m"                     // Reverse

// 화면
"\033[2J"                     // 화면 지우기
"\033[?25l"                   // 커서 숨기기
"\033[?25h"                   // 커서 보이기
"\033[?1049h"                 // 대체 화면 버퍼 사용
"\033[?1049l"                 // 메인 화면 버퍼 복원
```

---

## 키 바인딩 (v0.1)

| 키 | 동작 |
|----|------|
| ↑/↓ | 파일 선택 이동 |
| ←/→ | 열 이동 (멀티컬럼 모드) |
| Enter | 디렉토리 진입 / 파일 실행 |
| Backspace | 상위 디렉토리 |
| Home | 첫 항목 |
| End | 마지막 항목 |
| PgUp/PgDn | 페이지 이동 |
| q / Alt-X | 종료 |

---

## 빌드 및 테스트

```bash
# 빌드
cmake -B build -S .
cmake --build build

# 실행
./build/kgm

# 테스트 (향후)
ctest --test-dir build
```

---

## 참고

- [ANSI Escape Codes](https://en.wikipedia.org/wiki/ANSI_escape_code)
- [termios(3)](https://man7.org/linux/man-pages/man3/termios.3.html)
- [XTerm Control Sequences](https://invisible-island.net/xterm/ctlseqs/ctlseqs.html)
