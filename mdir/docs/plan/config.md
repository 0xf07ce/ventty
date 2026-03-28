# 설정 시스템

## 설계 원칙

- **VSCode/Sublime 스타일**: JSON 파일을 외부 에디터로 직접 편집
- **기본값 내장**: 설정 파일이 없어도 동작
- **검증 및 오류 처리**: 잘못된 설정은 무시하고 기본값 사용

---

## 설정 파일 위치

```
우선순위 (높은 순):
1. ./kgm.json                    # 현재 디렉토리 (프로젝트별)
2. ~/.config/kgm/config.json     # XDG 표준
3. ~/.kgm/config.json            # 레거시 호환
4. (내장 기본값)
```

---

## 설정 파일 구조

```json
{
    "panel": {
        "mode": "single",
        "showHidden": false,
        "sortBy": "name",
        "sortReverse": false,
        "dirsFirst": true
    },

    "display": {
        "dateFormat": "YY-MM-DD",
        "timeFormat": "12h",
        "sizeFormat": "auto"
    },

    "colors": {
        "background": "#000000",
        "foreground": "#AAAAAA",
        "border": "#00AAAA",
        "selection": "#AA0000",
        "statusBar": {
            "background": "#00AAAA",
            "foreground": "#000000"
        },
        "files": {
            "directory": "#FFFF55",
            "executable": "#00AA00",
            "symlink": "#55FFFF",
            "archive": "#AA00AA",
            "hidden": "#555555",
            "default": "#AAAAAA"
        }
    },

    "keys": {
        "quit": ["q", "Alt-x"],
        "execute": ["Enter"],
        "parentDir": ["Backspace", "Left"],
        "enterDir": ["Enter", "Right"],
        "selectUp": ["Up", "k"],
        "selectDown": ["Down", "j"],
        "pageUp": ["PageUp"],
        "pageDown": ["PageDown"],
        "home": ["Home", "g"],
        "end": ["End", "G"],
        "toggleHidden": ["Ctrl-h", "."],
        "refresh": ["Ctrl-r", "F2"]
    },

    "extensions": {
        "executable": [".sh", ".bash", ".py", ".pl"],
        "archive": [".tar", ".gz", ".bz2", ".xz", ".zip", ".rar", ".7z"],
        "image": [".png", ".jpg", ".jpeg", ".gif", ".bmp"],
        "document": [".txt", ".md", ".pdf", ".doc", ".docx"]
    },

    "external": {
        "editor": "$EDITOR",
        "pager": "$PAGER",
        "shell": "$SHELL"
    }
}
```

---

## 설정 항목 설명

### panel

| 키 | 타입 | 기본값 | 설명 |
|----|------|--------|------|
| mode | string | "single" | "single" 또는 "dual" |
| showHidden | bool | false | 숨김 파일 표시 |
| sortBy | string | "name" | "name", "size", "date", "ext" |
| sortReverse | bool | false | 역순 정렬 |
| dirsFirst | bool | true | 디렉토리 먼저 표시 |

### display

| 키 | 타입 | 기본값 | 설명 |
|----|------|--------|------|
| dateFormat | string | "YY-MM-DD" | 날짜 형식 |
| timeFormat | string | "12h" | "12h" 또는 "24h" |
| sizeFormat | string | "auto" | "auto", "bytes", "human" |

### colors

- 모든 색상은 `#RRGGBB` 형식 (True Color)
- 또는 DOS 색상 이름: `"black"`, `"red"`, `"green"`, `"yellow"`, `"blue"`, `"magenta"`, `"cyan"`, `"white"`, `"gray"`, `"bright-red"` 등

### keys

- 배열로 여러 키 바인딩 가능
- 형식: `"키"`, `"Ctrl-키"`, `"Alt-키"`, `"Shift-키"`
- 특수키: `"Enter"`, `"Escape"`, `"Tab"`, `"Backspace"`, `"Space"`
- 기능키: `"F1"` ~ `"F12"`
- 방향키: `"Up"`, `"Down"`, `"Left"`, `"Right"`
- 기타: `"Home"`, `"End"`, `"PageUp"`, `"PageDown"`, `"Insert"`, `"Delete"`

---

## JSON 파서 구현

외부 라이브러리 없이 간단한 JSON 파서 직접 구현:

```cpp
// src/config/Json.h
namespace json
{

enum class Type { Null, Bool, Number, String, Array, Object };

class Value
{
public:
    Type type() const;

    bool asBool() const;
    double asNumber() const;
    std::string const & asString() const;
    std::vector<Value> const & asArray() const;
    std::map<std::string, Value> const & asObject() const;

    Value const & operator[](std::string const & key) const;
    Value const & operator[](size_t index) const;

    bool has(std::string const & key) const;

    template<typename T>
    T get(std::string const & key, T defaultValue) const;
};

Value parse(std::string_view json);
std::string stringify(Value const & value, bool pretty = false);

}
```

---

## Config 클래스

```cpp
// src/config/Config.h
class Config
{
public:
    static Config & instance();

    void load();                              // 설정 파일 로드
    void loadFrom(std::filesystem::path const & path);

    // Panel
    std::string panelMode() const;            // "single" or "dual"
    bool showHidden() const;
    std::string sortBy() const;
    bool sortReverse() const;
    bool dirsFirst() const;

    // Display
    std::string dateFormat() const;
    std::string timeFormat() const;
    std::string sizeFormat() const;

    // Colors
    Color backgroundColor() const;
    Color foregroundColor() const;
    Color borderColor() const;
    Color selectionColor() const;
    Color fileColor(FileType type) const;

    // Keys
    std::vector<std::string> keysFor(std::string const & action) const;
    std::string actionFor(KeyEvent const & key) const;

    // External
    std::string editor() const;
    std::string pager() const;
    std::string shell() const;

private:
    Config() = default;
    json::Value _config;
    json::Value _defaults;

    template<typename T>
    T get(std::string const & path, T defaultValue) const;
};
```

---

## 사용 예시

```cpp
// 설정 로드
Config::instance().load();

// 색상 가져오기
Color dirColor = Config::instance().fileColor(FileType::Directory);

// 키 바인딩 확인
if (Config::instance().actionFor(keyEvent) == "quit")
{
    // 종료 처리
}

// 설정값 가져오기
bool showHidden = Config::instance().showHidden();
```

---

## 기본 설정 파일 생성

프로그램 최초 실행 시 `~/.config/kgm/config.json` 파일이 없으면:

1. 디렉토리 생성
2. 기본 설정 파일 생성 (주석 포함 불가하므로 별도 README 제공)

```cpp
void Config::createDefault()
{
    auto configDir = getConfigDir();
    std::filesystem::create_directories(configDir);

    auto configPath = configDir / "config.json";
    if (!std::filesystem::exists(configPath))
    {
        std::ofstream file(configPath);
        file << json::stringify(_defaults, true);
    }
}
```

---

## v0.1 범위

v0.1에서는 최소한의 설정만 지원:

- [x] JSON 파서 구현
- [x] 설정 파일 로드
- [x] 색상 설정
- [ ] 키 바인딩 (v0.2)
- [ ] 확장자별 색상 (v0.2)
- [ ] 외부 프로그램 연동 (v0.2)
