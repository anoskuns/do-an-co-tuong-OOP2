# 📘 TỔNG HỢP ĐỒ ÁN CỜ TƯỚNG — BÁCH CHIẾN TIÊN CỤC (百战仙局)

> Tài liệu này liệt kê **toàn bộ những gì đang có trong thư mục đồ án**, cách chạy,
> kiến trúc mã nguồn, hệ thống đồ họa, và **danh sách lỗi đã sửa** trong đợt thiết kế lại giao diện.

| Thông tin | Giá trị |
|:---|:---|
| Tên trò chơi | **Bách Chiến Tiên Cục** (百战仙局) — Cờ Tướng Tu Tiên Huyền Huyễn |
| Ngôn ngữ | C++17 |
| Thư viện đồ họa / âm thanh | SFML 2.6.1 (đi kèm trong `third_party/`) |
| Thư viện JSON | nlohmann/json (header-only, đi kèm) |
| Build | CMake 3.16+ + Visual Studio 2022 (x64, cấu hình Release) |
| Kích thước cửa sổ | 1280 × 720 |
| Hai phe | Thái Hư Huyễn Phái (Đỏ) — Trầm Mộc Đan Tông (Đen) |
| Triết lý | *Lạc tử vô hối* (落子无悔) — **không có chức năng hoàn tác (Undo)** |

---

## 1. 🚀 Chạy nhanh

### Cách 1 — Chạy ngay file đã build sẵn
```
build\Release\BachChienTienCuc.exe
```

