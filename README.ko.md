# ventty

![ventty](docs/ventty.png)

![build](https://github.com/0xf07ce/ventty/actions/workflows/build.yml/badge.svg)
![version](https://img.shields.io/badge/version-0.1.0-blue)
![C++20](https://img.shields.io/badge/C%2B%2B-20-brightgreen)
![license](https://img.shields.io/badge/license-MIT-green)

사람과 AI 에이전트 모두를 위한 현대적 C++20 터미널 UI 라이브러리.

[English](README.md)

---

## 개요

**ventty**는 TUI(Text User Interface) 및 그래픽 터미널 애플리케이션을 만들기 위한 셀 기반 터미널 추상화 라이브러리입니다. diff 기반 렌더링, 겹침 윈도우 관리, 유니코드/UTF-8 지원, 24비트 트루컬러, 위젯 툴킷을 제공합니다. ANSI/VT100 터미널 백엔드와 SDL3 그래픽 백엔드를 지원합니다.

## 왜 ventty인가?

**ventty는 AI 에이전트를 위한 하네스(harness)로 설계되었습니다.** 핵심 아이디어는 하네스 엔지니어링 — 잘 구조화된 타입 안전 API를 제공하여 AI 코드 생성이 설계 단계에서부터 올바르고 안전한 결과를 내도록 유도하는 것입니다.

AI 에이전트가 ncurses를 직접 사용하면 수십 년 된 C API의 암묵적 전역 상태, 매크로 기반 설정, 플랫폼별 특이사항을 다뤄야 합니다. 그 결과물은 깨지기 쉽고 오류가 잦아 수동 교정이 필요합니다. ventty는 이런 복잡성을 현대적 C++20 인터페이스 뒤에 감추어, 타입 시스템과 API 설계가 실수를 원천적으로 방지합니다.

**ncurses와의 비교:**

| | ncurses | ventty |
|---|---|---|
| **언어** | 전역 상태를 가진 C API | C++20, 전역 상태 없음 |
| **렌더링** | 명령형 커서 이동 | 셀 기반 자동 diff |
| **백엔드** | 터미널 전용 | 터미널 + SDL3 그래픽 윈도우 |
| **유니코드** | 부분적, 설정에 의존 | UTF-8 내장, 한국어 기본 지원 |
| **위젯** | 없음 (별도 라이브러리 필요) | Dialog, Menu, ListView, Label 내장 |
| **AI 친화성** | 낮음 — 암묵적 상태, 매크로, 함정 | 높음 — 타입 기반 API, RAII, 암묵적 상태 없음 |

## 기능

- **셀 기반 렌더링** — 터미널을 2D 스타일 셀 그리드로 모델링하여 모든 문자를 정밀하게 제어
- **Diff 기반 업데이트** — 변경된 셀만 다시 렌더링하여 I/O를 최소화
- **윈도우 관리** — Z-순서 겹침, 스크롤백 버퍼, 가시성 제어
- **유니코드 지원** — UTF-8 인코딩/디코딩, 한글(Hangul) 기본 지원
- **24비트 트루컬러** — RGB, xterm256, CGA 16색 팔레트 및 색상 보간
- **입력 처리** — 키보드(수정자 키 포함), 마우스 이벤트, 터미널 크기 변경 감지
- **위젯 툴킷** — Label, ListView, Dialog, InputDialog, Menu/MenuBar
- **ASCII 아트 툴킷** — 박스 그리기, 스피너, 프로그레스 바, 막대 그래프, 점자 플롯
- **듀얼 백엔드** — ANSI 터미널 (`ventty::Terminal`) 및 SDL3 그래픽 윈도우 (`ventty::GfxTerminal`)

## 요구사항

- CMake >= 3.20
- C++20 컴파일러
- POSIX 호환 시스템 (Linux, macOS, BSD)
- SDL3, SDL3_image (그래픽 백엔드용, 선택사항 — 미설치 시 자동 획득)

## 빌드

### Docker 사용 (Linux 권장)

빌드 의존성이 포함된 Docker 이미지를 제공합니다:

```bash
docker build -t ventty-build -f docker/Dockerfile .
docker run --rm -v $(pwd):/workspace ventty-build bash -c "cmake -S . -B build && cmake --build build"
```

### 네이티브 빌드 (macOS)

```bash
cmake -S . -B build
cmake --build build
```

옵션:

| 옵션 | 기본값 | 설명 |
|---|---|---|
| `VENTTY_BUILD_GFX` | `ON` | SDL3 그래픽 백엔드 빌드 |
| `VENTTY_BUILD_TESTS` | `ON` | 단위 테스트 빌드 |
| `VENTTY_BUILD_EXAMPLES` | `ON` | 예제 프로그램 빌드 |

## 문서

- [빠른 시작](docs/quick-start.md)
- [프로젝트 구조 & 아키텍처](docs/project-structure.md)

## 라이선스

[LICENSE](LICENSE) 파일을 참고하세요.
