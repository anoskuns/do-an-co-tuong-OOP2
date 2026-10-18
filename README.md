# 🏯 BÁCH CHIẾN TIÊN CỤC (百战仙局)
### Đồ Án Cờ Tướng Tu Tiên Huyền Huyễn — Lập Trình Hướng Đối Tượng (OOP C++ & SFML)

---

## 🌟 Giới Thiệu Dự Án

**Bách Chiến Tiên Cục** (百战仙局) là một trò chơi Cờ Tướng hoàn chỉnh được xây dựng trên nền tảng **C++ hướng đối tượng (OOP)** kết hợp cùng thư viện đồ họa **SFML 2.6.1**, biên dịch qua **Visual Studio 2022 + CMake**. 

Trò chơi lấy bối cảnh huyền huyễn tiên hiệp cổ phong với cuộc đối đầu giữa hai đại môn phái:
- **Thái Hư Huyễn Phái** (Phe Đỏ - Tiên Môn chính đạo)
- **Trầm Mộc Đan Tông** (Phe Đen - Ma Đạo u minh)

Đặc biệt, theo đúng triết lý tu chân **"Lạc tử vô hối"** (落子无悔), game đề cao tính quyết đoán trong từng nước cờ và **không có tính năng hoàn tác (Undo)**.

---

## ⚔️ Tông Phái & Danh Xưng Quân Cờ Tu Tiên

| Quân cờ gốc | Thái Hư Huyễn Phái (Tiên Đạo - Đỏ) | Trầm Mộc Đan Tông (Ma Đạo - Đen) | Ký hiệu truyền thống |
|:---:|:---:|:---:|:---:|
| **Tướng / Soái** | **Huyễn Thiên Tông Chủ** | **Trầm Mộc Đan Tổ** | 帥 / 將 |
| **Sĩ** | **Hộ Pháp Chân Nhân** | **Hắc Vệ Ma Quân** | 仕 / 士 |
| **Tượng** | **Đại Tượng Linh Thú** | **U Minh Hắc Tượng** | 相 / 象 |
| **Xe** | **Cửu Tiêu Linh Xa** | **U Hồn Hắc Xa** | 俥 / 車 |
| **Mã** | **Ngự Phong Thiên Mã** | **Huyết Sát Quỷ Mã** | 傌 / 馬 |
| **Pháo** | **Thiên Hỏa Linh Pháo** | **Cửu U Âm Pháo** | 砲 / 炮 |
| **Tốt / Binh** | **Thái Hư Linh Tốt** | **Trầm Mộc Hắc Tốt** | 兵 / 卒 |

---

## 🏛️ Kiến Trúc Lập Trình Hướng Đối Tượng (OOP)

Dự án áp dụng đầy đủ và chặt chẽ 4 tính chất cốt lõi của OOP cùng các Design Pattern:
1. **Tính Đóng Gói (Encapsulation)**:
   - Dữ liệu bàn cờ (`Board`), thuộc tính quân cờ (`Piece`), âm lượng (`SettingsManager`) đều có phạm vi truy cập `protected` hoặc `private`, chỉ cho phép tương tác qua các phương thức hợp lệ.
2. **Tính Kế Thừa (Inheritance)**:
   - Lớp trừu tượng `Piece` là lớp cha của 7 lớp quân cờ cụ thể: `King`, `Guard`, `Bishop`, `Knight`, `Rook`, `Cannon`, `Pawn`.
   - Lớp trừu tượng `Scene` là lớp cha của 7 màn hình: `MenuScene`, `FactionSelectScene`, `GameScene`, `SettingsScene`, `HelpScene`, `ResultScene`, `SaveLoadScene`.
3. **Tính Đa Hình (Polymorphism)**:
   - Phương thức ảo thuần túy `getPseudoLegalMoves()` được override riêng biệt cho từng loại quân để xử lý logic di chuyển đặc thù (mắt voi, cản chân mã, ngòi pháo, sông, cửu cung).
   - Vòng lặp `SceneManager` cập nhật và vẽ màn hình hiện tại thông qua con trỏ đa hình `Scene*`.