### Cách 2 — Build lại bằng dòng lệnh
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -B build -S . -G "Visual Studio 17 2022" -A x64
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Release
```

### Cách 3 — Mở bằng Visual Studio 2022
Mở `build\BachChienTienCuc.sln`, chọn **Release / x64**, nhấn **F5**.

### Chạy kiểm thử tự động
```
build\Release\ChessLogicTest.exe
```

### Chụp ảnh giao diện của cả 7 màn hình (tính năng mới)
```bash
BCT_SCREENSHOT="duong_dan_thu_muc" build/Release/BachChienTienCuc.exe
```
Trò chơi sẽ lần lượt dựng từng màn hình rồi lưu ra `Menu.png`, `Game.png`,
`FactionSelect.png`, `Settings.png`, `Help.png`, `Result.png`, `SaveLoad.png`.
> Biến môi trường `BCT_EXPORT_PLACEHOLDERS=1` sẽ xuất thêm toàn bộ hình ảnh
> procedural ra file PNG để tiện chỉnh sửa.

---

## 2. 🖼️ Ảnh xem trước

Thư mục **`Preview/`** chứa ảnh chụp thật của cả 7 màn hình:

| File | Màn hình |
|:---|:---|
| `Preview/Menu.png` | Sảnh chính — tiêu đề, con dấu, sương linh khí |
| `Preview/Game.png` | Bàn cờ + bảng điều khiển (đang chọn quân Xe) |
| `Preview/FactionSelect.png` | Chọn tông phái & cảnh giới AI |
| `Preview/Settings.png` | Cài đặt âm thanh, toàn màn hình |
| `Preview/Help.png` | Luật cờ tướng |
| `Preview/Result.png` | Kết quả ván đấu |
| `Preview/SaveLoad.png` | Lưu / tải 3 mật thất ván cờ |

---

## 3. 📁 Cấu trúc thư mục đầy đủ

```
DoAnCoTuong/
├── CMakeLists.txt                 # Cấu hình build; tự sinh BuildConfig.h và copy assets/
├── README.md                      # Tài liệu giới thiệu đồ án
├── TONG_HOP_DO_AN.md              # ★ File tổng hợp này
├── CoTuong.cpp                    # Bản cờ tướng 1 file duy nhất thuở ban đầu (KHÔNG nằm trong build)
│
├── Preview/                       # ★ Ảnh chụp 7 màn hình giao diện
│
├── assets/                        # Tài nguyên trò chơi
│   ├── fonts/
│   │   ├── calligraphy.ttf        # Font thư pháp Hán tự (KaiTi) — dùng cho 帥, 將, 楚河, 漢界
│   │   └── default.ttf            # Font Latin dự phòng (hỗ trợ đầy đủ dấu tiếng Việt)
│   ├── images/
│   │   ├── pieces/                # Chỗ để người dùng thả ảnh quân cờ riêng (đang trống)
│   │   ├── board/                 # Chỗ để người dùng thả ảnh bàn cờ riêng (đang trống)
│   │   └── _legacy_placeholders/  # 15 ảnh placeholder cũ do bản trước tự sinh (đã ngừng dùng)
│   ├── sounds/ (bgm/, sfx/)       # Thả file .ogg/.wav vào để có nhạc & hiệu ứng
│   ├── saves/                     # 3 slot lưu ván cờ JSON
│   └── settings.json              # Âm lượng & chế độ toàn màn hình
│
├── src/
│   ├── main.cpp                   # Điểm vào chương trình
│   │
│   ├── core/                      # ★ Tầng hạ tầng
│   │   ├── Common.h               # Hằng số, enum (Side, PieceType, GameMode, GameState…), layout bàn cờ
│   │   ├── AssetPaths.h/.cpp      # ★ MỚI — Tự dò thư mục assets/ ở nhiều vị trí
│   │   ├── BuildConfig.h.in       # ★ MỚI — Mẫu header do CMake sinh ra (đường dẫn assets gốc)
│   │   ├── AssetManager.h/.cpp    # ★ VIẾT LẠI — Sinh toàn bộ đồ họa bằng thuật toán
│   │   ├── AudioManager.h/.cpp    # Quản lý nhạc nền & hiệu ứng (pool 16 kênh)
│   │   ├── SettingsManager.h/.cpp # Đọc/ghi settings.json
│   │   ├── Scene.h                # Lớp trừu tượng cho mọi màn hình
│   │   ├── SceneManager.h/.cpp    # Quản lý & chuyển màn hình
│   │   └── Game.h/.cpp            # Vòng lặp chính, tạo cửa sổ, chế độ chụp ảnh
│   │
│   ├── chess/                     # ★ Tầng logic cờ (thuần OOP, không dính đồ họa)
│   │   ├── Board.h/.cpp           # Bàn cờ 10×9, thế trận, lịch sử nước đi, JSON
│   │   ├── Move.h                 # Struct nước đi
│   │   ├── GameController.h/.cpp  # Điều phối lượt, chọn quân, chiếu tướng, lưu/tải
│   │   ├── AIPlayer.h/.cpp        # Minimax + cắt tỉa Alpha-Beta
│   │   └── pieces/
│   │       ├── Piece.h/.cpp       # Lớp cha trừu tượng của 7 loại quân
│   │       └── King/Guard/Bishop/Knight/Rook/Cannon/Pawn .h/.cpp
│   │
│   ├── ui/                        # ★ Tầng giao diện
│   │   ├── Theme.h/.cpp           # ★ MỚI — Bảng màu + hàm vẽ bo góc/chuyển sắc/quầng sáng
│   │   ├── Button.h/.cpp          # ★ VIẾT LẠI — Nút bo góc, đổ bóng, sáng dần khi rê chuột
│   │   └── Panel.h/.cpp           # ★ VIẾT LẠI — Khung nội dung, hoa văn góc, đường phân cách
│   │
│   ├── scenes/                    # 7 màn hình
│   │   ├── MenuScene.h/.cpp       # ★ THIẾT KẾ LẠI — Sảnh chính
│   │   ├── GameScene.h/.cpp       # ★ THIẾT KẾ LẠI — Bàn cờ & bảng điều khiển
│   │   ├── FactionSelectScene.*   # Chọn phe & độ khó
│   │   ├── SettingsScene.*        # Cài đặt
│   │   ├── HelpScene.*            # Luật cờ
│   │   ├── ResultScene.*          # Kết quả
│   │   └── SaveLoadScene.*        # Lưu / tải ván
│   │
│   └── effects/
│       ├── Particle.h             # Struct hạt (vị trí, vận tốc, màu, vòng đời, loại)
│       └── ParticleSystem.h/.cpp  # ★ NÂNG CẤP — Hạt vẽ bằng sprite quầng sáng + blend cộng sáng
│
├── tests/
│   └── test_chess_logic.cpp       # 7 test case kiểm thử luật cờ, AI, lưu/tải
│
├── third_party/
│   ├── nlohmann/json.hpp
│   └── SFML-2.6.1/
│
└── Thamkhao/
    └── DoAnOOP2.pdf               # Đề bài tham khảo
