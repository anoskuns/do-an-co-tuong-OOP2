#pragma once

#include <string>

namespace BachChien {

// Trả về thư mục gốc chứa tài nguyên (thư mục "assets"), tự dò một lần duy nhất.
// Thứ tự dò: biến môi trường BCT_ASSETS -> "assets" cạnh thư mục làm việc ->
// "assets" cạnh file .exe -> các cấp thư mục cha -> đường dẫn gốc lúc biên dịch.
const std::string& assetRoot();

// Ghép đường dẫn tài nguyên tương đối thành đường dẫn đầy đủ.
// Ví dụ: assetPath("fonts/calligraphy.ttf")
std::string assetPath(const std::string& relative);

} // namespace BachChien
