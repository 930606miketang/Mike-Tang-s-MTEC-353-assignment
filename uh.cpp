/**
 * @file main.cpp
 * @brief C++ 物件導向 (OOP) 與多型音訊效果器架構範例
 * @note 適用於期中考 Live Coding 或 C++ 基礎練習
 */

#include <iostream>
#include <string>
#include <iomanip> // 用於控制小數點位數 (std::fixed, std::setprecision)

// =============================================================
// 1. 基礎類別 (Base Class) - 規定所有效果器的共同介面
// =============================================================
class AudioEffect {
protected:
    std::string effectName; // 效果器名稱 (讓子類別存取)
    bool enabled;           // 開關狀態 (true = 開啟, false = 關閉)

public:
    // 建構子：初始化名稱與預設為開啟狀態
    AudioEffect(std::string name) : effectName(name), enabled(true) {}

    // 虛擬解構子：確保透過父類別指標刪除物件時，子類別能正確釋放記憶體（防記憶體洩漏）
    virtual ~AudioEffect() {
        std::cout << "Destroying effect: " << effectName << std::endl;
    }

    // 純虛擬函數 (Pure Virtual Function)：強迫所有子類別必須實作此方法
    virtual void process() = 0;
    virtual void printSettings() const = 0;

    // 一般成員函數：切換開關狀態
    void toggle() { 
        enabled = !enabled;
        std::cout << effectName << " is now " << (enabled ? "enabled" : "disabled") << std::endl;
    }

    // Getter 函數
    bool isEnabled() const { return enabled; }
    std::string getName() const { return effectName; }
};

// =============================================================
// 2. 衍生類別一：殘響效果器 (ReverbEffect)
// =============================================================
class ReverbEffect : public AudioEffect {
private:
    float roomSize; // 房型大小參數 (封裝：限制在 0.0 到 1.0)

public:
    // 建構子：呼叫父類別建構子傳入名稱，並透過 Setter 初始化數值
    ReverbEffect(float size) : AudioEffect("Reverb") {
        setRoomSize(size);
    }

    // Setter：包含數值驗證（防呆機制）
    void setRoomSize(float size) {
        if (size < 0.0f) {
            roomSize = 0.0f;
        } else if (size > 1.0f) {
            roomSize = 1.0f;
        } else {
            roomSize = size;
        }
    }

    // Getter
    float getRoomSize() const {
        return roomSize;
    }

    // 實作父類的 process()
    void process() override {
        if (enabled) {
            std::cout << "Processing reverb with room size: " << roomSize << std::endl;
        } else {
            std::cout << "Reverb bypassed" << std::endl;
        }
    }

    // 實作父類的 printSettings()
    void printSettings() const override {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Effect: " << effectName << ", Room Size: " << roomSize << std::endl;
    }
};

// =============================================================
// 3. 衍生類別二：破音效果器 (DistortionEffect)
// =============================================================
class DistortionEffect : public AudioEffect {
private:
    int driveAmount; // 破音強度 (封裝：限制在 0 到 100)

public:
    // 建構子
    DistortionEffect(int drive) : AudioEffect("Distortion") {
        setDrive(drive);
    }

    // Setter：包含數值驗證
    void setDrive(int drive) {
        if (drive < 0) {
            driveAmount = 0;
        } else if (drive > 100) {
            driveAmount = 100;
        } else {
            driveAmount = drive;
        }
    }

    // Getter
    int getDrive() const {
        return driveAmount;
    }

    // 實作父類的 process()
    void process() override {
        if (enabled) {
            std::cout << "Applying distortion with drive: " << driveAmount << "%" << std::endl;
        } else {
            std::cout << "Distortion bypassed" << std::endl;
        }
    }

    // 實作父類的 printSettings()
    void printSettings() const override {
        std::cout << "Effect: " << effectName << ", Drive: " << driveAmount << "%" << std::endl;
    }
};

// =============================================================
// 4. 主程式 (Main Entry Point) - 展示多型與記憶體管理
// =============================================================
int main() {
    std::cout << "=== Audio Effect System Test ===" << std::endl;

    // 1. 動態配置記憶體 (Heap Allocation) 建立物件
    ReverbEffect* reverb = new ReverbEffect(0.7f);
    DistortionEffect* distortion = new DistortionEffect(75);

    // 2. 建立父類別指標陣列（體現多型 Polymorphism）
    AudioEffect* effects[2];
    effects[0] = reverb;
    effects[1] = distortion;

    // 3. 透過迴圈呼叫各自的設定印出函數
    for (int i = 0; i < 2; ++i) {
        effects[i]->printSettings();
    }

    // 4. 切換第一個效果器的開關狀態
    effects[0]->toggle();

    // 5. 執行音訊處理模擬
    for (int i = 0; i < 2; ++i) {
        effects[i]->process();
    }

    // 6. 釋放動態記憶體（避免記憶體洩漏 Memory Leak）
    for (int i = 0; i < 2; ++i) {
        delete effects[i];
    }

    std::cout << "=== Test Complete ===" << std::endl;
    return 0;
}