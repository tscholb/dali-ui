# SVG reload 샘플

[소스](svg-reload-example.cpp)는 같은 경로의 SVG 파일을 덮어쓴 뒤
`ImageView::Reload()`로 새 내용을 다시 읽는 과정을 보여 준다.
SVG의 기본 크기는 파일의 `width`, `height`에서 오므로, 파일이 바뀌면
reload해야 natural size와 화면 내용이 함께 갱신된다.

시작할 때 시스템 임시 디렉터리에 `dali-ui-svg-reload-sample.svg`를 쓰고,
두 `ImageView`가 이 파일을 표시한다.

- 왼쪽 view는 `WRAP_CONTENT`라서 크기가 SVG의 `width`, `height`를 따른다.
- 오른쪽 view는 160×160 고정 크기라서 내용만 바뀐다.

파일 내용은 아래 세 가지를 차례로 돈다.

| 이름 | 크기 | 색 |
|---|---|---|
| blue | 120×60 | 파랑 |
| green | 240×120 | 초록 |
| orange | 90×180 | 주황 |

## 빌드와 실행

`samples` 디렉터리 기준이다. CMake·C++17 도구와 pkg-config로 검색 가능한
`dali3-core`, `dali3-adaptor`, `dali3-ui-foundation`, `dali3-ui-components`가 필요하다.
설치 prefix의 `lib/pkgconfig`를 `PKG_CONFIG_PATH`에, `lib`를 `LD_LIBRARY_PATH`에 추가한다.
실행에는 DALi가 지원하는 실제 디스플레이 환경이 필요하다.

```sh
cmake -S . -B _build -DDALI_UI_SAMPLE_LIST=svg-reload
cmake --build _build --target svg-reload.example
./svg-reload/bin/svg-reload.example
```

out-of-source 빌드여도 [공통 CMake](../common.cmake)는 실행 파일을 소스 디렉터리의 `bin/`에 만든다.

## 조작과 기대 결과

- 화면 위쪽 글에는 디스크의 파일 내용, 현재 natural size, 마지막 reload 이후
  받은 ResourceReady 신호 수가 표시된다.
- `Rewrite file (W)` 버튼이나 W 키는 파일을 다음 내용으로 덮어쓴다.
  이 단계에서는 화면이 바뀌지 않아야 한다. 글의 파일 내용만 바뀐다.
- `Reload (R)` 버튼이나 R 키는 두 view의 `Reload()`를 호출한다.
  두 view가 새 색으로 다시 그려지고, 왼쪽 view의 크기와 natural size가 새 파일의 크기로 바뀐다.
  ResourceReady 신호는 view마다 한 번씩 와야 한다.
- Escape나 Back 키는 종료한다.

## 자동 실행

`--auto`를 주면 파일을 한 번 바꾸고 reload한 뒤 각 단계의 상태를 표준 출력에 찍고 종료한다.
status 값은 `Visual::ResourceStatus`이며 0은 PREPARING, 1은 READY, 2는 FAILED다.

```sh
./svg-reload/bin/svg-reload.example --auto | grep SvgReload
```

SVG reload가 동작하면 마지막 줄은 다음과 같아야 한다.

```text
[SvgReload] after reload: file=green natural=240x120 status=1/1 readySignals=2
```

reload가 동작하지 않는 라이브러리에서는 natural size가 120x60으로 남고 readySignals가 0이다.
이 문서는 실행 결과 기록이 아니다. 실제 확인 결과는 변경 내역에 따로 남긴다.
