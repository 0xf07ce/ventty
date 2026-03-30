# GfxFont: TTF -> FontAtlas 마이그레이션

## Context

GfxFont는 현재 SDL3_ttf를 사용하여 런타임에 TTF를 열고 글리프마다 개별 SDL_Texture를 생성한다.
터미널은 고정폭 셀 그리드이므로 동적 폰트 사이즈가 불필요하고, 글리프당 텍스처는 비효율적이다.

RogueLike 예제의 FontAtlas가 이미 검증된 비트맵 아틀라스 방식(PNG + BMFont .fnt)을 구현하고 있으므로,
이를 ventty_gfx에 도입하여 SDL3_ttf 의존성을 제거한다.

## 목표

- GfxFont를 FontAtlas 기반으로 교체
- SDL3_ttf 의존성 제거
- RogueLike 폰트 에셋(cp437_8x16, hangul_16x16) 재사용
- 기존 GfxTerminal/GfxRenderer 공개 API 최소 변경

## 변경 파일

### 1. `src/ventty/gfx/GfxFont.h` — 전면 재작성

현재 API:
```cpp
struct GlyphEntry { SDL_Texture * texture; int width, height, advance; };
class GfxFont {
    bool open(string const & ttfPath, float ptSize);
    void setRenderer(SDL_Renderer * renderer);
    void shutdown();
    GlyphEntry const * getGlyph(char32_t codepoint);
    int cellWidth() const;
    int cellHeight() const;
};
```

새 API (RogueLike FontAtlas 기반):
```cpp
struct GlyphInfo {
    int atlasX = 0;
    int atlasY = 0;
    int width = 0;
    int height = 0;
    int advance = 1;   // 1: 반각, 2: 전각
    int fontIndex = 0;
};

class GfxFont {
    bool load(SDL_Renderer * renderer, string const & pngPath, string const & fntPath);
    void shutdown();
    GlyphInfo const * getGlyph(char32_t codepoint) const;
    SDL_Texture * getTexture(int fontIndex) const;
    int cellWidth() const;
    int cellHeight() const;
};
```

주요 변경:
- `GlyphEntry` -> `GlyphInfo` (아틀라스 좌표 기반)
- `open(ttfPath, ptSize)` -> `load(renderer, pngPath, fntPath)` (다중 호출로 다중 아틀라스)
- `setRenderer()` 제거 (load 시점에 renderer 전달)
- `renderGlyph()` 제거 (사전 생성된 아틀라스 사용)
- `getTexture(fontIndex)` 추가
- TTF_Font, 글리프 캐시 제거
- BMFont .fnt 파서 추가 (`parseFnt()`)
- 구현은 `examples/RogueLike/src/terminal/Font.cpp`를 ventty 네임스페이스로 이식

### 2. `src/ventty/gfx/GfxRenderer.h` — 시그니처 유지, 내부 변경

- `renderCell()` / `renderWindow()` 시그니처는 `GfxFont &` 그대로 유지
- renderCell 내부:
  - `font.getGlyph(cell.ch)` -> `GlyphInfo const *` 반환 (아틀라스 좌표)
  - `font.getTexture(glyph->fontIndex)`로 텍스처 획득
  - `SDL_RenderTexture(renderer, tex, &srcRect, &dstRect)` — srcRect 추가
  - 기존 underline/strikethrough 로직 유지

### 3. `src/ventty/gfx/GfxTerminal.h/.cpp` — init() 시그니처 변경

현재:
```cpp
bool init(int cols, int rows, string const & title,
          string const & fontPath = "Menlo", float fontSize = 16.0f, int scale = 0);
```

새:
```cpp
bool init(int cols, int rows, string const & title, int scale = 0);
bool loadFont(string const & pngPath, string const & fntPath);
```

변경:
- `init()`에서 fontPath/fontSize 파라미터 제거
- `loadFont()` 추가 — 내부적으로 `_font.load(renderer, pngPath, fntPath)` 호출
- `TTF_Init()` / `TTF_Quit()` 호출 제거
- 첫 `loadFont()` 호출 시 셀 크기가 결정되므로, renderer init은 `init()` 시점에 기본값으로 생성 후 첫 `loadFont()` 후에 논리 크기 업데이트
  - 또는: init()에서는 SDL/renderer만 생성하고, loadFont() 이후 별도 `finalize()` 호출
  - **선택**: init()에서 SDL 윈도우/렌더러 생성, loadFont()에서 셀 크기 확정 후 `_renderer.updateLogicalSize()` 호출. 첫 loadFont 호출 전까지는 기본 8x16.

실제 초기화 순서:
```cpp
_terminal.init(80, 25, "MDIR");
_terminal.loadFont(basePath + "assets/cp437_8x16.png", basePath + "assets/cp437_8x16.fnt");
_terminal.loadFont(basePath + "assets/hangul_16x16.png", basePath + "assets/hangul_16x16.fnt");
```

### 4. `src/CMakeLists.txt` — SDL3_ttf 제거

```cmake
# 제거
find_package(SDL3_ttf REQUIRED)
SDL3_ttf::SDL3_ttf

# 유지
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
```

### 5. 폰트 에셋 배치

`examples/RogueLike/assets/`의 폰트 에셋을 ventty_gfx에서도 사용:
- `cp437_8x16.png` + `cp437_8x16.fnt`
- `hangul_16x16.png` + `hangul_16x16.fnt`

에셋 위치 옵션:
- **A**: `assets/fonts/` 디렉토리에 복사 (ventty 프로젝트 루트)
- **B**: RogueLike 에셋을 심볼릭 링크 또는 빌드 시 복사
- **선택 B**: 에셋 중복을 피하고, 각 예제/앱의 CMakeLists에서 RogueLike assets를 참조하거나 빌드 시 복사

### 6. `src/ventty/ventty_gfx.h` — 변경 없음 (GfxFont.h 경로 동일)

### 7. mdir_gfx 등 사용처 업데이트

기존:
```cpp
string fontPath = GfxTerminal::basePath() + "assets/fonts/IyagiGGC.ttf";
_terminal->init(80, 25, "MDIR", fontPath, 16.0f);
```

새:
```cpp
_terminal->init(80, 25, "MDIR");
string base = GfxTerminal::basePath() + "assets/";
_terminal->loadFont(base + "cp437_8x16.png", base + "cp437_8x16.fnt");
_terminal->loadFont(base + "hangul_16x16.png", base + "hangul_16x16.fnt");
```

## 구현 순서

1. GfxFont.h/cpp 재작성 (FontAtlas 로직 이식)
2. GfxRenderer.cpp renderCell() 수정 (srcRect 추가)
3. GfxTerminal.h/cpp 수정 (init 시그니처 변경, loadFont 추가, TTF 제거)
4. CMakeLists.txt에서 SDL3_ttf 제거
5. 에셋 복사 설정 (CMake POST_BUILD)
6. 사용처(mdir_gfx 등) 업데이트
7. 빌드 및 실행 테스트

## 검증

```bash
cmake -B cmake-build-agent -S .
cmake --build cmake-build-agent
# mdir_gfx 또는 테스트 예제 실행하여 한글/영문 렌더링 확인
```

- CP437 반각 문자 정상 렌더링
- 한글 전각 문자 정상 렌더링 (2셀 차지)
- underline/strikethrough 속성 동작
- SDL3_ttf 없이 빌드 성공