```
★ = file mới hoặc được viết lại trong đợt thiết kế giao diện này.

---

## 4. 🏛️ Kiến trúc lập trình hướng đối tượng

### 4.1. Bốn tính chất cốt lõi

**① Tính đóng gói (Encapsulation)**
- `Board` giữ `grid[10][9]`, `allPieces`, `history` ở phạm vi `private`; bên ngoài chỉ truy cập qua `getPieceAt()`, `getPieces()`, `getHistory()`.
- `Piece` có `row`, `col`, `alive` là `protected`; thay đổi qua `setPosition()`, `setAlive()`.
- `SettingsManager` bọc `sfxVolume`, `musicVolume`, `fullscreen` và chỉ cho sửa qua các setter có kẹp biên độ `std::clamp`.
- `AssetManager` giấu `textures`, `primaryFont`, `uiFont`; bên ngoài chỉ gọi `getTexture()`, `getFont()`, `getUiFont()`.

**② Tính kế thừa (Inheritance)**
- `Piece` (lớp cha trừu tượng) → 7 lớp con: `King`, `Guard`, `Bishop`, `Knight`, `Rook`, `Cannon`, `Pawn`.
- `Scene` (lớp cha trừu tượng) → 7 lớp con: `MenuScene`, `FactionSelectScene`, `GameScene`, `SettingsScene`, `HelpScene`, `ResultScene`, `SaveLoadScene`.

**③ Tính đa hình (Polymorphism)**
- Hàm ảo thuần túy `Piece::getPseudoLegalMoves()` được **override riêng cho từng loại quân**, xử lý đúng luật đặc thù: mắt Voi (Tượng), cản chân Mã, ngòi Pháo, giới hạn Cửu Cung, Tốt qua sông.
- `SceneManager` giữ `std::map<std::string, std::unique_ptr<Scene>>` và gọi `current->update(dt)`, `current->render(window)` **qua con trỏ lớp cha**.
- `ParticleSystem::update()` phân nhánh theo `ParticleType` (MIST / SPARK / EXPLOSION / CHECK_AURA) với cách xử lý độ mờ khác nhau.

**④ Tính trừu tượng (Abstraction)**
- Tách bạch 5 tầng: **logic cờ** (`chess/`), **hạ tầng** (`core/`), **giao diện** (`ui/`), **màn hình** (`scenes/`), **hiệu ứng** (`effects/`).
- `Scene` định nghĩa giao kèo chung `onEnter()`, `onExit()`, `handleEvent()`, `update()`, `render()` mà không biết gì về nội dung cụ thể của từng màn hình.

### 4.2. Mẫu thiết kế (Design Patterns) đang dùng

| Pattern | Vị trí áp dụng |
|:---|:---|
| **Template Method** | `Scene` chốt khung vòng đời; lớp con điền chi tiết |
| **State** | `SceneManager` chuyển đổi 7 màn hình; `GameState` mô tả thế cờ |
| **Observer / Callback** | `GameController::setOnMoveMadeCallback`, `setOnPieceCapturedCallback`, `setOnCheckCallback`, `setOnGameOverCallback` — `GameScene` đăng ký để bắn hiệu ứng hạt |
| **Strategy** | `Difficulty` (Luyện Khí / Trúc Cơ / Kim Đan) đổi độ sâu và cách chọn nước của `AIPlayer` |
| **Facade** | `Game` gom `AssetManager`, `AudioManager`, `SettingsManager`, `SceneManager`, `GameController` thành một mặt tiền duy nhất |
| **Flyweight / Cache** | `AssetManager` sinh texture **một lần** rồi tái sử dụng; `Theme::glowTexture()` là một static texture dùng chung cho mọi quầng sáng |
| **Null Object** | `AssetManager::getTexture()` trả về texture trong suốt 1×1 khi thiếu, tránh crash khi vẽ |

---

## 5. 🎨 Hệ thống đồ họa (đợt thiết kế lại)

### 5.1. `src/ui/Theme.h` — Bộ "design token"

Toàn bộ giao diện lấy màu sắc và hình khối từ đây, nhờ vậy 7 màn hình trông đồng nhất.

**Bảng màu:** `Ink`, `Night`, `Indigo` (nền) · `WoodDark/Mid/Light` (gỗ) · `GoldDark/Gold/GoldBright/GoldPale` (hoàng kim) · `Jade/JadeBright` (bích ngọc) · `Crimson/CrimsonBright` (chu sa) · `Ivory` (ngọc ngà) · `Steel/SteelBright` (huyền thiết) · `TextDim/TextLight`.

**Hàm vẽ dùng chung:**
| Hàm | Công dụng |
|:---|:---|
| `drawRoundedRect()` | Hình chữ nhật bo góc (dựng bằng `sf::VertexArray` nên góc tròn mượt, viền dày tùy ý) |
| `drawRoundedRectGradient()` | Như trên nhưng tô chuyển sắc dọc, **vẫn giữ nguyên góc bo tròn** |
| `drawVerticalGradient()` | Dải chuyển sắc 4 đỉnh |
| `drawSoftShadow()` | Đổ bóng mềm nhiều lớp phía sau khối bo góc |
| `drawGlow()` | Quầng sáng tỏa tròn, dùng texture gradient dựng sẵn nên không bị vân bậc thang |
| `glowTexture()` | Texture gradient tròn 256×256 sinh một lần, chia sẻ cho mọi quầng sáng/bóng đổ |
| `toUnicode()` | Chuyển `std::string` UTF-8 (tiếng Việt có dấu, Hán tự) sang `sf::String` **một cách an toàn** |

### 5.2. `src/core/AssetManager.cpp` — Vẽ toàn bộ đồ họa bằng thuật toán

Trò chơi **không phụ thuộc vào file ảnh ngoài**: mọi hình ảnh đều được sinh bằng code.

| Tài nguyên | Cách dựng |
|:---|:---|
| **Bàn cờ** (`board`) | Texture 1216×1340 (gấp đôi rồi thu nhỏ 0.5 → sắc nét). Gồm: khung gỗ sẫm có vân gỗ lượn sóng, mặt bàn gỗ sáng có vân mịn, bóng mờ mép trong (cảm giác lõm nhẹ), viền kim tuyến 2 lớp, hoa văn 4 góc (2 thanh ôm góc + viên ngọc thoi), lưới 10×9, **con sông ngắt quãng** với chữ **楚 河 / 漢 界**, đường chéo Cửu Cung, **hoa văn định vị** (ngoặc vuông ở 4 điểm Pháo và 10 điểm Binh/Tốt), và **toạ độ cột** (số Ả Rập 1–9 phía Đen, số Hán 九…一 phía Đỏ) |
| **14 quân cờ** | Texture 256×256. Gồm: bóng đổ, vành màu phe (chu sa cho Đỏ / trầm mộc–bích ngọc cho Đen), ánh sáng viền trên-trái + bóng viền dưới-phải tạo khối nổi, thân quân gradient hướng tâm 56 lớp, 2 vòng hoa văn khắc, **chữ Hán khắc nổi** (bản bóng chìm + nét chính), điểm phản chiếu mềm, và **vòng kim tuyến riêng cho Tướng**. Quân Đỏ = ngọc trắng ngà + chữ chu sa; quân Đen = trầm mộc pha bích ngọc + chữ bích ngọc |
| **Nền huyền ảo** (`scene_bg`) | 1280×720 dùng chung cho cả 7 màn hình: trời đêm chuyển sắc, quầng linh khí, 220 ngôi sao, **3 dải núi xa** tạo chiều sâu, sương quyện, vignette tối 4 phía |
| **Con dấu** (`emblem`) | 512×512: quầng sáng, 3 vòng đồng tâm, 8 viên ngọc thoi, chữ **棋** ở giữa — dùng làm watermark màn hình chính và làm **biểu tượng cửa sổ** |
| **Hạt sáng** (`particle_spark`) | 64×64 quầng sáng mềm có lõi sáng |

**Ghi đè bằng ảnh riêng:** nếu tồn tại `assets/images/pieces/<tên>.png` hoặc
`assets/images/board/board_bg.png`, game sẽ **ưu tiên dùng ảnh của người dùng**.

### 5.3. `src/scenes/GameScene.cpp` — Bàn cờ & bảng điều khiển

**Bàn cờ:** vẽ texture bàn cờ đã dựng sẵn, phủ thêm lớp hiệu ứng động.

**Hiệu ứng tương tác:**
- **Nước đi vừa rồi:** 2 ô sáng mờ + khung góc kim tuyến + quầng sáng ở ô đến.
- **Quân đang chọn:** quầng sáng vàng **nhấp nháy theo hàm sin** + khung bo góc vàng.
- **Nước đi hợp lệ:** ô trống → chấm ngọc lam có viền và quầng sáng; ô ăn quân → **vòng tròn đỏ huyết nhấp nháy**.
- **Khi bị chiếu tướng:** quầng đỏ mờ nhấp nháy quanh khu vực bàn cờ.

**Bảng điều khiển bên phải:**
- `Panel` tiêu đề *BÁCH CHIẾN TIÊN CỤC* với đường phân cách có viên ngọc.
- **Thẻ lượt đi:** nền chuyển sắc, thanh màu phe bên trái, tên tông phái đang đi, tên đối thủ, và **huy hiệu tròn mang chữ 帥/將**.
- Dòng trạng thái thế cờ (đang giao tranh / chiếu tướng / thắng / hòa).
- Thông báo *Tiên Cơ đang ngưng thần tính toán…* nhấp nháy khi AI suy nghĩ.
- **Sử ký đối pháp:** 5 nước gần nhất, nền sọc xen kẽ, nước mới nhất tô vàng.
- 3 nút: *Lưu Ván Đấu*, *Khai Cuộc Mới*, *Thoát Về Menu* (**cố ý không có nút Hoàn Tác**).
- Chân trang: **落 子 无 悔** + dòng chú thích tiếng Việt.

### 5.4. `src/scenes/MenuScene.cpp` — Sảnh chính

Con dấu **棋** xoay rất chậm làm watermark sau tiêu đề · tiêu đề vàng 70px có bóng đổ + quầng sáng "thở" theo nhịp sin · Hán tự **百 战 仙 局** · phụ đề hai tông phái · đường phân cách kim tuyến có viên ngọc · 6 nút chức năng · sương linh khí bay lên.

### 5.5. `src/ui/Button.cpp` & `Panel.cpp`

**Button:** bóng đổ, **quầng sáng bật dần khi rê chuột** (nội suy mượt theo `dt`, không nhảy tức thời), thân chuyển sắc dọc, viền ngoài + viền trong mảnh, vệt sáng cạnh trên gợi chất kim loại, chữ có bóng mờ, và **hạ xuống 2px khi nhấn**. 4 phong cách: `GOLD`, `CYAN`, `DANGER`, `DARK`.

**Panel:** bóng đổ, thân chuyển sắc hơi trong suốt để nền huyền ảo lộ qua, ánh sáng mềm phía trên, viền kim tuyến 2 lớp, **4 viên ngọc thoi ở 4 góc**, dải tiêu đề, đường phân cách có viên ngọc ở giữa, tiêu đề kèm bóng mờ.

### 5.6. Hiệu ứng hạt (ParticleSystem)

Hạt được vẽ bằng **sprite quầng sáng mềm** thay vì hình tròn đặc, và dùng **blend cộng sáng (`sf::BlendAdd`)** cho tia lửa/vụ nổ nên trông rực rỡ hơn hẳn.

| Hiệu ứng | Kích hoạt khi |
|:---|:---|
| `emitMist()` | Sương linh khí bay lên ở sảnh chính |
| `emitMoveTrail()` | Quân cờ di chuyển — để lại vệt sáng |
| `emitCaptureExplosion()` | Ăn quân — vụ nổ tia sáng |
| `emitCheckAura()` | Tướng bị chiếu — vòng cảnh báo đỏ |

---

## 6. 🔤 Hệ thống font hai lớp (điểm quan trọng)

Font thư pháp Hán tự (KaiTi) **không chứa các ký tự Latin có dấu của tiếng Việt**
(ă, â, đ, ơ, ư, ế, ộ…). Nếu dùng nó cho toàn bộ giao diện thì mọi chữ tiếng Việt
sẽ hiện thành **ô vuông ▯**. Vì vậy game dùng **hai font tách biệt**:

| Font | Vai trò | Nạp từ |
|:---|:---|:---|
| **Font Hán tự** — `getFont()` | Hán tự & con dấu: 帥 將 楚 河 漢 界 兵 卒 棋 落子无悔 | `assets/fonts/calligraphy.ttf` → dự phòng `simkai.ttf`, `simsun.ttc`, `msyh.ttc` |
| **Font giao diện** — `getUiFont()` | Toàn bộ chữ Latin/tiếng Việt có dấu | `assets/fonts/ui.ttf` / `vietnamese.ttf` → `pala.ttf` (Palatino Linotype) → `georgia.ttf` → `cambria.ttc` → `assets/fonts/default.ttf` → `segoeui.ttf` → `arial.ttf` |

Game **tự kiểm tra** font có thật sự chứa ký tự tiếng Việt hay không (so sánh glyph
với ký tự `.notdef`) trước khi chọn, nên không bao giờ chọn nhầm font thiếu dấu.
Muốn đổi font giao diện: chỉ cần thả file `.ttf` vào `assets/fonts/ui.ttf`.

---

## 7. 🐞 Các lỗi đã sửa trong đợt này

Đây là lý do trước đó **bạn không thấy giao diện đồ họa gì cả**. Có 4 lỗi độc lập
chồng lên nhau, mỗi lỗi đều đủ để làm giao diện trắng trơn.

### ① Liên kết thư mục `assets/` bị hỏng → game không tìm thấy tài nguyên nào
`build/Release/assets` là một **symlink hỏng**, trỏ tới `…/3.OOP/assets` (không tồn tại).
Hệ quả: không nạp được font, không nạp được ảnh, không ghi được file lưu.
Game vẫn mở cửa sổ nhưng **toàn bộ chữ đều tàng hình** → nhìn như màn hình trống.

**Đã sửa:**
- Xóa symlink hỏng.
- `CMakeLists.txt` **tự copy toàn bộ `assets/` vào thư mục chứa file .exe** sau mỗi lần build.
- Thêm module mới `src/core/AssetPaths.h/.cpp`: tự dò thư mục `assets/` theo thứ tự
  `BCT_ASSETS` → `./assets` → `assets` cạnh file .exe → các cấp thư mục cha → đường dẫn gốc lúc biên dịch (`BuildConfig.h`).
  Nhờ vậy game chạy được **từ Explorer, từ Visual Studio, hay từ dòng lệnh** ở bất kỳ thư mục làm việc nào.
- `SettingsManager`, `AudioManager`, `GameController`, `SaveLoadScene` đều dùng `assetPath()` thay vì chuỗi `"assets/…"` cứng.
- Tự tạo sẵn các thư mục `images/pieces`, `images/board`, `sounds/bgm`, `sounds/sfx`, `saves`.

### ② Chuyển chuỗi UTF-8 sai — lỗi undefined behavior làm chữ biến mất
Mẫu code cũ xuất hiện **13 chỗ** trong 4 màn hình:
```cpp
// SAI: hai đối tượng std::string tạm thời KHÁC NHAU, begin() và end() không cùng một chuỗi
sf::String::fromUtf8(std::string("Chọn Tông Phái").begin(),
                     std::string("Chọn Tông Phái").end());
