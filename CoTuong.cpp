#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cmath>
#include <cstdlib>
#include <conio.h>

using namespace std;
class BanCo;
// ============================================================
// QuanCo
// ============================================================

class QuanCo
{
protected:
    int hang;
    int cot;

public:
    QuanCo(int hang, int cot) {
        this->hang = hang;
        this->cot = cot;
    }

    virtual ~QuanCo() {}

    int getHang() const {
        return hang;
    }

    int getCot() const {
        return cot;
    }

    void datViTri(int hangMoi, int cotMoi) {
        hang = hangMoi;
        cot = cotMoi;
    }

    virtual bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const = 0;
    virtual string ve() const = 0;
};


// ============================================================
// Xe
// ============================================================

class Xe : public QuanCo
{
public:
    Xe(int hang, int cot) : QuanCo(hang, cot) {}
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    string ve() const override { return "(X)"; }
};


// ============================================================
// Voi
// ============================================================

class Voi : public QuanCo
{
public:
    Voi(int hang, int cot) : QuanCo(hang, cot) {}
    bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const override;
    string ve() const override { 
        return "(V)"; 
    }
};

using namespace std;

class BanCo
{
private:
    static const int SO_HANG = 10;
    static const int SO_COT = 9;
    vector<unique_ptr<QuanCo>> cacQuan;
    QuanCo* quanDangChon;
    int hangConTro;
    int cotConTro;

public:
    BanCo();
    QuanCo* timQuan(int hang, int cot) const;
    bool trongBanCo(int hang, int cot) const;
    bool coQuanTai(int hang, int cot) const;
    bool duongThangTrong(int hang1, int cot1, int hang2, int cot2) const;
    void ve() const;
    void xuLyEnter();
    void chay();
};


// ============================================================
// Constructor
// ============================================================

BanCo::BanCo() {
    quanDangChon = nullptr;

    // Con tro bat dau o Xe ben trai
    hangConTro = 9;
    cotConTro = 0;

    // Hai quan Xe
    cacQuan.push_back(make_unique<Xe>(9, 0));
    cacQuan.push_back(make_unique<Xe>(9, 8));

    // Hai quan Voi
    cacQuan.push_back(make_unique<Voi>(9, 2));
    cacQuan.push_back(make_unique<Voi>(9, 6));
}


// ============================================================
// Tim quan co tai mot vi tri
// ============================================================

QuanCo* BanCo::timQuan(int hang, int cot) const
{
    for (const auto& quan : cacQuan) {
        if (quan->getHang() == hang && quan->getCot() == cot) {
            return quan.get();
        }
    }

    return nullptr;
}


// ============================================================
// Kiem tra toa do co nam tren ban co hay khong
// ============================================================

bool BanCo::trongBanCo(int hang, int cot) const {
    return hang >= 0 && hang < SO_HANG && cot >= 0 && cot < SO_COT;
}


// ============================================================
// Kiem tra tai vi tri co quan co hay khong
// ============================================================

bool BanCo::coQuanTai(int hang, int cot) const {
    return timQuan(hang, cot) != nullptr;
}


// ============================================================
// Kiem tra duong thang giua hai diem co bi chan khong
// Dung cho quan Xe
// ============================================================

bool BanCo::duongThangTrong(int hang1, int cot1, int hang2, int cot2) const {
    // Di ngang
    if (hang1 == hang2) {
        int batDau, ketThuc;        
        if (cot1 < cot2) {
            batDau = cot1 + 1;
            ketThuc = cot2;
        }
        else {
            batDau = cot2 + 1;
            ketThuc = cot1;
        }
        for (int cot = batDau; cot < ketThuc; cot++) {
            if (coQuanTai(hang1, cot)) return false;
        }

        return true;
    }

    // Di doc
    if (cot1 == cot2) {
        int batDau, ketThuc;
        if (hang1 < hang2) {
            batDau = hang1 + 1;
            ketThuc = hang2;
        }
        else {
            batDau = hang2 + 1;
            ketThuc = hang1;
        }

        for (int hang = batDau; hang < ketThuc; hang++) {
            if (coQuanTai(hang, cot1)) return false;
        }

        return true;
    }

    return false;
}


// ============================================================
// Ve ban co
// ============================================================