4. **Tính Trừu Tượng (Abstraction)**:
   - Tách biệt hoàn toàn giữa **Logic Game** (`chess/`), **Giao Diện** (`ui/`, `scenes/`), **Hệ Thống Âm Thanh/Tài Nguyên** (`core/`), và **Hiệu Ứng** (`effects/`).

---

## 🎮 Các Tính Năng Nổi Bật

- **2 Chế độ chơi chính**:
  1. **Đại Chiến Tiên Ma (PvE)**: Đấu với AI Tiên Cơ trang bị thuật toán **Minimax + Cắt tỉa Alpha-Beta** với 3 cấp độ tu vi:
     - *Luyện Khí (Dễ)*: Depth 1 + chọn ngẫu nhiên trong nhóm nước tốt.
     - *Trúc Cơ (Trung bình)*: Depth 3.
     - *Kim Đan (Khó)*: Depth 4/5 + bảng điểm vị trí chiến lược và sắp xếp nước đi (Move Ordering).
  2. **Tông Môn Luận Đạo (PvP)**: 2 người so tài trực tiếp trên cùng một máy.
- **Hệ thống Hiệu Ứng Hạt Linh Lực (Particle System)**:
  - Sương mù linh khí huyền ảo lơ lửng tại sảnh chính (Menu).
  - Vệt sáng kéo theo khi phi quân hạ ấn trên bàn cờ.
  - Vụ nổ tia sáng linh lực rực rỡ khi ăn quân đối phương.
  - Hào quang đỏ rực nhấp nháy cảnh báo nguy cấp khi Tướng bị chiếu.
- **Hệ Thống Asset Tự Sinh & Modular Âm Thanh (Safe Placeholder System)**:
  - Game tích hợp bộ vẽ đồ họa procedural dự phòng: tự động vẽ quân cờ và bàn cờ đẹp mắt nếu chưa có ảnh PNG.
  - Người dùng có thể tự do thả file âm thanh `.ogg`/`.wav` vào thư mục `assets/sounds/` hoặc ảnh `.png` vào `assets/images/pieces/` bất cứ lúc nào.
- **Lưu & Tải Ván Đấu (JSON Persistence)**:
  - Hỗ trợ 3 mật thất lưu trữ độc lập (`save_1.json`, `save_2.json`, `save_3.json`), khôi phục ván đấu 100% chính xác.
- **Cài Đặt Càn Khôn**:
  - Tùy chỉnh âm lượng nhạc nền (BGM), hiệu ứng âm thanh (SFX), và chế độ toàn màn hình.

---

## 📁 Cấu Trúc Thư Mục Dự Án