```
`.begin()` thuộc về đối tượng tạm thứ nhất còn `.end()` thuộc về đối tượng tạm thứ hai
→ hành vi không xác định, thực tế cho ra **chuỗi rỗng** nên chữ không bao giờ hiện.
Vì lỗi này, màn *Chọn tông phái* trước đây **mất hẳn tiêu đề và các dòng nhắc**,
màn *Cài đặt / Trợ giúp / Lưu–Tải* cũng mất chữ tương tự.

**Đã sửa:** thay cả 13 chỗ bằng `Theme::toUnicode("…")` (một hàm dùng đúng một đối tượng chuỗi).

### ③ `sf::TriangleFan` không tự khép vòng → mọi khối bo góc thiếu một mảnh
`sf::TriangleFan` chỉ vẽ các tam giác `(v0,v1,v2), (v0,v2,v3), …` nên **không vẽ tam giác
nối điểm cuối về điểm đầu**. Kết quả: mọi nút bấm và mọi khung nền đều **khuyết mất một
mảnh tam giác lớn ở phía bên trái** — nhìn như bị cắt chéo.

**Đã sửa:** lặp lại điểm đầu tiên ở cuối danh sách đỉnh để khép kín vòng, trong cả
`drawRoundedRect()` và `drawRoundedRectGradient()`.

### ④ Giao diện cũ quá đơn sơ + font thiếu dấu
Bàn cờ cũ chỉ là hình chữ nhật tối với vài đường kẻ mảnh; quân cờ là hình tròn đơn giản;
chữ tiếng Việt hiện thành ô vuông. Đã thay bằng toàn bộ hệ thống đồ họa ở mục 5 và 6.

### ⑤ (Phụ) Ảnh placeholder cũ ghi đè đồ họa mới
15 file PNG do bản cũ tự sinh (`assets/images/pieces/*.png`, `board_bg.png`) sẽ được
nạp như "ảnh người dùng" và che mất đồ họa mới. Đã **chuyển sang
`assets/images/_legacy_placeholders/`** (không xóa, có thể khôi phục nếu cần).
Tính năng tự xuất ảnh giờ chỉ chạy khi đặt `BCT_EXPORT_PLACEHOLDERS=1`.

---

## 8. 🎮 Tính năng

- **2 chế độ chơi**
  - **Đại Chiến Tiên Ma (PvE)** — đấu với AI *Tiên Cơ* (Minimax + cắt tỉa Alpha-Beta), 3 cảnh giới:
    - *Luyện Khí (Dễ)* — độ sâu 1, chọn ngẫu nhiên trong nhóm nước tốt.
    - *Trúc Cơ (Trung bình)* — độ sâu 3.
    - *Kim Đan (Khó)* — độ sâu 4–5, có bảng điểm vị trí chiến lược và sắp xếp nước đi.
  - **Tông Môn Luận Đạo (PvP)** — hai người cùng một máy.
- **Luật cờ tướng đầy đủ:** cản chân Mã, mắt Voi, ngòi Pháo, Tốt qua sông, giới hạn Cửu Cung, cấm lộ mặt Tướng (flying generals), chiếu tướng / chiếu bí / hết nước.
- **Lưu & tải ván cờ** — 3 mật thất độc lập (`save_1/2/3.json`), khôi phục chính xác 100% thế cờ, lượt đi, chế độ và độ khó.
- **Cài đặt** — âm lượng nhạc nền, âm lượng hiệu ứng, chế độ toàn màn hình (lưu vào `settings.json`).
- **Hiệu ứng hạt linh lực** — sương mù, vệt sáng khi di chuyển, vụ nổ khi ăn quân, hào quang cảnh báo chiếu tướng.
- **Âm thanh tùy biến** — quét tự động `assets/sounds/`; thiếu file thì game vẫn chạy bình thường.
- **Chụp ảnh giao diện** — `BCT_SCREENSHOT=<thư mục>`.
- **3 slot gợi ý nước đi** trực quan bằng màu sắc khác nhau cho ô trống và ô ăn quân.

---

## 9. 🖱️ Điều khiển

| Thao tác | Kết quả |
|:---|:---|
| Nhấp quân của phe đang tới lượt | Chọn quân — hiện quầng sáng và mọi nước đi hợp lệ |
| Nhấp ô hợp lệ | Hạ quân (kèm vệt sáng), hoặc ăn quân (kèm vụ nổ) |
| Nhấp quân khác cùng phe | Đổi sang chọn quân đó |
| Nhấp ra ngoài bàn cờ | Bỏ chọn |
| Nút *Khai Cuộc Mới* | Bắt đầu ván mới với cấu hình đang chọn |
| `Alt + F4` / nút X | Thoát |

> Không có nút Hoàn Tác — đúng theo triết lý **Lạc tử vô hối** của đồ án.

---

## 10. ✅ Kiểm thử tự động

`ChessLogicTest.exe` chạy 7 test case — **tất cả đều PASS**:

| Test case | Nội dung kiểm chứng |
|:---|:---|
| `testInitialBoard` | 32 quân, thế trận khai cuộc, 2 Tướng nằm trong Cửu Cung |
| `testPawnMovement` | Luật Tốt trước và sau khi qua sông |
| `testKnightLegBlocking` | Luật cản chân Mã (8 hướng L) |
| `testCannonScreen` | Luật ngòi Pháo (nhảy qua đúng 1 quân để ăn) |
| `testFlyingGenerals` | Luật cấm lộ mặt Tướng |
| `testAIFastMove` | AI Minimax trả về nước đi hợp lệ |
| `testSaveLoadJSON` | Tính toàn vẹn khi lưu & tải JSON |

---

## 11. 🔧 Tùy chỉnh

| Muốn đổi gì | Làm thế nào |
|:---|:---|
| **Quân cờ** | Thả `red_king.png`, `black_pawn.png`… (256×256, nền trong suốt) vào `assets/images/pieces/` — game tự ưu tiên ảnh này |
| **Bàn cờ** | Thả ảnh vào `assets/images/board/board_bg.png` — ảnh sẽ được co giãn vừa khung bàn cờ |
| **Nhạc nền** | Thả `menu_bgm.ogg` và `battle_bgm.ogg` vào `assets/sounds/bgm/` |
| **Hiệu ứng âm thanh** | Thả `select`, `move`, `capture`, `check`, `victory`, `defeat`, `click` (`.ogg`/`.wav`/`.mp3`/`.flac`) vào `assets/sounds/sfx/` |
| **Màu sắc toàn game** | Sửa bảng màu trong `src/ui/Theme.h` (namespace `Theme::Palette`) |
| **Kích thước bàn cờ / bố cục** | Sửa `TILE_SIZE`, `BOARD_OFFSET_X/Y`, `BOARD_MARGIN`, `SIDEBAR_*` trong `src/core/Common.h` |
| **Số nước cờ hiển thị** | Sửa `src/scenes/GameScene.cpp` (mảng `historyTexts`, hiện là 5) |
| **Độ sâu AI** | Sửa enum `Difficulty` trong `src/core/Common.h` và bảng điểm trong `src/chess/AIPlayer.cpp` |

---

## 12. 📌 Ghi chú khi nộp bài

- File `CoTuong.cpp` ở thư mục gốc là **bản cờ tướng một-file thuở ban đầu**, không nằm
  trong `CMakeLists.txt` và **không được biên dịch**. Có thể giữ lại để đối chiếu tiến độ
  hoặc xóa nếu không cần.
- Thư mục `build/` chứa file thực thi và bản sao `assets/` — **có thể xóa và build lại**
  bất cứ lúc nào, không ảnh hưởng mã nguồn.
- `third_party/` đã chứa sẵn SFML 2.6.1 và nlohmann/json nên **không cần cài thêm gì**.
- Nếu chép đồ án sang máy khác mà máy đó **không có Palatino Linotype**, game tự động lùi
  về font `assets/fonts/default.ttf` (đã đi kèm sẵn trong đồ án) nên chữ tiếng Việt
  **luôn hiển thị đúng**.
- Muốn xuất lại toàn bộ ảnh procedural ra PNG để chỉnh sửa bằng tay:
  `BCT_EXPORT_PLACEHOLDERS=1 build/Release/BachChienTienCuc.exe`
