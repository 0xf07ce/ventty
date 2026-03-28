# MDIR 구현 정보

## 1. 개요

MDIR은 1990년대 한국에서 가장 널리 사용된 MS-DOS 셸/파일 관리자 프로그램이다.
개발자 최정한이 컴퓨터 사용이 어려운 여자친구를 위해 만들었다는 일화가 유명하다.

### 버전 역사

| 버전 | 설명 |
|------|------|
| MDIR II | 8086/8088 이상 지원 |
| MDIR III | 80286 이상 필요, 최종 버전 3.10 (1998) |
| WinM | Windows 버전, 4.5.591 베타가 마지막 |

### 개발 환경

- **언어**: Turbo Pascal
- **플랫폼**: MS-DOS
- **저작권**: 현재 한빛인포텍 소유

## 2. 기술 구현

### 2.1 화면 출력

DOS 환경에서 MDIR은 다음 방식으로 화면을 제어했다:

```
[INT 10h BIOS Video Services]
- AH=00h: Set video mode (80x25 text mode)
- AH=02h: Set cursor position
- AH=09h: Write character with attribute
- AH=0Eh: Write character in TTY mode

[직접 비디오 메모리 접근]
- 세그먼트 0xB800 (CGA/EGA/VGA 텍스트 모드)
- 각 문자 = 2바이트 (문자 + 속성)
- 속성 바이트: [Blink:1][BG:3][FG:4]
```

### 2.2 파일 시스템 접근

```
[INT 21h DOS Services]
- AH=1Ah: Set DTA (Disk Transfer Address)
- AH=4Eh: Find first file
- AH=4Fh: Find next file
- AH=3Bh: Change current directory
- AH=47h: Get current directory
```

### 2.3 키보드 입력

```
[INT 16h BIOS Keyboard Services]
- AH=00h: Read character
- AH=01h: Check for keystroke
- AH=02h: Get shift flags
```

## 3. UI 구조

### 3.1 화면 레이아웃 (80x25)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ F1xxx │ F2xxx │ F3xxx │ F4xxx │ F5xxx │ F6xxx │ F7xxx │ F8xxx │ F9xxx │     │ 행 0: 기능키
├─────────────────────────────────────────────────────────────────────────────┤
│ Path C:\*.*                                           Volume [C_DRIVE]     │ 행 1: 경로/볼륨
├─────────────────────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────────────────────┐ │ 행 2: 테두리
│ │ ..          [ Up-Dir ] 26-01-28 10:07p                                 ▲│ │
│ │ GAME        [ SubDir ] 25-12-18  4:19p                                  │ │
│ │ LANG        [ SubDir ] 25-12-18  4:19p                                  │ │
│ │ ...                                                                     │ │ 행 3-19: 파일목록
│ │                                                                        ▼│ │
│ └─────────────────────────────────────────────────────────────────────────┘ │ 행 20: 테두리
│ [-A-] Remove                                                                │ 행 21: 드라이브
│ [-C-] HDD C_DRIVE                                                           │ 행 22: 드라이브
├─────────────────────────────────────────────────────────────────────────────┤
│  6 File    6 Dir         39,350 Byte    262,144,000(48%)byte free (511M) Q │ 행 23: 상태
│ Directory│26-01-28│22:07│....│26-03-25│WED│10:19:40pm│F10=MCD│F11=QCD│F12=M│ 행 24: 하단바
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 파일 목록 형식

```
[파일명   ][확장자] [    크기] [날짜    ] [시간 ]
FILENAME   EXT       12,345  25-12-18    4:19p

[디렉토리명     ] [ SubDir ] [날짜    ] [시간 ]
DIRNAME          [ SubDir ]  25-12-18    4:19p
```

### 3.3 색상 체계 (16색 DOS 팔레트)