```
DoAnCoTuong/
├── CMakeLists.txt                    # File cấu hình build CMake cho Visual Studio 2022
├── README.md                         # Tài liệu hướng dẫn toàn diện
├── TONG_HOP_DO_AN.md                 # Tổng hợp chi tiết toàn bộ đồ án + danh sách lỗi đã sửa
├── Preview/                          # Ảnh chụp giao diện cả 7 màn hình
├── assets/                           # Tài nguyên trò chơi
│   ├── fonts/
│   │   ├── calligraphy.ttf           # Font thư pháp Hán tự (KaiTi)
│   │   └── default.ttf               # Font Latin dự phòng (hỗ trợ dấu tiếng Việt)
│   ├── images/
│   │   ├── pieces/                   # Thả ảnh quân cờ riêng vào đây (tùy chọn)
│   │   ├── board/                    # Thả ảnh bàn cờ riêng vào đây (tùy chọn)
│   │   └── _legacy_placeholders/     # Ảnh placeholder cũ (bản cũ tự sinh, đã ngừng dùng)
│   ├── sounds/                       # Thư mục âm thanh tự do thêm vào
│   │   ├── bgm/                      # Nhạc nền: menu_bgm.ogg, battle_bgm.ogg
│   │   └── sfx/                      # Hiệu ứng: select, move, capture, check, victory, defeat, click
│   └── saves/                        # 3 slot lưu trữ ván đấu JSON
├── build/Release/                    # Thư mục chứa file chạy hoàn chỉnh (kèm bản sao assets/)
│   ├── BachChienTienCuc.exe          # File thực thi chính của game
│   ├── ChessLogicTest.exe            # File chạy kiểm thử tự động
│   └── *.dll                         # Các thư viện liên kết động của SFML
├── src/
│   ├── main.cpp                      # Điểm vào chính của chương trình
│   ├── core/                         # Hệ thống quản lý cốt lõi
│   │   ├── Common.h                  # Hằng số & enum dùng chung, bố cục bàn cờ
│   │   ├── AssetPaths.h/.cpp         # Tự dò thư mục assets/ ở nhiều vị trí
│   │   ├── AssetManager.h/.cpp       # Sinh toàn bộ đồ họa bằng thuật toán
│   │   ├── AudioManager.h/.cpp       # Nhạc nền & hiệu ứng âm thanh
│   │   ├── SettingsManager.h/.cpp    # Đọc/ghi settings.json
│   │   ├── Scene.h / SceneManager.*  # Lớp cha của màn hình & bộ quản lý chuyển màn
│   │   └── Game.h/.cpp               # Vòng lặp chính, chế độ chụp ảnh giao diện
│   ├── chess/                        # Logic cờ tướng (Board, Move, GameController, AIPlayer)
│   │   └── pieces/                   # 7 loại quân cờ kế thừa từ Piece
│   ├── scenes/                       # 7 màn hình (Menu, Select, Game, Settings, Help, Result, SaveLoad)
│   ├── effects/                      # Hệ thống hạt linh lực (ParticleSystem)
│   └── ui/                           # Theme (bảng màu & hàm vẽ), Button, Panel
├── tests/                            # Bộ kiểm thử tự động quy tắc cờ và AI
└── third_party/
    ├── nlohmann/json.hpp             # Thư viện JSON header-only
    └── SFML-2.6.1/                   # Thư viện đồ họa & âm thanh SFML
```

---

## 🛠️ Hướng Dẫn Biên Dịch & Khởi Chạy

### Cách 1: Chạy trực tiếp file đã build sẵn
Chỉ cần nhấp đúp vào:
```
build\Release\BachChienTienCuc.exe
```

### Cách 2: Mở và chạy bằng Visual Studio 2022
1. Mở Visual Studio 2022, chọn **Open a local folder** và trỏ đến thư mục dự án `DoAnCoTuong`.
2. Hoặc mở file giải pháp `build\BachChienTienCuc.sln`.
3. Chọn cấu hình **Release** và kiến trúc **x64**.
4. Nhấn **F5** (hoặc nút **Local Windows Debugger**) để trải nghiệm trò chơi.

### Cách 3: Biên dịch lại từ dòng lệnh (PowerShell / Command Prompt)
```powershell
# Tạo thư mục build và cấu hình CMake
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -B build -S . -G "Visual Studio 17 2022" -A x64

# Biên dịch toàn bộ
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Release

# Khởi chạy trò chơi
.\build\Release\BachChienTienCuc.exe
```

### Chạy kiểm thử tự động (Unit Tests)
```powershell
.\build\Release\ChessLogicTest.exe
```
Kết quả kiểm thử tự động xác minh:
- [x] Số lượng quân và thế cờ khởi đầu (32 quân, 2 tướng trong cửu cung).
- [x] Luật di chuyển Tốt (trước & sau khi qua sông).
- [x] Luật cản chân Mã (8 hướng L và chân cản).
- [x] Luật ngòi Pháo (nhảy qua đúng 1 quân để ăn).
- [x] Luật cấm lộ mặt Tướng (Flying Generals).
- [x] Phản hồi nước đi của AI Minimax.
- [x] Tính toàn vẹn khi Lưu & Tải JSON.

---

## 🎵 Hướng Dẫn Tùy Chỉnh Âm Thanh & Hình Ảnh

- **Thêm Nhạc Nền**:
  - Thả file nhạc nền menu định dạng `.ogg` hoặc `.wav` vào `assets/sounds/bgm/menu_bgm.ogg`.
  - Thả file nhạc nền ván đấu vào `assets/sounds/bgm/battle_bgm.ogg`.
