# Changelog

## [0.3.0-dev] - 진행중

### Added
- 파일 다중 선택 시스템
  - Space/Insert: 현재 파일 태그 토글
  - Ctrl+A: 전체 선택
  - Ctrl+D: 전체 해제
  - *: 선택 반전
- 태그된 파일 시각적 표시 (노란 배경, * 마커)

### Planned
- 파일 복사 (F5)
- 파일 이동/이름변경 (F6)
- 진행률 표시
- 듀얼 패널 지원

---

## [0.2.0] - 2026-03-25

### Added
- 메뉴 시스템
  - MenuBar 위젯
  - Menu 위젯 (드롭다운)
  - F12로 메뉴 활성화
  - Alt+키로 메뉴 단축키
- Dialog 위젯 (확인/취소)
- InputDialog 위젯 (텍스트 입력)
- 파일 삭제 기능 (F8)
  - 확인 다이얼로그
  - 파일/디렉토리 삭제
- 디렉토리 생성 기능 (F7)
  - 이름 입력 다이얼로그
  - 에러 처리

### Changed
- 레이아웃에 메뉴 바 추가 (상단)

---

## [0.1.0] - 2026-03-25

### Added
- vtty 레이어 (자체 터미널 라이브러리)
  - Terminal: termios raw mode 제어
  - Screen: 더블 버퍼링, ANSI escape sequence 출력
  - Input: 키보드 입력 파싱 (일반키, 기능키, 조합키)
  - Color: True Color (24-bit) 지원
- 파일 시스템 레이어
  - FileSystem: 디렉토리 읽기, 정렬
  - FileInfo: 파일 정보, 타입 감지
- 위젯 레이어
  - Widget: 기본 위젯 클래스
  - Label: 텍스트 표시
  - ListView: 스크롤 가능한 목록
- 뷰 레이어
  - FunctionBar: F1-F10 기능키 바
  - PathBar: 경로 및 볼륨 표시
  - FilePanel: 파일 목록 패널
  - StatusBar: 파일/디렉토리 수, 용량 표시
  - InfoBar: 현재 파일, 날짜/시간 표시
- 기본 기능
  - MDIR 스타일 UI (색상, 테두리)
  - 파일 타입별 색상 구분
  - 키보드 네비게이션
  - 디렉토리 진입/이동
  - 파일 실행
  - 숨김 파일 토글 (Ctrl+H)
  - 새로고침 (F5)
  - 종료 (q, Alt+X)
- UTF-8 한글 파일명 지원
- 터미널 크기 변경 대응 (SIGWINCH)