void BanCo::ve() const {
    std::cout << "\n";
    std::cout << "      ";
    for (int cot = 0; cot < SO_COT; cot++) {
        std::cout << cot << "       ";
    }
    std::cout << "\n\n";
    for (int hang = 0; hang < SO_HANG; hang++) {
        std::cout << hang << "   ";
        for (int cot = 0; cot < SO_COT; cot++) {
            QuanCo* quan = timQuan(hang, cot);
            bool laConTro = hang == hangConTro && cot == cotConTro;
            // ========================================
            // Co quan co tai vi tri nay
            // ========================================
            if (quan != nullptr) {
                // Quan dang duoc chon
                if (quan == quanDangChon) {
                    if (laConTro) std::cout << "{" << quan->ve() << "}";                    
                    else std::cout << "[" << quan->ve() << "]";                    
                }

                // Quan binh thuong
                else {
                    if (laConTro) std::cout << "{" << quan->ve() << "}";                    
                    else std::cout << " " << quan->ve() << " ";                    
                }
            }

            // ========================================
            // O trong
            // ========================================

            else {
                if (laConTro) std::cout << " {+} ";
                else std::cout << "  +  ";
            }
            // Noi ngang
            if (cot < SO_COT - 1) std::cout << "---";            
        }
        std::cout << "\n";
        // Noi doc
        if (hang < SO_HANG - 1) {
            std::cout << "    ";
            for (int cot = 0; cot < SO_COT; cot++) {
                std::cout << "  |  ";
                if (cot < SO_COT - 1) std::cout << "   ";
            }
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}


// ============================================================
// Xu ly khi nhan Enter
// ============================================================

void BanCo::xuLyEnter()
{
    QuanCo* quanTaiConTro = timQuan(hangConTro, cotConTro);

    // ========================================================
    // Neu con tro dang nam tren mot quan
    // -> chon quan do
    // ========================================================

    if (quanTaiConTro != nullptr) {
        quanDangChon = quanTaiConTro;
        return;
    }

    // ========================================================
    // Neu chua chon quan nao
    // -> khong lam gi
    // ========================================================

    if (quanDangChon == nullptr) return;    

    // ========================================================
    // Da chon quan + con tro dang nam tren o trong
    // -> thu di chuyen
    // ========================================================

    if (quanDangChon->kiemTraNuocDi(*this, hangConTro, cotConTro)) {
        quanDangChon->datViTri(hangConTro, cotConTro);
        // Di xong thi bo chon
        quanDangChon = nullptr;
    }

    // Neu nuoc di sai:
    // khong lam gi ca
}


// ============================================================
// Vong lap chinh
// ============================================================

void BanCo::chay()
{
    while (true)
    {
        system("cls");

        ve();

        std::cout << "=================================\n";
        std::cout << "W A S D : Di chuyen con tro\n";
        std::cout << "ENTER   : Chon quan / di chuyen\n";
        std::cout << "ESC     : Bo chon quan\n";
        std::cout << "Q       : Thoat\n";
        std::cout << "=================================\n";

        std::cout << "Con tro: (" << hangConTro << ", " << cotConTro << ")\n";

        if (quanDangChon != nullptr) {
            std::cout << "Quan dang chon: " << quanDangChon->ve() << " tai (" << quanDangChon->getHang() << ", " << quanDangChon->getCot() << ")\n";
        }
        else  std::cout << "Chua chon quan nao.\n";        

        int phim = _getch();

        switch (phim) {
            // Len            
        case 'w':
        case 'W':
            if (hangConTro > 0) hangConTro--;            
            break;
            // Xuong            
        case 's':
        case 'S':
            if (hangConTro < SO_HANG - 1) hangConTro++;            
            break;
            // Trai            
        case 'a':
        case 'A':
            if (cotConTro > 0) cotConTro--;            
            break;
            // Phai            
        case 'd':
        case 'D':
            if (cotConTro < SO_COT - 1) cotConTro++;            
            break;
            // ENTER            
        case 13:
            xuLyEnter();
            break;
            // ESC            
        case 27:
            quanDangChon = nullptr;
            break;
            // Thoat            
        case 'q':
        case 'Q':
            return;
        }
    }
}

bool Xe::kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const {
    if (!banCo.trongBanCo(hangMoi, cotMoi)) return false;
    if (hangMoi == hang && cotMoi == cot) return false;
    // Xe chi di ngang hoac doc
    if (hangMoi != hang && cotMoi != cot) return false;
    if (banCo.coQuanTai(hangMoi, cotMoi)) return false;
    if (!banCo.duongThangTrong(hang, cot, hangMoi, cotMoi)) return false;    
    return true;
}
bool Voi::kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const {
    if (!banCo.trongBanCo(hangMoi, cotMoi)) return false;
    int dh = abs(hangMoi - hang), dc = abs(cotMoi - cot);
    // Voi di cheo dung 2 o
    if (dh != 2 || dc != 2) return false;
    // Khong vuot song
    if (hangMoi < 5) return false;
    if (banCo.coQuanTai(hangMoi, cotMoi)) return false;
    // Mat voi
    int hangGiua = (hang + hangMoi) / 2, cotGiua = (cot + cotMoi) / 2;
    if (banCo.coQuanTai(hangGiua, cotGiua)) return false;
    return true;
}


// ============================================================
// main
// ============================================================

int main() {
    BanCo banCo;

    banCo.chay();

    return 0;
}