- **Thêm Hiệu Ứng Âm Thanh**:
  - Thả các file hiệu ứng vào `assets/sounds/sfx/` với tên tương ứng: `select.ogg`, `move.ogg`, `capture.ogg`, `check.ogg`, `victory.ogg`, `defeat.ogg`, `click.ogg`.
- **Thay Đổi Quân Cờ**:
  - Thả file `.png` (kích thước 256×256, nền trong suốt) vào `assets/images/pieces/` với tên tương ứng:
    `red_king.png`, `black_pawn.png`, v.v. Game sẽ tự ưu tiên dùng ảnh của bạn thay cho hình vẽ procedural.
  - Đổi ảnh bàn cờ: thả file vào `assets/images/board/board_bg.png`.

---

## 🎨 Hệ Thống Giao Diện & Đồ Họa

Toàn bộ hình ảnh (bàn cờ, 14 quân cờ, nền huyền ảo, con dấu, hạt sáng) đều được **sinh
bằng thuật toán** trong `src/core/AssetManager.cpp`, không phụ thuộc vào file ảnh ngoài.

- **`src/ui/Theme.h`** giữ bảng màu và các hàm vẽ dùng chung: hình chữ nhật bo góc,
  chuyển sắc dọc, đổ bóng mềm, quầng sáng tỏa tròn. Đổi màu ở đây là đổi cả game.
- **Bàn cờ** được vẽ ở độ phân giải gấp đôi rồi thu nhỏ nên rất sắc nét, gồm khung gỗ có
  vân, mặt bàn gỗ sáng, viền kim tuyến, hoa văn 4 góc, con sông **楚 河 / 漢 界**, đường chéo
  Cửu Cung, hoa văn định vị ở điểm Pháo và điểm Binh/Tốt, cùng toạ độ cột hai bên.
- **Quân cờ** có bóng đổ, vành màu theo phe, khối nổi, chữ Hán khắc nổi và vòng kim tuyến riêng cho Tướng.
- **Nền huyền ảo** dùng chung cho cả 7 màn hình: trời đêm chuyển sắc, sao trời, ba dải núi xa, vignette.

### Chữ Hán và chữ tiếng Việt dùng hai font khác nhau

Font thư pháp Hán tự (KaiTi) **không có** các ký tự Latin có dấu của tiếng Việt
(ă, â, đ, ơ, ư, ế, ộ…). Vì vậy game nạp **hai font tách biệt**:

| Font | Dùng cho | Nạp từ |
|:---|:---|:---|
| Font Hán tự — `getFont()` | 帥 將 楚河 漢界 兵 卒 棋 | `assets/fonts/calligraphy.ttf` |
| Font giao diện — `getUiFont()` | Toàn bộ chữ tiếng Việt có dấu | `assets/fonts/ui.ttf` → `pala.ttf` → `georgia.ttf` → `assets/fonts/default.ttf` → … |

Game tự kiểm tra font có thật sự chứa ký tự tiếng Việt trước khi chọn, nên **chữ tiếng Việt
luôn hiển thị đúng** kể cả khi chạy trên máy khác.

### Chụp ảnh giao diện để báo cáo

```bash
# Windows (PowerShell)
$env:BCT_SCREENSHOT="Preview"; .\build\Release\BachChienTienCuc.exe

# Git Bash
BCT_SCREENSHOT="Preview" ./build/Release/BachChienTienCuc.exe
```
Game sẽ lần lượt dựng cả 7 màn hình và lưu thành `Menu.png`, `Game.png`,
`FactionSelect.png`, `Settings.png`, `Help.png`, `Result.png`, `SaveLoad.png`.

Muốn xuất toàn bộ hình ảnh procedural ra file PNG để chỉnh sửa bằng tay, thêm biến
môi trường `BCT_EXPORT_PLACEHOLDERS=1`.

> 📖 Xem **`TONG_HOP_DO_AN.md`** để có bản tổng hợp đầy đủ về kiến trúc, danh sách file,
> và chi tiết các lỗi đã được sửa trong đợt thiết kế lại giao diện.