| 인덱스 | 색상 | 용도 |
|--------|------|------|
| 0x00 | Black | 배경 |
| 0x01 | Blue | - |
| 0x02 | Green | EXE 파일 |
| 0x03 | Cyan | 테두리, 상태바 배경 |
| 0x04 | Red | 선택 항목 배경 |
| 0x05 | Magenta | LZH/ZIP 압축파일 |
| 0x06 | Brown | - |
| 0x07 | Light Gray | 일반 파일 |
| 0x08 | Dark Gray | - |
| 0x09 | Light Blue | COM 파일 |
| 0x0A | Light Green | - |
| 0x0B | Light Cyan | - |
| 0x0C | Light Red | - |
| 0x0D | Light Magenta | - |
| 0x0E | Yellow | 디렉토리, BAT 파일 |
| 0x0F | White | 선택된 항목, Up-Dir |

## 4. 주요 기능

### 4.1 기능키 매핑

| 키 | 기능 |
|----|------|
| F1 | 도움말 |
| F2 | 사용자 정의 프로그램 |
| F3 | 파일 보기 |
| F4 | 파일 편집 |
| F5 | 복사 |
| F6 | 이동/이름변경 |
| F7 | 디렉토리 생성 |
| F8 | 삭제 |
| F9 | 사용자 정의 프로그램 |
| F10 | MCD (디렉토리 변경) |
| F11 | QCD (빠른 디렉토리) |
| F12 | 메뉴 |
| Enter | 실행/디렉토리 진입 |
| Tab | 패널 전환 (듀얼 패널 모드) |
| Alt-X | 종료 |

### 4.2 MCD (MDIR Change Directory)

```
┌─ MdirIII Change Directory ─────────────────────┐
│                                                │
│ \                                              │
│ ├─ MDIR ─── BABO ─── LOVE ─── M...             │
│ ├─ GOOD ─── UTIL ─── OUCH ─── M...             │
│ └─ HELP ─── GAME                               │
│                                                │
│ C:\MDIR                              [MDIR   ] │
├────────────────────────────────────────────────┤
│ F1=Help │ F2=Rescan │ F3=Drive │ F4=Move       │
└────────────────────────────────────────────────┘
```

### 4.3 메뉴 시스템

```
┌─ Mdir ─┬─ Run ─┬─ File ─┬─ Direct ─┬─ Archive ─┬─ Option ─┐
│ About  │       │        │          │           │          │
│ Help   │ F1    │        │          │           │          │
│ View.. │       │        │          │           │          │
├────────│       │        │          │           │          │
│ Parking│Ctrl-P │        │          │           │          │
│ Volume │Ctrl-V │        │          │           │          │
│ ...    │       │        │          │           │          │
├────────│       │        │          │           │          │
│ Quit   │Alt-X  │        │          │           │          │
└────────┴───────┴────────┴──────────┴───────────┴──────────┘
```

## 5. 유사 프로젝트 (오픈소스)

### 5.1 LinM (Linux Mdir)

- **URL**: https://github.com/la9527/linm
- **언어**: C++ (ncurses 사용)
- **특징**: MDIR 직접 클론, FTP/SFTP 지원

### 5.2 Midnight Commander

- **URL**: https://github.com/MidnightCommander/mc
- **언어**: C (S-Lang 또는 ncurses)
- **특징**: Norton Commander 클론, VFS 지원

### 5.3 mdir.js

- **URL**: https://github.com/la9527/mdir.js
- **언어**: TypeScript/Node.js
- **특징**: 듀얼 패널, SSH/SFTP 지원

## 6. 참고 자료

- [MDIR - 나무위키](https://namu.wiki/w/Mdir)
- [MDIR - 위키백과](https://ko.wikipedia.org/wiki/MDIR)
- [Code Page 437](https://en.wikipedia.org/wiki/Code_page_437)
- [VGA Text Mode](https://en.wikipedia.org/wiki/VGA_text_mode)
- [INT 10H](https://en.wikipedia.org/wiki/INT_10H)
